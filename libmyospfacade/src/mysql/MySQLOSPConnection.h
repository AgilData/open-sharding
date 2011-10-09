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


#ifndef __MySQLOSPConnection_h_
#define __MySQLOSPConnection_h_

#include <unistd.h>
#include <vector>

#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLConnMap.h>
#include <mysql/MySQLOSPConnection.h>

#include <opensharding/OSPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPConnectRequest.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPCreateStatementRequest.h>
#include <opensharding/OSPCreateStatementResponse.h>
#include <opensharding/OSPExecuteRequest.h>
#include <opensharding/OSPExecuteResponse.h>
#include <opensharding/OSPResultSetRequest.h>
#include <opensharding/OSPResultSetResponse.h>
#include <opensharding/OSPDisconnectRequest.h>
#include <opensharding/OSPDisconnectResponse.h>

#include <logger/Logger.h>

#include <mysql.h>

#define JDBC_BIT -7
#define JDBC_TINYINT -6
#define JDBC_SMALLINT 5
#define JDBC_INTEGER 4
#define JDBC_BIGINT -5
#define JDBC_FLOAT 6
#define JDBC_REAL 7
#define JDBC_DOUBLE 8
#define JDBC_NUMERIC 2
#define JDBC_DECIMAL 3
#define JDBC_CHAR 1
#define JDBC_VARCHAR 12
#define JDBC_LONGVARCHAR -1
#define JDBC_DATE 91
#define JDBC_TIME 92
#define JDBC_TIMESTAMP 93
#define JDBC_BINARY -2
#define JDBC_VARBINARY -3
#define JDBC_LONGVARBINARY -4
#define JDBC_NULL 0
#define JDBC_OTHER 1111
#define JDBC_JAVA_OBJECT 2000
#define JDBC_DISTINCT 2001
#define JDBC_STRUCT 2002
#define JDBC_ARRAY 2003
#define JDBC_BLOB 2004
#define JDBC_CLOB 2005
#define JDBC_REF 2006
#define JDBC_DATALINK 70
#define JDBC_BOOLEAN 16
#define JDBC_ROWID -8
#define JDBC_NCHAR -15
#define JDBC_NVARCHAR -9
#define JDBC_LONGNVARCHAR -16
#define JDBC_NCLOB 2011
#define JDBC_SQLXML 2009


using namespace logger;
using namespace opensharding;

class MySQLOSPConnection : public MySQLAbstractConnection, public OSPMessageConsumer {
private:

    // logger
    static Logger &log;

    pid_t pid;

    OSPConnection *ospConn;

    /* OSP connection ID */
    string connID;

    /* OSP statement ID */
    int stmtID;

    /* Last result set response */
    int affectedRows;
    int insertID;
    int fieldCount;

    MySQLConnMap *mysqlResourceMap;

    /* Current and next result set ID */
    int resultSetID;
    int nextResultSetID;
    
    /* MySQL error flags */
    const char *my_sqlstate;
    int my_errno;
    char *my_error;

    /* Mutex for thread-safe access to resultSets */
    //boost::mutex resultSetMutex;

    // temporary variable used to populate
    MYSQL_RES  *currentRes;
    MYSQL_ROWS *currentRow;
    MYSQL_ROWS *prevRow;


public:
    MySQLOSPConnection(string host, int port, string database, string user, string password, MySQLConnMap *mysqlResourceMap);
    virtual ~MySQLOSPConnection();
    bool connect(const char *, const char *, const char *, const char *, unsigned int port, const char *unix_socket, unsigned long clientflag) ;
    void setError(const char *sqlstate, int x_errno, const char *x_error);
    string getLogPrefix(MYSQL *mysql);
    char *ensureCapacity(char *buffer, unsigned int *length, unsigned int capacity);
    void processMessage(OSPMessage*);

    // highest priority methods to add support for
    int mysql_server_init(int argc, char **argv, char **groups);
    void mysql_server_end(void);
    int mysql_select_db(MYSQL *mysql, const char *db);
    my_bool mysql_autocommit(MYSQL * _mysql, my_bool auto_mode);
    int mysql_query(MYSQL *_mysql, const char *sql);
    int mysql_real_query(MYSQL *mysql, const char *q, unsigned long length);
    int mysql_send_query(MYSQL *mysql, const char *q, unsigned long length);
    MYSQL_RES * mysql_store_result(MYSQL *_mysql);
    MYSQL_RES * mysql_use_result(MYSQL *mysql);
    my_bool mysql_commit(MYSQL * _mysql);
    void mysql_close(MYSQL *_mysql);
    my_ulonglong mysql_num_rows(MYSQL_RES *res);
    unsigned int mysql_num_fields(MYSQL_RES *res);
    MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res);
    MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res);
    MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res);
    unsigned int mysql_field_count(MYSQL *mysql);
    void mysql_free_result(MYSQL_RES *result);
    void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
    MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
    MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
    unsigned long * mysql_fetch_lengths(MYSQL_RES *result);
    MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result);
    my_ulonglong mysql_affected_rows(MYSQL *mysql);
    my_ulonglong mysql_insert_id(MYSQL *mysql);
    unsigned int mysql_errno(MYSQL *mysql);
    const char * mysql_error(MYSQL *mysql);
    const char * mysql_sqlstate(MYSQL *mysql);
    unsigned int mysql_warning_count(MYSQL *mysql);
    const char * mysql_info(MYSQL *mysql);
    const char * mysql_character_set_name(MYSQL *mysql);
    int mysql_set_character_set(MYSQL *mysql, const char *csname);

    // medium priority
    unsigned long mysql_escape_string(char *to,const char *from, unsigned long from_length);
    unsigned long mysql_hex_string(char *to,const char *from, unsigned long from_length);
    unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length);
    my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher);
    const char * mysql_get_ssl_cipher(MYSQL *mysql);
    my_bool mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db);
    unsigned long mysql_thread_id(MYSQL *mysql);
    my_bool mysql_eof(MYSQL_RES *res);
    MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr);
    my_bool mysql_thread_init(void);
    void mysql_thread_end(void);
    MYSQL_STMT * mysql_stmt_init(MYSQL *_mysql);
    MYSQL_PARAMETERS * mysql_get_parameters(void);
    void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset);
    int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option);
    int mysql_ping(MYSQL *mysql);
    const char * mysql_stat(MYSQL *mysql);
    const char * mysql_get_server_info(MYSQL *mysql);
    const char * mysql_get_client_info(void);
    unsigned long mysql_get_client_version(void);
    const char * mysql_get_host_info(MYSQL *mysql);
    unsigned long mysql_get_server_version(MYSQL *mysql);
    unsigned int mysql_get_proto_info(MYSQL *mysql);
    MYSQL_RES * mysql_list_dbs(MYSQL *mysql,const char *wild);
    MYSQL_RES * mysql_list_tables(MYSQL *mysql,const char *wild);
    MYSQL_RES * mysql_list_processes(MYSQL *mysql);

    int mysql_options(MYSQL *mysql,enum mysql_option option, const char *arg);
    int mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg);

    MYSQL_RES * mysql_list_fields(MYSQL *mysql, const char *table, const char *wild);
    void mysql_debug(const char *debug);
    void myodbc_remove_escape(MYSQL *mysql,char *name);
    unsigned int mysql_thread_safe(void);
    my_bool mysql_embedded(void);

    // low priority
    int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length);
    int mysql_stmt_execute(MYSQL_STMT *stmt);
    my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length);
    void mysql_enable_rpl_parse(MYSQL* mysql);
    void mysql_disable_rpl_parse(MYSQL* mysql);
    int mysql_rpl_parse_enabled(MYSQL* mysql);
    void mysql_enable_reads_from_master(MYSQL* mysql);
    void mysql_disable_reads_from_master(MYSQL* mysql);
    my_bool mysql_reads_from_master_enabled(MYSQL* mysql);
    enum mysql_rpl_type mysql_rpl_query_type(const char* q, int len);
    my_bool mysql_rpl_probe(MYSQL* mysql);
    int mysql_set_master(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
    int mysql_add_slave(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
    int mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level);
    int mysql_dump_debug_info(MYSQL *mysql);
    int mysql_refresh(MYSQL *mysql, unsigned int refresh_options);
    int mysql_kill(MYSQL *mysql,unsigned long pid);
    MYSQL_MANAGER* mysql_manager_init(MYSQL_MANAGER* con);
    MYSQL_MANAGER* mysql_manager_connect(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port);
    void mysql_manager_close(MYSQL_MANAGER* con);
    int mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len);
    int mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf, int res_buf_size);
    my_bool mysql_read_query_result(MYSQL *mysql);
    int mysql_stmt_fetch(MYSQL_STMT *stmt);
    int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset);
    int mysql_stmt_store_result(MYSQL_STMT *stmt);
    unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt);
    my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr);
    my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr);
    my_bool mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
    my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
    my_bool mysql_stmt_close(MYSQL_STMT * stmt);
    my_bool mysql_stmt_reset(MYSQL_STMT * stmt);
    my_bool mysql_stmt_free_result(MYSQL_STMT *stmt);
    my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length);
    MYSQL_RES * mysql_stmt_result_metadata(MYSQL_STMT *stmt);
    MYSQL_RES * mysql_stmt_param_metadata(MYSQL_STMT *stmt);
    unsigned int mysql_stmt_errno(MYSQL_STMT * stmt);
    const char * mysql_stmt_error(MYSQL_STMT * stmt);
    const char * mysql_stmt_sqlstate(MYSQL_STMT * stmt);
    MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset);
    MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt);
    void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset);
    my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt);
    my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt);
    my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt);
    unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt);
    my_bool mysql_rollback(MYSQL * mysql);
    my_bool mysql_more_results(MYSQL *mysql);
    int mysql_next_result(MYSQL *mysql);
    int mysql_create_db(MYSQL *mysql, const char *DB);
    int mysql_drop_db(MYSQL *mysql, const char *DB);

};

#endif /* __MySQLOSPConnection_h_ */
