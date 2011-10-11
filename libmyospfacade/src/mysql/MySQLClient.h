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


#ifndef __MySQLClient_h__
#define __MySQLClient_h__

#include <list>
#include <map>
#include <string>
#include <mysql.h>

#include <boost/thread/mutex.hpp>

#include <util/DynLib.h>
#include <logger/Logger.h>

using namespace std;
using namespace logger;
using namespace util;

// #ifdef for the real mysql library that this wrapper will load.
// NOTE: we *always* link to the thread-safe version (with _r suffix)

//TODO: need to decide if we want to use _real_ or not .. seems safer to use this even if it is a symlink

#ifdef MYSQL_5_0
//#define MYSQL_LIBRARY "libmysqlclient_r.so.15.0.0"
#define MYSQL_LIBRARY "libmysqlclient_real_r.so.15.0.0"
#endif

#ifdef MYSQL_5_1
//#define MYSQL_LIBRARY "libmysqlclient_r.so.16.0.0"
#define MYSQL_LIBRARY "libmysqlclient_real_r.so.16.0.0"
#endif

#ifdef MYSQL_5_5
//#define MYSQL_LIBRARY "libmysqlclient_r.so.18.0.0"
#define MYSQL_LIBRARY "libmysqlclient_real_r.so.18.0.0"
#endif

// TYPEDEFs for all MySQL functions that we can dynamically invoke in libmysqlclient

//int STDCALL mysql_library_init(int argc, char **argv, char **groups);
//typedef int mysql_library_initFnType(int argc, char **argv, char **groups);
//void STDCALL mysql_library_end(void);
//typedef void mysql_library_endFnType(void);
//int STDCALL mysql_server_init(int argc, char **argv, char **groups);
typedef int mysql_server_initFnType(int argc, char **argv, char **groups);
//void STDCALL mysql_server_end(void);
typedef void mysql_server_endFnType(void);
//MYSQL_PARAMETERS *STDCALL mysql_get_parameters(void);
typedef MYSQL_PARAMETERS * mysql_get_parametersFnType(void);
//my_bool STDCALL mysql_thread_init(void);
typedef my_bool mysql_thread_initFnType(void);
//void STDCALL mysql_thread_end(void);
typedef void mysql_thread_endFnType(void);
//my_ulonglong STDCALL mysql_num_rows(MYSQL_RES *res);
typedef my_ulonglong mysql_num_rowsFnType(MYSQL_RES *res);
//unsigned int STDCALL mysql_num_fields(MYSQL_RES *res);
typedef unsigned int mysql_num_fieldsFnType(MYSQL_RES *res);
//my_bool STDCALL mysql_eof(MYSQL_RES *res);
typedef my_bool mysql_eofFnType(MYSQL_RES *res);
//MYSQL_FIELD *STDCALL mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr);
typedef MYSQL_FIELD * mysql_fetch_field_directFnType(MYSQL_RES *res, unsigned int fieldnr);
//MYSQL_FIELD * STDCALL mysql_fetch_fields(MYSQL_RES *res);
typedef MYSQL_FIELD * mysql_fetch_fieldsFnType(MYSQL_RES *res);
//MYSQL_ROW_OFFSET STDCALL mysql_row_tell(MYSQL_RES *res);
typedef MYSQL_ROW_OFFSET mysql_row_tellFnType(MYSQL_RES *res);
//MYSQL_FIELD_OFFSET STDCALL mysql_field_tell(MYSQL_RES *res);
typedef MYSQL_FIELD_OFFSET mysql_field_tellFnType(MYSQL_RES *res);
//unsigned int STDCALL mysql_field_count(MYSQL *mysql);
typedef unsigned int mysql_field_countFnType(MYSQL *mysql);
//my_ulonglong STDCALL mysql_affected_rows(MYSQL *mysql);
typedef my_ulonglong mysql_affected_rowsFnType(MYSQL *mysql);
//my_ulonglong STDCALL mysql_insert_id(MYSQL *mysql);
typedef my_ulonglong mysql_insert_idFnType(MYSQL *mysql);
//unsigned int STDCALL mysql_errno(MYSQL *mysql);
typedef unsigned int mysql_errnoFnType(MYSQL *mysql);
//const char * STDCALL mysql_error(MYSQL *mysql);
typedef const char * mysql_errorFnType(MYSQL *mysql);
//const char *STDCALL mysql_sqlstate(MYSQL *mysql);
typedef const char * mysql_sqlstateFnType(MYSQL *mysql);
//unsigned int STDCALL mysql_warning_count(MYSQL *mysql);
typedef unsigned int mysql_warning_countFnType(MYSQL *mysql);
//const char * STDCALL mysql_info(MYSQL *mysql);
typedef const char * mysql_infoFnType(MYSQL *mysql);
//unsigned long STDCALL mysql_thread_id(MYSQL *mysql);
typedef unsigned long mysql_thread_idFnType(MYSQL *mysql);
//const char * STDCALL mysql_character_set_name(MYSQL *mysql);
typedef const char * mysql_character_set_nameFnType(MYSQL *mysql);
//int          STDCALL mysql_set_character_set(MYSQL *mysql, const char *csname);
typedef int mysql_set_character_setFnType(MYSQL *mysql, const char *csname);
//MYSQL *        STDCALL mysql_init(MYSQL *mysql);
typedef MYSQL * mysql_initFnType(MYSQL *mysql);
//my_bool        STDCALL mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher);
typedef my_bool mysql_ssl_setFnType(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher);
//const char *    STDCALL mysql_get_ssl_cipher(MYSQL *mysql);
typedef const char * mysql_get_ssl_cipherFnType(MYSQL *mysql);
//my_bool        STDCALL mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db);
typedef my_bool mysql_change_userFnType(MYSQL *mysql, const char *user,  const char *passwd, const char *db);
//MYSQL *        STDCALL mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag);
typedef MYSQL * mysql_real_connectFnType(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag);
//int        STDCALL mysql_select_db(MYSQL *mysql, const char *db);
typedef int mysql_select_dbFnType(MYSQL *mysql, const char *db);
//int        STDCALL mysql_query(MYSQL *mysql, const char *q);
typedef int mysql_queryFnType(MYSQL *mysql, const char *q);
//int        STDCALL mysql_send_query(MYSQL *mysql, const char *q, unsigned long length);
typedef int mysql_send_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//int        STDCALL mysql_real_query(MYSQL *mysql, const char *q, unsigned long length);
typedef int mysql_real_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//MYSQL_RES *     STDCALL mysql_store_result(MYSQL *mysql);
typedef MYSQL_RES * mysql_store_resultFnType(MYSQL *mysql);
//MYSQL_RES *     STDCALL mysql_use_result(MYSQL *mysql);
typedef MYSQL_RES * mysql_use_resultFnType(MYSQL *mysql);
//my_bool        STDCALL mysql_master_query(MYSQL *mysql, const char *q, unsigned long length);
typedef my_bool mysql_master_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//my_bool        STDCALL mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length);
typedef my_bool mysql_master_send_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//my_bool        STDCALL mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length);
typedef my_bool mysql_slave_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//my_bool        STDCALL mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length);
typedef my_bool mysql_slave_send_queryFnType(MYSQL *mysql, const char *q, unsigned long length);
//void        STDCALL mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset);
typedef void mysql_get_character_set_infoFnType(MYSQL *mysql, MY_CHARSET_INFO *charset);
//void            STDCALL mysql_enable_rpl_parse(MYSQL* mysql);
typedef void mysql_enable_rpl_parseFnType(MYSQL* mysql);
//void            STDCALL mysql_disable_rpl_parse(MYSQL* mysql);
typedef void mysql_disable_rpl_parseFnType(MYSQL* mysql);
//int             STDCALL mysql_rpl_parse_enabled(MYSQL* mysql);
typedef int mysql_rpl_parse_enabledFnType(MYSQL* mysql);
//void            STDCALL mysql_enable_reads_from_master(MYSQL* mysql);
typedef void mysql_enable_reads_from_masterFnType(MYSQL* mysql);
//void            STDCALL mysql_disable_reads_from_master(MYSQL* mysql);
typedef void mysql_disable_reads_from_masterFnType(MYSQL* mysql);
//my_bool        STDCALL mysql_reads_from_master_enabled(MYSQL* mysql);
typedef my_bool mysql_reads_from_master_enabledFnType(MYSQL* mysql);

#ifndef MYSQL_5_5
//enum mysql_rpl_type     STDCALL mysql_rpl_query_type(const char* q, int len);
typedef enum mysql_rpl_type mysql_rpl_query_typeFnType(const char* q, int len);
#endif

//my_bool        STDCALL mysql_rpl_probe(MYSQL* mysql);
typedef my_bool mysql_rpl_probeFnType(MYSQL* mysql);
//int             STDCALL mysql_set_master(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
typedef int mysql_set_masterFnType(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
//int             STDCALL mysql_add_slave(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
typedef int mysql_add_slaveFnType(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd);
//int        STDCALL mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level);
typedef int mysql_shutdownFnType(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level);
//int        STDCALL mysql_dump_debug_info(MYSQL *mysql);
typedef int mysql_dump_debug_infoFnType(MYSQL *mysql);
//int        STDCALL mysql_refresh(MYSQL *mysql, unsigned int refresh_options);
typedef int mysql_refreshFnType(MYSQL *mysql, unsigned int refresh_options);
//int        STDCALL mysql_kill(MYSQL *mysql,unsigned long pid);
typedef int mysql_killFnType(MYSQL *mysql,unsigned long pid);
//int        STDCALL mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option);
typedef int mysql_set_server_optionFnType(MYSQL *mysql, enum enum_mysql_set_option option);
//int        STDCALL mysql_ping(MYSQL *mysql);
typedef int mysql_pingFnType(MYSQL *mysql);
//const char *    STDCALL mysql_stat(MYSQL *mysql);
typedef const char * mysql_statFnType(MYSQL *mysql);
//const char *    STDCALL mysql_get_server_info(MYSQL *mysql);
typedef const char * mysql_get_server_infoFnType(MYSQL *mysql);
//const char *    STDCALL mysql_get_client_info(void);
typedef const char * mysql_get_client_infoFnType(void);
//unsigned long    STDCALL mysql_get_client_version(void);
typedef unsigned long mysql_get_client_versionFnType(void);
//const char *    STDCALL mysql_get_host_info(MYSQL *mysql);
typedef const char * mysql_get_host_infoFnType(MYSQL *mysql);
//unsigned long    STDCALL mysql_get_server_version(MYSQL *mysql);
typedef unsigned long mysql_get_server_versionFnType(MYSQL *mysql);
//unsigned int    STDCALL mysql_get_proto_info(MYSQL *mysql);
typedef unsigned int mysql_get_proto_infoFnType(MYSQL *mysql);
//MYSQL_RES *    STDCALL mysql_list_dbs(MYSQL *mysql,const char *wild);
typedef MYSQL_RES * mysql_list_dbsFnType(MYSQL *mysql,const char *wild);
//MYSQL_RES *    STDCALL mysql_list_tables(MYSQL *mysql,const char *wild);
typedef MYSQL_RES * mysql_list_tablesFnType(MYSQL *mysql,const char *wild);
//MYSQL_RES *    STDCALL mysql_list_processes(MYSQL *mysql);
typedef MYSQL_RES * mysql_list_processesFnType(MYSQL *mysql);
//int        STDCALL mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg);
typedef int mysql_optionsFnType(MYSQL *mysql,enum mysql_option option, const void *arg);
//void        STDCALL mysql_free_result(MYSQL_RES *result);
typedef void mysql_free_resultFnType(MYSQL_RES *result);
//void        STDCALL mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
typedef void mysql_data_seekFnType(MYSQL_RES *result, my_ulonglong offset);
//MYSQL_ROW_OFFSET STDCALL mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
typedef MYSQL_ROW_OFFSET mysql_row_seekFnType(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
//MYSQL_FIELD_OFFSET STDCALL mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
typedef MYSQL_FIELD_OFFSET mysql_field_seekFnType(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
//MYSQL_ROW    STDCALL mysql_fetch_row(MYSQL_RES *result);
typedef MYSQL_ROW mysql_fetch_rowFnType(MYSQL_RES *result);
//unsigned long * STDCALL mysql_fetch_lengths(MYSQL_RES *result);
typedef unsigned long * mysql_fetch_lengthsFnType(MYSQL_RES *result);
//MYSQL_FIELD *    STDCALL mysql_fetch_field(MYSQL_RES *result);
typedef MYSQL_FIELD * mysql_fetch_fieldFnType(MYSQL_RES *result);
//MYSQL_RES *     STDCALL mysql_list_fields(MYSQL *mysql, const char *table, const char *wild);
typedef MYSQL_RES * mysql_list_fieldsFnType(MYSQL *mysql, const char *table, const char *wild);
//unsigned long    STDCALL mysql_escape_string(char *to,const char *from, unsigned long from_length);
typedef unsigned long mysql_escape_stringFnType(char *to,const char *from, unsigned long from_length);
//unsigned long    STDCALL mysql_hex_string(char *to,const char *from, unsigned long from_length);
typedef unsigned long mysql_hex_stringFnType(char *to,const char *from, unsigned long from_length);
//unsigned long STDCALL mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length);
typedef unsigned long mysql_real_escape_stringFnType(MYSQL *mysql, char *to,const char *from, unsigned long length);
//void        STDCALL mysql_debug(const char *debug);
typedef void mysql_debugFnType(const char *debug);
//void         STDCALL myodbc_remove_escape(MYSQL *mysql,char *name);
typedef void myodbc_remove_escapeFnType(MYSQL *mysql,char *name);
//unsigned int    STDCALL mysql_thread_safe(void);
typedef unsigned int mysql_thread_safeFnType(void);
//my_bool        STDCALL mysql_embedded(void);
typedef my_bool mysql_embeddedFnType(void);
//MYSQL_MANAGER*  STDCALL mysql_manager_init(MYSQL_MANAGER* con);
typedef MYSQL_MANAGER* mysql_manager_initFnType(MYSQL_MANAGER* con);
//MYSQL_MANAGER*  STDCALL mysql_manager_connect(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port);
typedef MYSQL_MANAGER* mysql_manager_connectFnType(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port);
//void            STDCALL mysql_manager_close(MYSQL_MANAGER* con);
typedef void mysql_manager_closeFnType(MYSQL_MANAGER* con);
//int             STDCALL mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len);
typedef int mysql_manager_commandFnType(MYSQL_MANAGER* con, const char* cmd, int cmd_len);
//int             STDCALL mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf, int res_buf_size);
typedef int mysql_manager_fetch_lineFnType(MYSQL_MANAGER* con, char* res_buf, int res_buf_size);
//my_bool         STDCALL mysql_read_query_result(MYSQL *mysql);
typedef my_bool mysql_read_query_resultFnType(MYSQL *mysql);
//MYSQL_STMT * STDCALL mysql_stmt_init(MYSQL *mysql);
typedef MYSQL_STMT * mysql_stmt_initFnType(MYSQL *mysql);
//int STDCALL mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length);
typedef int mysql_stmt_prepareFnType(MYSQL_STMT *stmt, const char *query, unsigned long length);
//int STDCALL mysql_stmt_execute(MYSQL_STMT *stmt);
typedef int mysql_stmt_executeFnType(MYSQL_STMT *stmt);
//int STDCALL mysql_stmt_fetch(MYSQL_STMT *stmt);
typedef int mysql_stmt_fetchFnType(MYSQL_STMT *stmt);
//int STDCALL mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset);
typedef int mysql_stmt_fetch_columnFnType(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset);
//int STDCALL mysql_stmt_store_result(MYSQL_STMT *stmt);
typedef int mysql_stmt_store_resultFnType(MYSQL_STMT *stmt);
//unsigned long STDCALL mysql_stmt_param_count(MYSQL_STMT * stmt);
typedef unsigned long mysql_stmt_param_countFnType(MYSQL_STMT * stmt);
//my_bool STDCALL mysql_stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr);
typedef my_bool mysql_stmt_attr_setFnType(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr);
//my_bool STDCALL mysql_stmt_attr_get(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr);
typedef my_bool mysql_stmt_attr_getFnType(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr);
//my_bool STDCALL mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
typedef my_bool mysql_stmt_bind_paramFnType(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
//my_bool STDCALL mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
typedef my_bool mysql_stmt_bind_resultFnType(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
//my_bool STDCALL mysql_stmt_close(MYSQL_STMT * stmt);
typedef my_bool mysql_stmt_closeFnType(MYSQL_STMT * stmt);
//my_bool STDCALL mysql_stmt_reset(MYSQL_STMT * stmt);
typedef my_bool mysql_stmt_resetFnType(MYSQL_STMT * stmt);
//my_bool STDCALL mysql_stmt_free_result(MYSQL_STMT *stmt);
typedef my_bool mysql_stmt_free_resultFnType(MYSQL_STMT *stmt);
//my_bool STDCALL mysql_stmt_send_long_data(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length);
typedef my_bool mysql_stmt_send_long_dataFnType(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length);
//MYSQL_RES *STDCALL mysql_stmt_result_metadata(MYSQL_STMT *stmt);
typedef MYSQL_RES * mysql_stmt_result_metadataFnType(MYSQL_STMT *stmt);
//MYSQL_RES *STDCALL mysql_stmt_param_metadata(MYSQL_STMT *stmt);
typedef MYSQL_RES * mysql_stmt_param_metadataFnType(MYSQL_STMT *stmt);
//unsigned int STDCALL mysql_stmt_errno(MYSQL_STMT * stmt);
typedef unsigned int mysql_stmt_errnoFnType(MYSQL_STMT * stmt);
//const char *STDCALL mysql_stmt_error(MYSQL_STMT * stmt);
typedef const char * mysql_stmt_errorFnType(MYSQL_STMT * stmt);
//const char *STDCALL mysql_stmt_sqlstate(MYSQL_STMT * stmt);
typedef const char * mysql_stmt_sqlstateFnType(MYSQL_STMT * stmt);
//MYSQL_ROW_OFFSET STDCALL mysql_stmt_row_seek(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset);
typedef MYSQL_ROW_OFFSET mysql_stmt_row_seekFnType(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset);
//MYSQL_ROW_OFFSET STDCALL mysql_stmt_row_tell(MYSQL_STMT *stmt);
typedef MYSQL_ROW_OFFSET mysql_stmt_row_tellFnType(MYSQL_STMT *stmt);
//void STDCALL mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset);
typedef void mysql_stmt_data_seekFnType(MYSQL_STMT *stmt, my_ulonglong offset);
//my_ulonglong STDCALL mysql_stmt_num_rows(MYSQL_STMT *stmt);
typedef my_ulonglong mysql_stmt_num_rowsFnType(MYSQL_STMT *stmt);
//my_ulonglong STDCALL mysql_stmt_affected_rows(MYSQL_STMT *stmt);
typedef my_ulonglong mysql_stmt_affected_rowsFnType(MYSQL_STMT *stmt);
//my_ulonglong STDCALL mysql_stmt_insert_id(MYSQL_STMT *stmt);
typedef my_ulonglong mysql_stmt_insert_idFnType(MYSQL_STMT *stmt);
//unsigned int STDCALL mysql_stmt_field_count(MYSQL_STMT *stmt);
typedef unsigned int mysql_stmt_field_countFnType(MYSQL_STMT *stmt);
//my_bool STDCALL mysql_commit(MYSQL * mysql);
typedef my_bool mysql_commitFnType(MYSQL * mysql);
//my_bool STDCALL mysql_rollback(MYSQL * mysql);
typedef my_bool mysql_rollbackFnType(MYSQL * mysql);
//my_bool STDCALL mysql_autocommit(MYSQL * mysql, my_bool auto_mode);
typedef my_bool mysql_autocommitFnType(MYSQL * mysql, my_bool auto_mode);
//my_bool STDCALL mysql_more_results(MYSQL *mysql);
typedef my_bool mysql_more_resultsFnType(MYSQL *mysql);
//int STDCALL mysql_next_result(MYSQL *mysql);
typedef int mysql_next_resultFnType(MYSQL *mysql);
//void STDCALL mysql_close(MYSQL *sock);
typedef void mysql_closeFnType(MYSQL *sock);
//MYSQL *        STDCALL mysql_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd);
typedef MYSQL * mysql_connectFnType(MYSQL *mysql, const char *host, const char *user, const char *passwd);
//int        STDCALL mysql_create_db(MYSQL *mysql, const char *DB);
typedef int mysql_create_dbFnType(MYSQL *mysql, const char *DB);
//int        STDCALL mysql_drop_db(MYSQL *mysql, const char *DB);
typedef int mysql_drop_dbFnType(MYSQL *mysql, const char *DB);


namespace mysql {

class MySQLClient {
private:

    static Logger &log;

    bool initOK;

    boost::mutex initMutex;

    DynLib *dynLib;

    // cached function pointers
    mysql_free_resultFnType* mysql_free_resultFn;

public:
    MySQLClient();
    ~MySQLClient();

    bool init();
    void *get_mysql_function(const char *name);

    // utility functions that do not operate on a MYSQL handle
    const char *    mysql_get_client_info(void);
    unsigned long   mysql_get_client_version(void);
    unsigned long   mysql_escape_string(char *to,const char *from, unsigned long from_length);
    unsigned long   mysql_hex_string(char *to,const char *from, unsigned long from_length);
    unsigned long   mysql_real_escape_string(MYSQL *_mysql, char *to,const char *from, unsigned long length);
    void            mysql_debug(const char *debug);
    unsigned int    mysql_thread_safe(void);
    my_bool         mysql_embedded(void);

    // connection operations
    MYSQL *         mysql_init(MYSQL *);
    unsigned int    mysql_errno(MYSQL *);
    const char *    mysql_error(MYSQL *);
    my_bool mysql_autocommit(MYSQL *_ignore, my_bool auto_mode);
    MYSQL_STMT * mysql_stmt_init(MYSQL *mysql);
    int mysql_query(MYSQL *mysql, const char *sql);
    MYSQL_RES * mysql_store_result(MYSQL *mysql);
    unsigned int mysql_stmt_errno(MYSQL_STMT *stmt);
    const char * mysql_stmt_error(MYSQL_STMT *stmt);
    int mysql_server_init(int argc, char **argv, char **groups);
    void mysql_server_end(void);
    MYSQL_PARAMETERS * mysql_get_parameters(void);
    my_bool mysql_thread_init(void);
    void mysql_thread_end(void);
    unsigned int mysql_field_count(MYSQL *mysql);
    my_ulonglong mysql_affected_rows(MYSQL *mysql);
    const char * mysql_sqlstate(MYSQL *mysql);
    unsigned int mysql_warning_count(MYSQL *mysql);
    const char * mysql_info(MYSQL *mysql);
    unsigned long mysql_thread_id(MYSQL *mysql);
    const char * mysql_character_set_name(MYSQL *mysql);
    int mysql_set_character_set(MYSQL *mysql, const char *csname);
    my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher);
    const char * mysql_get_ssl_cipher(MYSQL *mysql);
    my_bool mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db);
    int mysql_send_query(MYSQL *mysql, const char *q, unsigned long length);
    MYSQL_RES * mysql_use_result(MYSQL *mysql);
    my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length);
    my_bool mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length);
    void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset);
    void mysql_enable_rpl_parse(MYSQL *mysql);
    void mysql_disable_rpl_parse(MYSQL *mysql);
    int mysql_rpl_parse_enabled(MYSQL *mysql);
    void mysql_enable_reads_from_master(MYSQL *mysql);
    void mysql_disable_reads_from_master(MYSQL *mysql);
    my_bool mysql_reads_from_master_enabled(MYSQL *mysql);
    enum mysql_rpl_type mysql_rpl_query_type(const char* q, int len);
    my_bool         mysql_rpl_probe(MYSQL *mysql);
    int             mysql_set_master(MYSQL *mysql, const char* host, unsigned int port, const char* user, const char* passwd);
    int             mysql_add_slave(MYSQL *mysql, const char* host, unsigned int port, const char* user, const char* passwd);
    int             mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level);
    int             mysql_dump_debug_info(MYSQL *mysql);
    int             mysql_refresh(MYSQL *mysql, unsigned int refresh_options);
    int             mysql_kill(MYSQL *mysql,unsigned long pid);
    int             mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option);
    int             mysql_ping(MYSQL *mysql);
    const char *    mysql_stat(MYSQL *mysql);
    const char *    mysql_get_server_info(MYSQL *mysql);
    const char *    mysql_get_host_info(MYSQL *mysql);
    unsigned long   mysql_get_server_version(MYSQL *mysql);
    unsigned int    mysql_get_proto_info(MYSQL *mysql);
    MYSQL_RES *     mysql_list_dbs(MYSQL *mysql,const char *wild);
    MYSQL_RES *     mysql_list_tables(MYSQL *mysql,const char *wild);
    MYSQL_RES *     mysql_list_processes(MYSQL *mysql);
    int             mysql_options(MYSQL *mysql,enum mysql_option option, const char *arg);
    int             mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg);
    MYSQL_RES *     mysql_list_fields(MYSQL *mysql, const char *table, const char *wild);
    void            myodbc_remove_escape(MYSQL *mysql,char *name);
    my_bool         mysql_more_results(MYSQL *mysql);
    int             mysql_next_result(MYSQL *mysql);
    int             mysql_create_db(MYSQL *mysql, const char *DB);
    int             mysql_drop_db(MYSQL *mysql, const char *DB);
    my_bool         mysql_read_query_result(MYSQL *mysql);

    // result set operations
    void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
    MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
    MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
    unsigned long * mysql_fetch_lengths(MYSQL_RES *result);
    MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result);
    void mysql_free_result(MYSQL_RES *res);
    my_ulonglong mysql_num_rows(MYSQL_RES *res);
    unsigned int mysql_num_fields(MYSQL_RES *res);
    my_bool mysql_eof(MYSQL_RES *res);
    MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr);
    MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res);
    MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res);
    MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res);

    // statement operations
    int mysql_stmt_fetch(MYSQL_STMT *stmt);
    int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset);
    int mysql_stmt_store_result(MYSQL_STMT *stmt);
    unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt);
    my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr);
    my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr);
    my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
    my_bool mysql_stmt_reset(MYSQL_STMT * stmt);
    my_bool mysql_stmt_free_result(MYSQL_STMT *stmt);
    my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length);
    MYSQL_RES * mysql_stmt_result_metadata(MYSQL_STMT *stmt);
    MYSQL_RES * mysql_stmt_param_metadata(MYSQL_STMT *stmt);
    const char * mysql_stmt_sqlstate(MYSQL_STMT * stmt);
    MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset);
    MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt);
    void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset);
    my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt);
    my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt);
    my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt);
    unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt);
    int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length);
    my_bool mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND * bnd);
    int mysql_stmt_execute(MYSQL_STMT *stmt);
    my_bool mysql_stmt_close(MYSQL_STMT *stmt);

    // MySQL Manager
    MYSQL_MANAGER* mysql_manager_init(MYSQL_MANAGER* con);
    MYSQL_MANAGER* mysql_manager_connect(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port);
    void mysql_manager_close(MYSQL_MANAGER* con);
    int mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len);
    int mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf, int res_buf_size);
};

}

#endif // __MySQLClient_h__

