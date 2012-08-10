/* Copyright (C) 2010-2011 CodeFutures Corporation

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

#include "MySQLOSPResultSet.h"

#include <mysql.h>

#include <mysql/MySQLAbstractResultSet.h>

#include <util/Util.h>

using namespace util;
using namespace logger;

Logger &MySQLOSPResultSet::log = Logger::getLogger("MySQLOSPResultSet");

MySQLOSPResultSet::MySQLOSPResultSet(MySQLOSPConnection *conn) {
    this->conn = conn;
}

MySQLOSPResultSet::~MySQLOSPResultSet() {
}

void MySQLOSPResultSet::mysql_free_result(MYSQL_RES *res) {

    if (log.isTraceEnabled()) {
        log.trace(string("mysql_free_result(") + Util::toString((void*)res) + string(")"));
    }

    if (res) {

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
            if (res->fields[i].table) {
                delete [] res->fields[i].table;
            }
            if (res->fields[i].name) {
                delete [] res->fields[i].name;
            }
        }
        if (res->fields) delete [] res->fields;
        if (res->lengths) delete [] res->lengths;
	if (res->data) delete res->data;
        delete res;
    }
}

MYSQL_ROW MySQLOSPResultSet::mysql_fetch_row(MYSQL_RES *result) {
    //TODO: implement
    return NULL;
}

unsigned long * MySQLOSPResultSet::mysql_fetch_lengths(MYSQL_RES *result) {
    //TODO: implement
    return NULL;
}
 
MYSQL_FIELD_OFFSET MySQLOSPResultSet::mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset) {
    //TODO: implement
    return 0;
}

MYSQL_FIELD * MySQLOSPResultSet::mysql_fetch_field(MYSQL_RES *result) {
    //TODO: implement
    return NULL;
}

void MySQLOSPResultSet::mysql_data_seek(MYSQL_RES *result, my_ulonglong offset) {
    //TODO: implement
}

MYSQL_ROW_OFFSET MySQLOSPResultSet::mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset) {
    //TODO: implement
    return NULL;
}

my_ulonglong MySQLOSPResultSet::mysql_num_rows(MYSQL_RES *res) {
    //TODO: implement
    return 0;
}

unsigned int MySQLOSPResultSet::mysql_num_fields(MYSQL_RES *res) {
    //TODO: implement
    return 0;
}

my_bool MySQLOSPResultSet::mysql_eof(MYSQL_RES *res) {
    //TODO: implement
    return false;
}

MYSQL_FIELD * MySQLOSPResultSet::mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr) {
    //TODO: implement
    return NULL;
}

MYSQL_FIELD * MySQLOSPResultSet::mysql_fetch_fields(MYSQL_RES *res) {
    //TODO: implement
    return NULL;
}

MYSQL_ROW_OFFSET MySQLOSPResultSet::mysql_row_tell(MYSQL_RES *res) {
    //TODO: implement
    return NULL;
}

MYSQL_FIELD_OFFSET MySQLOSPResultSet::mysql_field_tell(MYSQL_RES *res) {
    //TODO: implement
    return 0;
}




