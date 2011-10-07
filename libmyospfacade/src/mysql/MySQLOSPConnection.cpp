/* Copyright (C) 2010 CodeFutures Corporation

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <list>

#include <time.h>
#include <unistd.h>

#include <mysql/MySQLOSPConnection.h>
#include <mysql/MySQLOSPResultSet.h>
#include <mysql/MySQLConnMap.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPCreateStatementResponse.h>

#include <logger/Logger.h>
#include <util/Util.h>

#include <mysql.h>
#include <my_global.h>
#include <mysql_com.h>
#include <m_ctype.h>

using namespace std;
using namespace logger;
using namespace util;

#define LOG_METHOD_CALLS false

Logger MySQLOSPConnection::log = Logger::getLogger("MySQLOSPConnection");

MySQLOSPConnection::MySQLOSPConnection(string host, int port, string database, string user, string password, MySQLConnMap *mysqlResourceMap) {

    this->mysqlResourceMap = mysqlResourceMap;

    ospConn = new OSPTCPConnection(host, port==0 ? 4545 : port);
    
    //TODO: combine these 2 OSP calls into a single call for improved performance

    // connect to OSP server via TCP
    OSPConnectRequest request(database, user, password);
    OSPWireResponse* wireResponse = dynamic_cast<OSPWireResponse*>(ospConn->sendMessage(&request, true));
    //log.info(("wireResponse = ") + Util::toString((void*)wireResponse));
    OSPConnectResponse* response = dynamic_cast<OSPConnectResponse*>(wireResponse->getResponse());
    //log.info(("response = ") + Util::toString((void*)response));
    connID = response->getConnID();

    // close TCP connection
    ospConn->stop();
    ospConn = NULL;

    // now connect via named pipes
    ospConn = new OSPNamedPipeConnection(response->getRequestPipeFilename(), response->getResponsePipeFilename());

    // delete the response now we have all the info from it
    delete wireResponse;

    // create a statement that we will re-use with this connection
    OSPCreateStatementRequest request2(connID);
    wireResponse = dynamic_cast<OSPWireResponse*>(ospConn->sendMessage(&request2, true));
    OSPCreateStatementResponse* response2 = dynamic_cast<OSPCreateStatementResponse*>(wireResponse->getResponse());
    stmtID = response2->getStmtID();
    delete wireResponse;

    my_sqlstate = "00000";
    my_errno = 0;
    my_error = NULL;

    nextResultSetID = 1;
    resultSetID = -1;

    affectedRows = 0;
    fieldCount = 0;

    pid = getpid();

}

void MySQLOSPConnection::setError(const char *sqlstate, int _errno, const char *_error) {
    if (log.isDebugEnabled()) {
        log.debug(string("setError(") + string(sqlstate==NULL?"NULL":sqlstate) + string(",") + Util::toString(_errno) + string(",") + string(_error==NULL?"NULL":_error));
    }
    my_sqlstate = sqlstate;
    my_errno = _errno;
    my_error = (char *) _error;
}

MySQLOSPConnection::~MySQLOSPConnection() {
    if (ospConn) {
        delete ospConn;
    }
}

string MySQLOSPConnection::getLogPrefix(MYSQL *mysql) {
    string ret = "[mysql=" + Util::toString(mysql) + "] ";
    return ret;
}

bool MySQLOSPConnection::connect(const char *, const char *, const char *,
        const char *, unsigned int port, const char *unix_socket,
        unsigned long clientflag) {
    log.error("Call to connect() in ODBC connection");
    return false;
}

// MySQL API methods

my_bool MySQLOSPConnection::mysql_autocommit(MYSQL *mysql, my_bool autocommit) {
    if (autocommit) {
        mysql_real_query(mysql, "SET autocommit=1", 16);
    }
    else {
        mysql_real_query(mysql, "SET autocommit=0", 16);
    }
    return 0;
}

MYSQL_STMT * MySQLOSPConnection::mysql_stmt_init(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_init");
    return NULL;
}

int MySQLOSPConnection::mysql_stmt_prepare(MYSQL_STMT *_stmt,
        const char *query, unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_prepare");
    return -1;
}

int MySQLOSPConnection::mysql_stmt_execute(MYSQL_STMT *_stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_execute");
    return -1;
}

my_bool MySQLOSPConnection::mysql_more_results(MYSQL *_mysql) {
    my_bool ret = false;
    return ret;
}

int MySQLOSPConnection::mysql_query(MYSQL *mysql, const char *sql) {
    return mysql_real_query(mysql, sql, strlen(sql));
}

int MySQLOSPConnection::mysql_real_query(MYSQL *mysql, const char *sql, unsigned long length) {

    if (log.isTraceEnabled()) {
        log.trace(string("MySQLOSPConnection::mysql_real_query(") + Util::toString(mysql) + string(", ") + string(sql) + string(")"));
    }

    if (pid != getpid()) {
        log.warn(
            "Connection was created in process " + Util::toString((long)pid)
            + " but mysql_real_query() called from process " + Util::toString((long)getpid())
        );
    }

    if (strlen(sql)>9 && Util::_strnicmp("SET NAMES", sql, 9)==0) {
        // ignore this SQL because it is not supported by MySQL ODBC, pretend it worked through
        log.warn("Ignoring 'SET NAMES' query");
        return 0;
    }

    // allocate the next resultSetID
    resultSetID = nextResultSetID++;

    // reset error state
    my_sqlstate = "00000";
    my_errno = 0;
    my_error = NULL;

    int ret = -1;

    try {
        if (log.isDebugEnabled()) {
            log.debug(string("Sending query to OSP proxy. ConnID = ") + connID + string("; SQL=") + string(sql));
        }
        OSPExecuteRequest request(connID, stmtID, string(sql));
        OSPWireResponse *wireResponse = dynamic_cast<OSPWireResponse*>(ospConn->sendMessage(&request, true));
        OSPExecuteResponse *executeResponse = dynamic_cast<OSPExecuteResponse*>(wireResponse->getResponse());
        resultSetID = executeResponse->getResultSetID();
        fieldCount = executeResponse->getResultSetColumnCount();
        affectedRows = executeResponse->getUpdateCount();

        if (executeResponse->getErrorCode()) {

            log.error(
                string("Failed to execute query. ")
                + string("Error Code: ") + Util::toString(executeResponse->getErrorCode())
                + string("; Error Text: ") + executeResponse->getErrorMessage()
                + string("; ConnID = ") + connID + string("; SQL=") + string(sql)
            );

            resultSetID = 0;
            affectedRows = 0;
            fieldCount = 0;

            my_errno = executeResponse->getErrorCode();
			my_error = "Query failed due to OSP error. See log for details";
        }
        else {
            // success

            log.debug(string("Query ran OK. Field count: ") + Util::toString(fieldCount));

            ret = 0;
        }

        delete executeResponse;

    }
    catch (...) {
        log.error(string("Failed to execute query [unhandled exception]. ConnID = ") + connID + string("; SQL=") + string(sql));
        resultSetID = 0;
        affectedRows = 0;
        fieldCount = 0;
        my_errno = 9999;
        my_error = "Query failed due to OSP error";
    }

    return ret;
}

int MySQLOSPConnection::mysql_send_query(MYSQL *mysql, const char *q,
        unsigned long length) {
    log.debug("mysql_send_query (NOT SUPPORTED)");
    return -1;
}

char *MySQLOSPConnection::ensureCapacity(char *buffer, unsigned int *length, unsigned int capacity) {

    if (*length >= capacity) {
        return buffer;
    }


    // re-size buffer
    unsigned int newLength = capacity + 512;

    log.warn(string("ensureCapacity is resizing buffer from ") + Util::toString((int)(*length)) + " to " + Util::toString((int)newLength));

    char *ret = new char[newLength];
    memcpy(ret, buffer, *length); //TODO: would only need to copy as far as offset?
    delete [] buffer;

    *length = newLength;

    return ret;
}

MYSQL_RES * MySQLOSPConnection::mysql_store_result(MYSQL *mysql) {

    if (resultSetID==0) {
        //log.warn("mysql_store_result() fetching empty remote result set for CRUD query!");
        return NULL;
    }

    // create native MySQL result set structure
    currentRes = new MYSQL_RES();
    memset(currentRes, 0, sizeof(MYSQL_RES));

    if (log.isTraceEnabled()) {
        log.trace(string("mysql_store_result(") + Util::toString((void*)mysql)
             + string(") creating MYSQL_RES* ")
             + Util::toString((void*)currentRes)
            );
    }

    // initialize variables before results start streaming in
    currentRow = NULL;
    prevRow = NULL;

    // fetch results from OSP server
    try {
        OSPResultSetRequest request(connID, stmtID, resultSetID);
        ospConn->sendMessage(&request, true, this);
    }
    catch (...) {
        log.error("mysql_store_result() failed to retrieve results from OSP server");
        return NULL;
    }

    // finalize result set structure, now that all response messages have been received and processed (in
    // calls to processMessage()).
    currentRes->handle = mysql;
    currentRes->eof = 1;

    // store result set
    mysqlResourceMap->setResultSet(currentRes, new MySQLOSPResultSet(this));

    // return the result set
    return currentRes;
}

void MySQLOSPConnection::processMessage(OSPMessage *message) {

    MYSQL_RES *res = currentRes;

    // cast to expected message type
    OSPWireResponse *wireResponse = dynamic_cast<OSPWireResponse *>(message);
    OSPResultSetResponse *response = dynamic_cast<OSPResultSetResponse *>(wireResponse->getResponse());

    // how many columns?
    int columnCount = response->getColumnCount();

    if (log.isTraceEnabled()) {
        log.trace(string("Result set has ") + Util::toString((int)res->field_count) + string(" column(s)"));
    }

    bool firstMessage = currentRow==NULL;

    if (firstMessage) {

        res->lengths = new unsigned long[columnCount];
        memset(res->lengths, 0, sizeof(unsigned long[columnCount]));

        res->field_count = columnCount;
        res->row_count = 0;
        res->current_field = 0;

        // fields
        res->fields = new MYSQL_FIELD[res->field_count];
        memset(res->fields, 0, res->field_count * sizeof(MYSQL_FIELD));

        char *emptyString = Util::createString("");

        for (unsigned int i = 0; i < res->field_count; i++) {

            /*
             typedef struct st_mysql_field {
             char *name;                 // Name of column
             char *org_name;             // Original column name, if an alias
             char *table;                // Table of column if column was a field
             char *org_table;            // Org table name, if table was an alias
             char *db;                   // Database for table
             char *catalog;              // Catalog for table
             char *def;                  // Default value (set by mysql_list_fields)
             unsigned long length;       // Width of column (create length)
             unsigned long max_length;   // Max width for selected set
             unsigned int name_length;
             unsigned int org_name_length;
             unsigned int table_length;
             unsigned int org_table_length;
             unsigned int db_length;
             unsigned int catalog_length;
             unsigned int def_length;
             unsigned int flags;         // Div flags
             unsigned int decimals;      // Number of decimals in field
             unsigned int charsetnr;     // Character set
             enum enum_field_types type; // Type of field. See mysql_com.h for types
             } MYSQL_FIELD;
             */

            OSPString *columnName = response->getColumnNames()[i];
            int jdbcType = response->getColumnTypes()[i];
            int columnNameLength = columnName->getLength();
            if (columnNameLength<1) {
                res->fields[i].name = "";
            }
            else {
                res->fields[i].name = new char[columnNameLength + 1];
                memcpy(res->fields[i].name, (const char *) columnName->getBuffer(), columnNameLength); //TODO: avoid this copy
                res->fields[i].name[columnNameLength] = '\0';
            }

            res->fields[i].name_length = columnNameLength;
            res->fields[i].length = 16; //TODO: get from response object

            // set the data type
            res->fields[i].type = MYSQL_TYPE_VARCHAR;

            bool TRACE = true;		
            const char *odbcColumnName = columnName->getBuffer();

            switch (jdbcType) {
                case JDBC_BLOB:
                    res->fields[i].type = MYSQL_TYPE_BLOB;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_BLOB --> MYSQL_TYPE_BLOB"));
                    break;
                case JDBC_BINARY:
                    res->fields[i].type = MYSQL_TYPE_BLOB;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_BINARY --> MYSQL_TYPE_BLOB"));
                    break;
                case JDBC_VARBINARY:
                    res->fields[i].type = MYSQL_TYPE_BLOB;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_VARBINARY --> MYSQL_TYPE_BLOB"));
                    break;
                case JDBC_LONGVARBINARY:
                    res->fields[i].type = MYSQL_TYPE_BLOB;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_LONGVARBINARY --> MYSQL_TYPE_BLOB"));
                    break;
                case JDBC_DATE:
                    res->fields[i].type = MYSQL_TYPE_DATE;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_TYPE_DATE --> MYSQL_TYPE_DATE"));
                    break;
                case JDBC_TIME:
                    res->fields[i].type = MYSQL_TYPE_DATETIME;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_TIME --> MYSQL_TYPE_DATETIME"));
                    break;
                case JDBC_TIMESTAMP:
                    res->fields[i].type = MYSQL_TYPE_DATETIME;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_DATETIME --> MYSQL_TYPE_DATETIME"));
                    break;
                case JDBC_DECIMAL:
                case JDBC_NUMERIC:
                    res->fields[i].type = MYSQL_TYPE_DECIMAL;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_DECIMAL --> MYSQL_TYPE_DECIMAL"));
                    break;
                case JDBC_BIT:
                case JDBC_TINYINT:
                    res->fields[i].type = MYSQL_TYPE_TINY;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_BIT --> MYSQL_TYPE_TINY"));
                    break;
                case JDBC_SMALLINT:
                case JDBC_BIGINT:
                case JDBC_INTEGER:
                    res->fields[i].type = MYSQL_TYPE_INT24;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_INTEGER --> MYSQL_TYPE_INT24"));
                    break;
                case JDBC_REAL:
                case JDBC_FLOAT:
                    res->fields[i].type = MYSQL_TYPE_FLOAT;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_FLOAT --> MYSQL_TYPE_FLOAT"));
                    break;
                case JDBC_DOUBLE:
                    res->fields[i].type = MYSQL_TYPE_DOUBLE;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_DOUBLE --> MYSQL_TYPE_DOUBLE"));
                    break;
                case JDBC_CHAR:
                case JDBC_VARCHAR:
                case JDBC_LONGVARCHAR:
                    res->fields[i].type = MYSQL_TYPE_VARCHAR;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_VARCHAR --> MYSQL_TYPE_VARCHAR"));
                    break;
                case JDBC_CLOB:
                    res->fields[i].type = MYSQL_TYPE_VARCHAR;
                    if (TRACE) log.trace(string("Column ") + string((const char *)odbcColumnName) + string(" JDBC_CLOB --> MYSQL_TYPE_VARCHAR"));
                    break;
                default:
                    res->fields[i].type = MYSQL_TYPE_VARCHAR;
                    log.warn(string("Column ") + string((const char *)odbcColumnName)
                            + string(" used UNKNOWN JDBC datatype (") + Util::toString((int)jdbcType) + string(") --> MYSQL_TYPE_VARCHAR"));
                    break;
            }

            res->fields[i].org_name = emptyString;
            res->fields[i].table = emptyString;
            res->fields[i].org_table = emptyString;
            res->fields[i].db = emptyString;
            res->fields[i].catalog = emptyString;
            res->fields[i].def = emptyString;
            res->fields[i].max_length = 0; // this must be set to the max length of any data item in the current result set
        }

        // create data structure
        res->data = new MYSQL_DATA();
        res->data->rows = 0;
        res->data->fields = columnCount;
        res->data->data = NULL; // populate this later
    }

    /*
     typedef struct st_mysql_res {
     my_ulonglong row_count;
     MYSQL_FIELD   *fields;
     MYSQL_DATA    *data;
     MYSQL_ROWS    *data_cursor;
     unsigned long *lengths;               // column lengths of current row
     MYSQL         *handle;                // for unbuffered reads
     MEM_ROOT      field_alloc;
     unsigned int  field_count, current_field;
     MYSQL_ROW     row;                    // If unbuffered read
     MYSQL_ROW     current_row;            // buffer to current row
     my_bool       eof;                    // Used by mysql_fetch_row
     my_bool       unbuffered_fetch_cancelled;
     const struct st_mysql_methods *methods;
     } MYSQL_RES;
     */

    // iterate over result set
    list<OSPString**> *resultRows = response->getResultRows();
    list<OSPString**>::iterator it;
    for (it=resultRows->begin(); it!=resultRows->end(); it++) {

        OSPString **currentRowData = *it;

        /*
         typedef struct st_mysql_data {
         my_ulonglong rows;
         unsigned int fields;
         MYSQL_ROWS *data;
         MEM_ROOT alloc;
         struct embedded_query_result *embedded_info;
         } MYSQL_DATA;
         */

        // create new native row structure
        currentRow = new MYSQL_ROWS();
        currentRow->length = 0;
        currentRow->data = new char*[columnCount];

        // on the first row, mark this as the result set data
        if (res->data_cursor == NULL) {
            res->data_cursor = currentRow;
            res->data->data = currentRow;
        }

        // create buffer to store data for entire row in a contiguous block
        unsigned int rowDataSize = 0;
        int col;
        //log.info("CALC row length");
        for (col = 1; col <= columnCount; col++) {
            rowDataSize += currentRowData[col-1] ? currentRowData[col-1]->getLength() : 0;
            rowDataSize += 1; // null terminator
            //log.info(string("interim row data size now is ") + Util::toString((int)rowDataSize));
        }

        //log.info(string("TOTAL row data size is ") + Util::toString((int)rowDataSize));

        unsigned int rowDataOffset = 0;
        char *rowData = new char[rowDataSize];

        // temporary array for storing length of each column
        int rowDataLength[columnCount];

        // fetch each field's value as a string
        for (col = 1; col <= columnCount; col++) {

            unsigned int l = currentRowData[col-1] ? currentRowData[col-1]->getLength() : 0;

            // ensure the buffer is large enough to store this data plus a null terminator
            rowData = ensureCapacity(rowData, &rowDataSize, rowDataOffset+l+1);

            // calculate pointer to offset where we will store the data
            char *fieldValue = rowData + rowDataOffset;

            if (currentRowData[col-1]) {
                // store data
                memcpy(fieldValue, currentRowData[col-1]->getBuffer(), l);
                rowDataOffset += l;

                // store length (excluding null terminator)
                rowDataLength[col-1] = l;

            }
            else {
                // store -1 to indicate a NULL field
                rowDataLength[col-1] = l;
            }

            // we always store a null terminator after the data just to be safe
            rowData[rowDataOffset++] = '\0';

            // update max length
            if (l > res->fields[col - 1].max_length) {
                res->fields[col - 1].max_length = l;
            }
        }

        // store char** pointers based on rowDataLength array
        unsigned int tempOffset = 0;
        for (col=1; col<=columnCount; col++) {

            if (rowDataLength[col-1] == -1) {
                // NULL
                currentRow->data[col-1] = NULL;
            }
            else {

                // store pointer to data for this column
                currentRow->data[col-1] = rowData + tempOffset;

                // increase offset
                tempOffset += rowDataLength[col-1] + 1; // add 1 for each null terminator

                // calculate total row length
                currentRow->length += rowDataLength[col-1] + 1; // add 1 for each null terminator
            }
        }


        if (prevRow != NULL) {
            prevRow->next = currentRow;
        }
        prevRow = currentRow;

        res->row_count++;
        res->data->rows++;
    }

    // memory cleanup
    delete wireResponse;
}

void MySQLOSPConnection::mysql_free_result(MYSQL_RES *res) {

    if (log.isTraceEnabled()) {
        log.trace("mysql_free_result");
    }

    if (res != NULL) {

        // iterate through result set rows
        res->data_cursor = res->data->data;
        while (res->data_cursor) {

            // get pointer to row
            MYSQL_ROWS *currentRow = res->data_cursor;
            MYSQL_ROW tmp = res->data_cursor->data;

            // delete single buffer that contains all row data
            delete [] tmp[0];

            // delete array of pointers to offsets within the row data
            delete [] tmp;

            // go to next row
            res->data_cursor = res->data_cursor->next;

            // delete row
            delete currentRow;
        }

        // delete field meta-data
        for (unsigned short i = 0; i < res->field_count; i++) {
            if (res->fields[i].name) {
                delete [] res->fields[i].name;
            }
        }
        delete [] res->fields;
        delete [] res->lengths;

        delete res;
        res = NULL;
    }

}

MYSQL_RES * MySQLOSPConnection::mysql_use_result(MYSQL *mysql) {
    log.error("mysql_use_result not implemented with OSP yet!");
    return NULL;

//
//    if (log.isDebugEnabled()) log.debug(getLogPrefix(mysql) + "In mysql_use_result()");
//
//    // create native MySQL result set structure
//    res = new MYSQL_RES();
//    memset(res, 0, sizeof(MYSQL_RES));
//
//    // how many columns?
//    SQLSMALLINT columns;
//
//    SQLNumResultCols(stmt, &columns);
//    int columnCount = columns;
//
//    res->field_count = columns;
//    res->row_count = 0;
//    res->current_field = 0;
//
//    // fields
//    res->fields = new MYSQL_FIELD[res->field_count];
//    memset(res->fields, 0, res->field_count * sizeof(MYSQL_FIELD));
//
//    char *emptyString = Util::createString("");
//
//    SQLCHAR odbcColumnName[64];
//    SQLSMALLINT odbcNameLength;
//    SQLSMALLINT odbcDataType;
//    SQLULEN odbcColumnSize;
//    SQLSMALLINT odbcDecimalDigits;
//    SQLSMALLINT odbcNullable;
//
//    for (unsigned int i = 0; i < res->field_count; i++) {
//        SQLRETURN ret = SQLDescribeCol(stmt, i + 1, odbcColumnName,
//                sizeof(odbcColumnName), &odbcNameLength, &odbcDataType,
//                &odbcColumnSize, &odbcDecimalDigits, &odbcNullable);
//
//        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
//            log.error("SQLDescribeCol FAILED");
//            setODBCError(SQL_HANDLE_STMT);
//            return NULL;
//        }
//
//        res->fields[i].name = new char[odbcNameLength + 1];
//        strcpy(res->fields[i].name, (const char *) odbcColumnName);
//        res->fields[i].name_length = odbcNameLength;
//        res->fields[i].length = odbcColumnSize;
//        res->fields[i].type = MYSQL_TYPE_VARCHAR;
//
//        res->fields[i].org_name = emptyString;
//        res->fields[i].table = emptyString;
//        res->fields[i].org_table = emptyString;
//        res->fields[i].db = emptyString;
//        res->fields[i].catalog = emptyString;
//        res->fields[i].def = emptyString;
//        res->fields[i].max_length = 0;
//
//    } // end of for loop
//
//    // create data structure
//    res->data = new MYSQL_DATA();
//    res->data->rows = 0;
//    res->data->fields = columns;
//    res->data->data = NULL; // populate this later
//
//    // finalize result set structure
//    res->handle = mysql;
//    res->eof = 0;
//    res->lengths = new unsigned long[columnCount];
//    memset(res->lengths, 0, sizeof(unsigned long[columnCount]));
//
//    // store result
//    mysqlResourceMap->setConnection(res, this);
//
//    // return result set
//    return res;

}//end of use_result

my_bool MySQLOSPConnection::mysql_commit(MYSQL * mysql) {
    return mysql_real_query(mysql, "COMMIT", 6);
}

my_bool MySQLOSPConnection::mysql_rollback(MYSQL * mysql) {
    return mysql_real_query(mysql, "ROLLBACK", 8);
}


void MySQLOSPConnection::mysql_data_seek(MYSQL_RES *result, my_ulonglong row) {
    if (log.isTraceEnabled()) log.trace(string("mysql_data_seek(") + Util::toString((void*)result) + string(", ") + string(Util::toString((long)row)));
    MYSQL_ROWS *tmp = NULL;
    if (result->data) {
        for (tmp = result->data->data; row-- && tmp; tmp = tmp->next) {
            // do nothing
        }
    }
    result->current_row = tmp->data;
    result->data_cursor = tmp;
}

unsigned int MySQLOSPConnection::mysql_field_count(MYSQL *mysql) {

    unsigned int ret = -1;

    if (log.isTraceEnabled()) log.trace(getLogPrefix(mysql) + string("mysql_field_count()"));

    ret = fieldCount;

    if (log.isDebugEnabled()) log.debug(getLogPrefix(mysql) + string("mysql_field_count() returning ") + Util::toString((int)ret));

    return ret;
}

unsigned int MySQLOSPConnection::mysql_num_fields(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_num_fields() RES=") + Util::toString((void*)result) + string(" returning ") + Util::toString((int)(result->data->fields)));
    return result->data->fields;
}

MYSQL_FIELD *MySQLOSPConnection::mysql_fetch_fields(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_fetch_fields() RES=") + Util::toString((void*)result));
    return result->fields;
}

my_ulonglong MySQLOSPConnection::mysql_affected_rows(MYSQL *mysql) {
    if (log.isTraceEnabled()) log.trace(getLogPrefix(mysql) + string("mysql_affected_rows()"));
    return affectedRows;
}

my_ulonglong MySQLOSPConnection::mysql_num_rows(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_num_rows() RES=") + Util::toString((void*)result));
    return result->data->rows;
}

unsigned long *MySQLOSPConnection::mysql_fetch_lengths(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_fetch_lengths() RES=") + Util::toString((void*)result)
        + string(" returning ") + Util::toString(result->lengths)
    );

    // dangerous - assumes array has length > 0
    if (log.isTraceEnabled()) log.trace(string("mysql_fetch_lengths() RES=") + Util::toString((void*)result)
        + string(" first length is ") + Util::toString((long)result->lengths[0])
    );

    return result->lengths;
}

MYSQL_ROW MySQLOSPConnection::mysql_fetch_row(MYSQL_RES *res) {

//    bool TRACE = log.isTraceEnabled();
//
//    if (TRACE) {
//        log.trace(string("mysql_fetch_row() RES=") + Util::toString((void*)res));
//    }
//
//    if (!stmt) {
//        return NULL;
//    }

    bool isUseResult = false;

    //determine if the request is useResult or storeResult
    if (!res->eof) {
        isUseResult = true;
    }

    if (isUseResult) {
        log.error("mysql_fetch_row not implemented in OSP yet!");
        return NULL;

//
//        if (TRACE) {
//            log.trace("USE RESULT VERSION (NOT STORE RESULT VERSION");
//        }
//
//        // mysql_use_result version
//
//        SQLLEN indicator;
//
//        long row;
//        char buf[1024];
//        SQLUSMALLINT col;
//        MYSQL_ROWS *currentRow = NULL;
//        SQLSMALLINT columns = res->field_count;
//        //cout << "columns: " << columns << endl;
//
//        if (isSuccess(row = SQLFetch(stmt), "SQLFetch")) {
//            currentRow = new MYSQL_ROWS();
//            currentRow->length = 0;
//            currentRow->data = new char*[columns];
//            // on the first row, mark this as the result set data
//            if (res->data_cursor == NULL) {
//                //cout << "in if 2" << endl;
//                res->data_cursor = currentRow;
//                res->data->data = currentRow;
//            }
//
//            // fetch each field's value as a string
//            for (col = 1; col <= columns; col++) {
//                SQLRETURN ret = SQLGetData(stmt, col, SQL_C_CHAR, buf,
//                        sizeof(buf), &indicator);
//                if (isSuccess(ret, "SQLGetData")) {
//
//                    //TODO: update this based on store_result ... this is out of date
//
//                    unsigned int l = 0;
//                    if (indicator == SQL_NULL_DATA) {
//                        // empty string TODO: seems like it could be more efficient
//                        char *tmp = new char[1];
//                        tmp[0] = '\0';
//                        l = 0;
//                        currentRow->data[col - 1] = tmp;
//                    } else {
//                        // copy the data from the buffer into the native structure
//                        l = indicator; //strlen(buf);
//                        currentRow->data[col - 1] = new char[l + 1];
//                        memcpy(currentRow->data[col - 1], buf, l);
//                        currentRow->data[col - 1][l] = '\0';
//                    }
//                    // update max length
//                    if (l > res->fields[col - 1].max_length) {
//                        res->fields[col - 1].max_length = l;
//                    }
//                }//end of isSuccess if condition
//
//                else {
//                    log.error("SQLGetData FAILED");
//                    setODBCError(SQL_HANDLE_STMT);
//                    return NULL;
//                }//end of else
//
//            }//end of for
//            res->row_count++;
//            res->data->rows++;
//
//            MYSQL_ROW tmp = res->data_cursor->data;
//            res->data_cursor = res->data_cursor->next;
//
//            res->current_row = tmp;
//
//            // calculate fields length for current row
//            for (unsigned short i=0; i<res->data->fields; i++) {
//                res->lengths[i] = res->current_row[i]==NULL ? 0 : strlen(res->current_row[i]);
//            }
//
//            return res->current_row;
//
//        } // end of SQLFetch
//
//        else {
//            return res->current_row = (MYSQL_ROW) NULL;
//        }
//
    }//end of useResult

    else {
        // mysql_store_result version
        if (!res->data_cursor) {
            return res->current_row = (MYSQL_ROW) NULL;
        }

        MYSQL_ROWS *tmp = res->data_cursor;
        res->current_row = tmp->data;

        // calculate fields length for current row
        unsigned int prevColTotalLen = 0;
        for (unsigned short i=0; i<res->data->fields; i++) {

            //log.trace(string("i=") + Util::toString((int)i));

            // is this a NULL field?
            if (res->current_row[i]==NULL) {
                res->lengths[i] = 0;

                //log.trace(string("NULL FIELD"));

            }
            else {

                //log.trace(string("res->current_row[i]=") + Util::toString((void*)res->current_row[i]));

                // find next non-NULL field
                unsigned short nextIndex = i +1;
                char *nextField = NULL;
                while (nextField == NULL && nextIndex<res->data->fields) {
                    nextField = res->current_row[nextIndex];
                    //log.trace(string("nextField=") + Util::toString((void*)nextField));
                    nextIndex++;
                }

                if (nextField==NULL) {
                    // the current field is the last non-NULL field
                    res->lengths[i] = tmp->length - prevColTotalLen - 1;
                    //log.trace(string("prevColTotalLen=") + Util::toString((int*)prevColTotalLen));
                }
                else {
                    res->lengths[i] = nextField - res->current_row[i] - 1; // remove null terminator from length
                }

                prevColTotalLen += res->lengths[i] + 1;
            }

//            if (TRACE) {
//                log.trace(string("column length calculated as ") + Util::toString((int)res->lengths[i]));
//            }
        }

        // move cursor to next row
        res->data_cursor = tmp->next;

        return res->current_row;
    }
}

MYSQL_FIELD_OFFSET MySQLOSPConnection::mysql_field_seek(MYSQL_RES *result,
        MYSQL_FIELD_OFFSET offset) {

    if (log.isTraceEnabled()) log.trace(string("mysql_field_seek() RES=") + Util::toString((void*)result));

    MYSQL_FIELD_OFFSET ret = result->current_field;
    result->current_field = offset;
    return ret;
}

MYSQL_FIELD * MySQLOSPConnection::mysql_fetch_field(MYSQL_RES *result) {

    if (log.isTraceEnabled()) log.trace(string("mysql_fetch_field() RES=") + Util::toString((void*)result));

    if (result->current_field >= result->field_count) {
        // no more fields left
        return NULL;
    }
    return &(result->fields[result->current_field++]);
}

void MySQLOSPConnection::mysql_close(MYSQL *mysql) {
    if (log.isDebugEnabled()) {
        log.debug("mysql_close");
    }

    OSPDisconnectRequest request(connID);
    OSPWireResponse *wireResponse = dynamic_cast<OSPWireResponse*>(ospConn->sendMessage(&request, true));
    if (wireResponse) {
        delete wireResponse;
    }
}

my_bool MySQLOSPConnection::mysql_stmt_bind_param(MYSQL_STMT *_stmt,
        MYSQL_BIND * bnd) {
    throw Util::createException(
            "not implemented - MySQLOSPConnection::mysql_stmt_bind_param");
}

my_bool MySQLOSPConnection::mysql_stmt_close(MYSQL_STMT *_stmt) {
    throw Util::createException(
            "not implemented - MySQLOSPConnection::mysql_stmt_close");
}

int MySQLOSPConnection::mysql_server_init(int argc, char **argv, char **groups) {
    return notImplementedInt("mysql_server_init");
}

void MySQLOSPConnection::mysql_server_end(void) {
    notImplementedInt("mysql_server_end");
}

MYSQL_PARAMETERS * MySQLOSPConnection::mysql_get_parameters(void) {
    notImplementedPtr("mysql_get_parameters");
    return NULL;
}

my_bool MySQLOSPConnection::mysql_thread_init(void) {
    notImplementedPtr("mysql_thread_init");
    return false;
}

void MySQLOSPConnection::mysql_thread_end(void) {
    notImplementedPtr("MySQLOSPConnection");
}

my_bool MySQLOSPConnection::mysql_eof(MYSQL_RES *res) {
    notImplementedPtr("MySQLOSPConnection::mysql_eof");
    return false;
}

MYSQL_FIELD * MySQLOSPConnection::mysql_fetch_field_direct(MYSQL_RES *result,
        unsigned int fieldnr) {

    if (log.isTraceEnabled()) log.trace(string("mysql_fetch_field_direct() RES=") + Util::toString((void*)result));

    MYSQL_FIELD *fields = mysql_fetch_fields(result);
    if (fields == NULL) {
        return NULL;
    }

    return &(fields[fieldnr]);
}

MYSQL_ROW_OFFSET MySQLOSPConnection::mysql_row_tell(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_row_tell() RES=") + Util::toString((void*)result));
    return result->data_cursor;
}

MYSQL_FIELD_OFFSET MySQLOSPConnection::mysql_field_tell(MYSQL_RES *result) {
    if (log.isTraceEnabled()) log.trace(string("mysql_field_tell() RES=") + Util::toString((void*)result));
    return result->current_field;
}

my_ulonglong MySQLOSPConnection::mysql_insert_id(MYSQL *mysql) {

    if (log.isTraceEnabled()) {
        log.trace("mysql_insert_id()");
    }

    return -1;
}

const char * MySQLOSPConnection::mysql_sqlstate(MYSQL *mysql) {
    return my_sqlstate;
}

unsigned int MySQLOSPConnection::mysql_errno(MYSQL *mysql) {
    if (log.isTraceEnabled()) log.trace(string("mysql_errno() returning ") + Util::toString(my_errno));
    return my_errno;
}

const char * MySQLOSPConnection::mysql_error(MYSQL *mysql) {
    return my_error;
}

unsigned int MySQLOSPConnection::mysql_warning_count(MYSQL *mysql) {
    if (log.isDebugEnabled()) {
        notImplementedPtr("MySQLOSPConnection::mysql_warning_count");
    }
    return 0;
}

const char * MySQLOSPConnection::mysql_info(MYSQL *mysql) {
    if (log.isDebugEnabled()) {
        notImplementedPtr("MySQLOSPConnection::mysql_info");
    }
    return NULL;
}

unsigned long MySQLOSPConnection::mysql_thread_id(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_thread_id");
    return 0;
}

const char * MySQLOSPConnection::mysql_character_set_name(MYSQL *mysql) {
    if (log.isDebugEnabled()) {
        if (log.isDebugEnabled()) log.debug(string("mysql_character_set_name() returning hard-coded 'utf8' value!"));
    }
    return "utf8";
}

int MySQLOSPConnection::mysql_set_character_set(MYSQL *mysql, const char *csname) {
    // MySQL ODBC does not support "SET NAMES" syntax. Correct charset must be specified in ODBC ConnectString.
    // return success code anyway
    if (log.isDebugEnabled()) {
        if (log.isDebugEnabled()) log.debug(string("mysql_set_character_set(") + string(csname==NULL?"NULL":csname) + string(") WILL BE IGNORED!"));
    }
    return 0;
}

void MySQLOSPConnection::mysql_get_character_set_info(MYSQL *mysql,
        MY_CHARSET_INFO *charset) {

    if (log.isDebugEnabled()) {
        if (log.isDebugEnabled()) log.debug(string("mysql_get_character_set_info() returning partialy info for 'utf8' char set!"));
    }

    charset->name = "utf8";
}



my_bool MySQLOSPConnection::mysql_ssl_set(MYSQL *mysql, const char *key,
        const char *cert, const char *ca, const char *capath,
        const char *cipher) {
    notImplementedPtr("MySQLOSPConnection::mysql_ssl_set");
    return false;
}

const char * MySQLOSPConnection::mysql_get_ssl_cipher(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_get_ssl_cipher");
    return NULL;
}

my_bool MySQLOSPConnection::mysql_change_user(MYSQL *mysql, const char *user,
        const char *passwd, const char *db) {
    notImplementedPtr("MySQLOSPConnection::mysql_change_user");
    return false;
}

int MySQLOSPConnection::mysql_select_db(MYSQL *mysql, const char *db) {
    notImplementedPtr("MySQLOSPConnection::mysql_select_db");
    return -1;
}

my_bool MySQLOSPConnection::mysql_master_query(MYSQL *mysql, const char *q,
        unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_master_query");
    return false;
}

my_bool MySQLOSPConnection::mysql_master_send_query(MYSQL *mysql,
        const char *q, unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_master_send_query");
    return false;
}

my_bool MySQLOSPConnection::mysql_slave_query(MYSQL *mysql, const char *q,
        unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_slave_query");
    return false;
}

my_bool MySQLOSPConnection::mysql_slave_send_query(MYSQL *mysql,
        const char *q, unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_slave_send_query");
    return false;
}

void MySQLOSPConnection::mysql_enable_rpl_parse(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
}

void MySQLOSPConnection::mysql_disable_rpl_parse(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
}

int MySQLOSPConnection::mysql_rpl_parse_enabled(MYSQL* mysql) {
    return notImplementedInt("TBD");
}

void MySQLOSPConnection::mysql_enable_reads_from_master(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
}

void MySQLOSPConnection::mysql_disable_reads_from_master(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
}

my_bool MySQLOSPConnection::mysql_reads_from_master_enabled(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
    return false;
}

enum mysql_rpl_type MySQLOSPConnection::mysql_rpl_query_type(const char* q,
        int len) {
    notImplementedPtr("MySQLOSPConnection::");
    return MYSQL_RPL_MASTER;
}

my_bool MySQLOSPConnection::mysql_rpl_probe(MYSQL* mysql) {
    notImplementedPtr("MySQLOSPConnection::");
    return false;
}

int MySQLOSPConnection::mysql_set_master(MYSQL* mysql, const char* host,
        unsigned int port, const char* user, const char* passwd) {
    return notImplementedInt("mysql_set_master");
}

int MySQLOSPConnection::mysql_add_slave(MYSQL* mysql, const char* host,
        unsigned int port, const char* user, const char* passwd) {
    return notImplementedInt("mysql_add_slave");
}

int MySQLOSPConnection::mysql_shutdown(MYSQL *mysql,
        enum mysql_enum_shutdown_level shutdown_level) {
    return notImplementedInt("mysql_shutdown");
}

int MySQLOSPConnection::mysql_dump_debug_info(MYSQL *mysql) {
    return notImplementedInt("mysql_dump_debug_info");
}

int MySQLOSPConnection::mysql_refresh(MYSQL *mysql, unsigned int refresh_options) {
    if (log.isTraceEnabled()) {
        log.trace("mysql_refresh() not implemented, but faking success");
    }
    return 0;
}

int MySQLOSPConnection::mysql_kill(MYSQL *mysql, unsigned long pid) {
    if (log.isTraceEnabled()) {
        log.trace("mysql_kill() not implemented, but faking success");
    }
    return 0;
}

int MySQLOSPConnection::mysql_set_server_option(MYSQL *mysql,
        enum enum_mysql_set_option option) {

    if (log.isTraceEnabled()) {
        log.trace("mysql_set_server_option() not implemented, but faking success");
    }

    return 0;
}

int MySQLOSPConnection::mysql_ping(MYSQL *mysql) {
    if (log.isTraceEnabled()) {
        log.trace("mysql_ping() not implemented, but faking success");
    }
    //TODO: run validation query such as "SELECT 1" to validate that the connection really is OK
    return 0;
}

const char * MySQLOSPConnection::mysql_stat(MYSQL *mysql) {
    notImplementedPtr("mysql_stat");
    return NULL;
}

const char * MySQLOSPConnection::mysql_get_server_info(MYSQL *mysql) {
    return mysql->server_version;
}

const char * MySQLOSPConnection::mysql_get_client_info(void) {
    notImplementedPtr("MySQLOSPConnection::mysql_get_client_info");
    return NULL;
}

unsigned long MySQLOSPConnection::mysql_get_client_version(void) {
    return notImplementedInt("MySQLOSPConnection::mysql_get_client_version");
}

const char * MySQLOSPConnection::mysql_get_host_info(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_get_host_info");
    return NULL;
}

unsigned long MySQLOSPConnection::mysql_get_server_version(MYSQL *mysql) {
    return notImplementedInt("MySQLOSPConnection::mysql_get_server_version");
}

unsigned int MySQLOSPConnection::mysql_get_proto_info(MYSQL *mysql) {
    return notImplementedInt("MySQLOSPConnection::mysql_get_proto_info");
}

MYSQL_RES * MySQLOSPConnection::mysql_list_dbs(MYSQL *mysql, const char *wild) {
    notImplementedPtr("MySQLOSPConnection::mysql_list_dbs");
    return NULL;
}

MYSQL_RES * MySQLOSPConnection::mysql_list_tables(MYSQL *mysql,
        const char *wild) {
    notImplementedPtr("MySQLOSPConnection::mysql_list_tables");
    return NULL;
}

MYSQL_RES * MySQLOSPConnection::mysql_list_processes(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_list_processes");
    return NULL;
}

int MySQLOSPConnection::mysql_options(MYSQL *mysql, enum mysql_option option,
        const char *arg) {
    log.warn("mysql_options() not implemented");
    return -1;
}

int MySQLOSPConnection::mysql_options(MYSQL *mysql, enum mysql_option option,
        const void *arg) {
    log.warn("mysql_options() not implemented");
    return -1;
}

MYSQL_ROW_OFFSET MySQLOSPConnection::mysql_row_seek(MYSQL_RES *res, MYSQL_ROW_OFFSET offset) {
    MYSQL_ROW_OFFSET val = res->data_cursor;
    res->current_row=0;
    res->data_cursor = offset;
    return val;
}

MYSQL_RES * MySQLOSPConnection::mysql_list_fields(MYSQL *mysql,
        const char *table, const char *wild) {
    notImplementedPtr("mysql_list_fields");       
    return NULL;
}

unsigned long MySQLOSPConnection::mysql_escape_string(char *to,
        const char *from, unsigned long from_length) {
    notImplementedPtr( "mysql_escape_string");
    return 0;
}

unsigned long MySQLOSPConnection::mysql_hex_string(char *to, const char *from,
        unsigned long from_length) {
    notImplementedPtr( "mysql_hex_string");
    return 0;
}

unsigned long MySQLOSPConnection::mysql_real_escape_string(MYSQL *mysql,
        char *to, const char *from, unsigned long length) {
    notImplementedPtr( "mysql_real_escape_string");
    return 0;
}

void MySQLOSPConnection::mysql_debug(const char *debug) {
    if (log.isDebugEnabled()) log.debug("MySQLOSPConnection::mysql_debug");
    // never called
}

void MySQLOSPConnection::myodbc_remove_escape(MYSQL *mysql, char *name) {
    if (log.isDebugEnabled()) log.debug("MySQLOSPConnection::myodbc_remove_escape");
}

unsigned int MySQLOSPConnection::mysql_thread_safe(void) {
    return notImplementedInt("MySQLOSPConnection::mysql_thread_safe");
}

my_bool MySQLOSPConnection::mysql_embedded(void) {
    return false;
}

MYSQL_MANAGER* MySQLOSPConnection::mysql_manager_init(MYSQL_MANAGER* con) {
    notImplementedPtr("MySQLOSPConnection::mysql_manager_init");
    return NULL;
}

MYSQL_MANAGER* MySQLOSPConnection::mysql_manager_connect(MYSQL_MANAGER* con,
        const char* host, const char* user, const char* passwd,
        unsigned int port) {
    notImplementedPtr("MySQLOSPConnection::mysql_manager_connect");
    return NULL;
}

void MySQLOSPConnection::mysql_manager_close(MYSQL_MANAGER* con) {
    notImplementedPtr("MySQLOSPConnection::mysql_manager_close");
}

int MySQLOSPConnection::mysql_manager_command(MYSQL_MANAGER* con,
        const char* cmd, int cmd_len) {
    notImplementedPtr("MySQLOSPConnection::mysql_manager_command");
    return -1;
}

int MySQLOSPConnection::mysql_manager_fetch_line(MYSQL_MANAGER* con,
        char* res_buf, int res_buf_size) {
    notImplementedPtr("MySQLOSPConnection::mysql_manager_fetch_line");
    return -1;
}

my_bool MySQLOSPConnection::mysql_read_query_result(MYSQL *mysql) {
    notImplementedPtr("MySQLOSPConnection::mysql_read_query_result");
    return false;
}

int MySQLOSPConnection::mysql_stmt_fetch(MYSQL_STMT *stmt) {
    return notImplementedInt("mysql_stmt_fetch");
}

int MySQLOSPConnection::mysql_stmt_fetch_column(MYSQL_STMT *stmt,
        MYSQL_BIND *bind_arg, unsigned int column, unsigned long offset) {
    return notImplementedInt("mysql_stmt_fetch_column");
}

int MySQLOSPConnection::mysql_stmt_store_result(MYSQL_STMT *stmt) {
    return notImplementedInt("mysql_stmt_store_result");
}

unsigned long MySQLOSPConnection::mysql_stmt_param_count(MYSQL_STMT * stmt) {
    return notImplementedInt("mysql_stmt_param_count");
}

my_bool MySQLOSPConnection::mysql_stmt_attr_set(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, const void *attr) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_attr_set");
    return false;
}

my_bool MySQLOSPConnection::mysql_stmt_attr_get(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, void *attr) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_attr_get");
    return false;
}

my_bool MySQLOSPConnection::mysql_stmt_bind_result(MYSQL_STMT * stmt,
        MYSQL_BIND * bnd) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_bind_result");
    return false;
}

my_bool MySQLOSPConnection::mysql_stmt_reset(MYSQL_STMT * stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_reset");
    return false;
}

my_bool MySQLOSPConnection::mysql_stmt_free_result(MYSQL_STMT *stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_free_result");
    return false;
}

my_bool MySQLOSPConnection::mysql_stmt_send_long_data(MYSQL_STMT *stmt,
        unsigned int param_number, const char *data, unsigned long length) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_send_long_data");
    return false;
}

MYSQL_RES * MySQLOSPConnection::mysql_stmt_result_metadata(MYSQL_STMT *stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_result_metadata");
    return NULL;
}

MYSQL_RES * MySQLOSPConnection::mysql_stmt_param_metadata(MYSQL_STMT *stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_param_metadata");
    return NULL;
}

unsigned int MySQLOSPConnection::mysql_stmt_errno(MYSQL_STMT * stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_errno");
    return -1;
}

const char * MySQLOSPConnection::mysql_stmt_error(MYSQL_STMT * stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_error");
    return NULL;
}

const char * MySQLOSPConnection::mysql_stmt_sqlstate(MYSQL_STMT * stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_sqlstate");
    return NULL;
}

MYSQL_ROW_OFFSET MySQLOSPConnection::mysql_stmt_row_seek(MYSQL_STMT *stmt,
        MYSQL_ROW_OFFSET offset) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_row_seek");
    return NULL;
}

MYSQL_ROW_OFFSET MySQLOSPConnection::mysql_stmt_row_tell(MYSQL_STMT *stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_row_tell");
    return NULL;
}

void MySQLOSPConnection::mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_data_seek");
}

my_ulonglong MySQLOSPConnection::mysql_stmt_num_rows(MYSQL_STMT *stmt) {
    return notImplementedInt("MySQLOSPConnection::mysql_stmt_num_rows");
}

my_ulonglong MySQLOSPConnection::mysql_stmt_affected_rows(MYSQL_STMT *stmt) {
    return notImplementedInt("MySQLOSPConnection::mysql_stmt_affected_rows");
}

my_ulonglong MySQLOSPConnection::mysql_stmt_insert_id(MYSQL_STMT *stmt) {
    return notImplementedInt("MySQLOSPConnection::mysql_stmt_insert_id");
}

unsigned int MySQLOSPConnection::mysql_stmt_field_count(MYSQL_STMT *stmt) {
    notImplementedPtr("MySQLOSPConnection::mysql_stmt_field_count");
    return -1;
}

int MySQLOSPConnection::mysql_next_result(MYSQL *mysql) {
    if (log.isDebugEnabled()) {
        notImplementedPtr("MySQLOSPConnection::mysql_next_result");
    }
    return -1;
}

/*
 MYSQL * MySQLOSPConnection::mysql_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd){
 return notImplementedPtr("mysql_connect");
 }
 */
int MySQLOSPConnection::mysql_create_db(MYSQL *mysql, const char *DB) {
    notImplementedPtr( "mysql_create_db");
    return -1;
}

int MySQLOSPConnection::mysql_drop_db(MYSQL *mysql, const char *DB) {
    notImplementedPtr( "mysql_drop_db");
    return -1;
}