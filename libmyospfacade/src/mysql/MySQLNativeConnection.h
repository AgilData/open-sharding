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


#ifndef DBSMYSQLNATIVECONNECTION_H_
#define DBSMYSQLNATIVECONNECTION_H_

#include <list>
#include <map>
#include <string>
#include <mysql.h>

//#include <boost/scoped_ptr.hpp>

#include <mysql/MySQLClient.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLConnMap.h>

#include <logger/Logger.h>
#include <util/Util.h>

using namespace std;
using namespace logger;
using namespace mysql;
using namespace util;

/**
 * Wrapper around a MySQL connection. No replication or sharding logic.
 */
class MySQLNativeConnection : public MySQLAbstractConnection {
private:

    // logger
    static Logger log;
    static Logger alog; // used by LOG_ANALYSER

    // an actual MySQL connection handle
    MYSQL *mysql;
    
    // MySQL shared library
    MySQLClient *mysqlClient;

    MySQLConnMap *mysqlResourceMap;

    // cache function pointers
    mysql_num_rowsFnType* mysql_num_rowsFn;
    mysql_num_fieldsFnType* mysql_num_fieldsFn;
    mysql_row_seekFnType* mysql_row_seekFn;
    mysql_data_seekFnType* mysql_data_seekFn;
    mysql_field_seekFnType* mysql_field_seekFn;
    mysql_row_tellFnType* mysql_row_tellFn;
    mysql_fetch_fieldFnType* mysql_fetch_fieldFn;
    mysql_fetch_rowFnType* mysql_fetch_rowFn;
    mysql_fetch_lengthsFnType* mysql_fetch_lengthsFn;
    mysql_free_resultFnType* mysql_free_resultFn;
    mysql_more_resultsFnType* mysql_more_resultsFn;
    mysql_real_queryFnType* mysql_real_queryFn;
    mysql_send_queryFnType* mysql_send_queryFn;
    mysql_store_resultFnType* mysql_store_resultFn;
    mysql_rollbackFnType* mysql_rollbackFn;
    mysql_commitFnType* mysql_commitFn;
    mysql_insert_idFnType* mysql_insert_idFn;

    //TODO: cache ALL function pointers!


private:

    void *get_mysql_function(const char *name);
    
public:
    // internal methods
    MySQLNativeConnection(MYSQL *, MySQLClient *, MySQLConnMap *connMap);
    virtual ~MySQLNativeConnection();
    void trace(const char *name, MYSQL *mysql);
    bool connect(const char *, const char *, const char *, const char *, unsigned int port, const char *unix_socket, unsigned long clientflag) ;

    // MySQL API methods
    my_bool mysql_autocommit(MYSQL * _mysql, my_bool auto_mode);
    MYSQL_STMT * mysql_stmt_init(MYSQL *_mysql);
    int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length);
    int mysql_stmt_execute(MYSQL_STMT *stmt);
    int mysql_query(MYSQL *_mysql, const char *sql);
    MYSQL_RES * mysql_store_result(MYSQL *_mysql);
    my_bool mysql_commit(MYSQL * _mysql);
    void mysql_close(MYSQL *_mysql);
    int mysql_server_init(int argc, char **argv, char **groups);
    void mysql_server_end(void);
    MYSQL_PARAMETERS * mysql_get_parameters(void);
    my_bool mysql_thread_init(void);
    void mysql_thread_end(void);
    my_ulonglong mysql_num_rows(MYSQL_RES *res);
    unsigned int mysql_num_fields(MYSQL_RES *res);
    my_bool mysql_eof(MYSQL_RES *res);
    MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr);
    MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res);
    MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res);
    MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res);
    unsigned int mysql_field_count(MYSQL *mysql);
    my_ulonglong mysql_affected_rows(MYSQL *mysql);
    my_ulonglong mysql_insert_id(MYSQL *mysql);
    unsigned int mysql_errno(MYSQL *mysql);
    const char * mysql_error(MYSQL *mysql);
    const char * mysql_sqlstate(MYSQL *mysql);
    unsigned int mysql_warning_count(MYSQL *mysql);
    const char * mysql_info(MYSQL *mysql);
    unsigned long mysql_thread_id(MYSQL *mysql);
    const char * mysql_character_set_name(MYSQL *mysql);
    int mysql_set_character_set(MYSQL *mysql, const char *csname);
    my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher);
    const char * mysql_get_ssl_cipher(MYSQL *mysql);
    my_bool mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db);
    int mysql_select_db(MYSQL *mysql, const char *db);
    int mysql_send_query(MYSQL *mysql, const char *q, unsigned long length);
    int mysql_real_query(MYSQL *mysql, const char *q, unsigned long length);
    MYSQL_RES * mysql_use_result(MYSQL *mysql);
    my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length);
    void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset);
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

    // pre 5.1.18
    int mysql_options(MYSQL *mysql,enum mysql_option option, const char *arg);
    // 5.1.18 and later
    int mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg);

    void mysql_free_result(MYSQL_RES *result);
    void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
    MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
    MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
    unsigned long * mysql_fetch_lengths(MYSQL_RES *result);
    MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result);
    MYSQL_RES * mysql_list_fields(MYSQL *mysql, const char *table, const char *wild);
    unsigned long mysql_escape_string(char *to,const char *from, unsigned long from_length);
    unsigned long mysql_hex_string(char *to,const char *from, unsigned long from_length);
    unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length);
    void mysql_debug(const char *debug);
    void myodbc_remove_escape(MYSQL *mysql,char *name);
    unsigned int mysql_thread_safe(void);
    my_bool mysql_embedded(void);
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

#endif /* DBSMYSQLREPLICATECONNECTION_H_ */
