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


#include <stdio.h>
#include <map>
#include <string.h>
#include <string>
#include <mysql.h>
//#include <my_sys.h>
#include <dlfcn.h>

#include <mysql/MySQLClient.h>
#include <util/Util.h>
#include <util/MutexLock.h>
#include <logger/Logger.h>


/*Not needed with the compatibility merge.
#ifdef MYSQL_5_0
//extern const char *my_progname = "libmysqlclient_15";
#endif

#ifdef MYSQL_5_0_R
extern const char *my_progname = "libmysqlclient_r_15";
#endif

#ifdef MYSQL_5_1
//extern const char *my_progname = "libmysqlclient_16";
//*my_progname = "libmysqlclient_16";
#endif

#ifdef MYSQL_5_1_R
//extern const char *my_progname = "libmysqlclient_r_16";
//*my_progname = "libmysqlclient_r_16"
#endif

#ifdef MYSQL_5_5
extern const char *my_progname = "libmysqlclient_18";
#endif

#ifdef MYSQL_5_5_R
extern const char *my_progname = "libmysqlclient_r_18";
#endif
*/

using namespace logger;
using namespace util;

namespace mysql {

Logger &MySQLClient::log = Logger::getLogger("MySQLClient");

MySQLClient::MySQLClient() {
    initOK = false;
    dynLib = NULL;
    // init function pointers
    mysql_free_resultFn = NULL;
}

bool MySQLClient::init() {

    if (initOK) {
        return true;
    }

    if (log.isDebugEnabled()) {
        log.debug(string("Loading library ") + string(MYSQL_LIBRARY));
    }
    
    dynLib = new DynLib(MYSQL_LIBRARY);

    // mysql_server_init
    mysql_server_initFnType* mysql_server_initFn = (mysql_server_initFnType*)get_mysql_function("mysql_server_init");
    if (NULL == mysql_server_initFn) {
        log.error(string("Failed to load library ") + string(MYSQL_LIBRARY));
        return false;
    }
    if (0 != mysql_server_initFn(0, NULL, NULL)) {
        log.error("mysql_server_init() FAILED");
        // this does not work cerr << "mysql_server_init() FAILED WITH " << mysql_errno(NULL) << mysql_error(NULL) << endl;
        return false;
    }
    else {
        initOK = true;
    }

    /*
    // mysql_init
    mysql_initFnType* mysql_initFn = (mysql_initFnType*)get_mysql_function("mysql_init");
    if (NULL == mysql_initFn) {
        log.error(string("Failed to load library ") + string(MYSQL_LIBRARY));
        return false;
    }
    MYSQL mysql;
    if (0 != mysql_initFn(&mysql)) {
        log.error("mysql_init() FAILED");
        cerr << "mysql_init() FAILED WITH " << mysql_errno(&mysql) << mysql_error(&mysql) << endl;
        return false;
    }
    */          

    if (log.isTraceEnabled()) {
        //TODO: show error if major version mismatch
        log.trace(string("This driver was compiled against MySQL version ") + string(MYSQL_SERVER_VERSION));
        log.trace(string("The MySQL driver loaded at runtime is ") + Util::toString((long)this->mysql_get_client_version()));
    }

    return initOK;
}

MySQLClient::~MySQLClient() {
}

void *MySQLClient::get_mysql_function(const char *name) {
    // debug output
    if (name==NULL){
        log.error("get_mysql_function() passed null function name");
        return NULL;
    }
    if (dynLib==NULL) {
        log.error("get_mysql_function() cannot be called before MySQLClient::init() is called");
        return NULL;
    }

    void *ret = dynLib->get_function(name);
    if (ret==NULL) {
        log.error(string("get_mysql_function() failed to locate MySQL function '") + string(name) + string("'"));
    }
    return ret;
}

const char * MySQLClient::mysql_get_client_info(void) {
    if (!initOK) return NULL;
    mysql_get_client_infoFnType* tempFunction = (mysql_get_client_infoFnType*)get_mysql_function("mysql_get_client_info");
    const char * tempValue = tempFunction();
    return tempValue;
}

unsigned long MySQLClient::mysql_get_client_version(void) {
    if (!initOK) return 0;
    mysql_get_client_versionFnType* tempFunction = (mysql_get_client_versionFnType*)get_mysql_function("mysql_get_client_version");
    unsigned long tempValue = tempFunction();
    return tempValue;
}

MYSQL *MySQLClient::mysql_init(MYSQL *mysql) {
    if (!initOK) return NULL;
    mysql_initFnType* mysql_initFn = (mysql_initFnType*)get_mysql_function("mysql_init");
    return mysql_initFn(mysql);
}

unsigned int MySQLClient::mysql_errno(MYSQL *mysql){
    if (!initOK) return 0;
    mysql_errnoFnType* tempFunction = (mysql_errnoFnType*)get_mysql_function("mysql_errno");
    unsigned int tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_error(MYSQL *mysql){
    if (!initOK) return NULL;
    mysql_errorFnType* tempFunction = (mysql_errorFnType*)get_mysql_function("mysql_error");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

unsigned long MySQLClient::mysql_escape_string(char *to,const char *from, unsigned long from_length){
    if (!initOK) return 0;
    mysql_escape_stringFnType* tempFunction = (mysql_escape_stringFnType*)get_mysql_function("mysql_escape_string");
    unsigned long tempValue = tempFunction(to, from, from_length);
    return tempValue;
}

unsigned long MySQLClient::mysql_hex_string(char *to,const char *from, unsigned long from_length){
    if (!initOK) return 0;
    mysql_hex_stringFnType* tempFunction = (mysql_hex_stringFnType*)get_mysql_function("mysql_hex_string");
    unsigned long tempValue = tempFunction(to, from, from_length);
    return tempValue;
}

unsigned long MySQLClient::mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length){
    mysql_real_escape_stringFnType* tempFunction = (mysql_real_escape_stringFnType*)get_mysql_function("mysql_real_escape_string");
    unsigned long tempValue = tempFunction(mysql, to, from, length);
    return tempValue;
}

void MySQLClient::mysql_free_result(MYSQL_RES *res) {
    if (!initOK) return;
    if (!mysql_free_resultFn) {
        mysql_free_resultFn = (mysql_free_resultFnType*)get_mysql_function("mysql_free_result");
    }
    mysql_free_resultFn(res);
}

my_bool MySQLClient::mysql_autocommit(MYSQL *mysql, my_bool auto_mode){
    mysql_autocommitFnType* tempFunction = (mysql_autocommitFnType*)get_mysql_function("mysql_autocommit");
    my_bool tempValue = tempFunction(mysql, auto_mode);
    return tempValue;
}

MYSQL_STMT * MySQLClient::mysql_stmt_init(MYSQL *mysql){
    mysql_stmt_initFnType* tempFunction = (mysql_stmt_initFnType*)get_mysql_function("mysql_stmt_init");
    MYSQL_STMT *mysql_stmt = tempFunction(mysql);
    return mysql_stmt;
}

int MySQLClient::mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length){
    mysql_stmt_prepareFnType* tempFunction = (mysql_stmt_prepareFnType*)get_mysql_function("mysql_stmt_prepare");
    int tempValue = tempFunction(stmt, query, length);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND * bnd){
    mysql_stmt_bind_paramFnType* tempFunction = (mysql_stmt_bind_paramFnType*)get_mysql_function("mysql_stmt_bind_param");
    my_bool tempValue = tempFunction(stmt, bnd);
    return tempValue;
}

int MySQLClient::mysql_stmt_execute(MYSQL_STMT *stmt){
    mysql_stmt_executeFnType* tempFunction = (mysql_stmt_executeFnType*)get_mysql_function("mysql_stmt_execute");
    int tempValue = tempFunction(stmt);
    return tempValue;
}

int MySQLClient::mysql_query(MYSQL *mysql, const char *sql){
    return mysql_real_query(mysql, sql, strlen(sql));
}

MYSQL_RES * MySQLClient::mysql_store_result(MYSQL *mysql){
    mysql_store_resultFnType *tempFunction = (mysql_store_resultFnType*)get_mysql_function("mysql_store_result");
    MYSQL_RES *tempValue = tempFunction(mysql);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_close(MYSQL_STMT *stmt){
    mysql_stmt_closeFnType* tempFunction = (mysql_stmt_closeFnType*)get_mysql_function("mysql_stmt_close");
    my_bool tempValue = tempFunction(stmt);
    return tempValue;
}

unsigned int MySQLClient::mysql_stmt_errno(MYSQL_STMT *stmt){
    mysql_stmt_errnoFnType* tempFunction = (mysql_stmt_errnoFnType*)get_mysql_function("mysql_stmt_errno");
    unsigned int tempValue = tempFunction(stmt);
    return tempValue;
}

const char * MySQLClient::mysql_stmt_error(MYSQL_STMT *stmt){
    mysql_stmt_errorFnType* tempFunction = (mysql_stmt_errorFnType*)get_mysql_function("mysql_stmt_error");
    const char * tempValue = tempFunction(stmt);
    return tempValue;
}

int MySQLClient::mysql_server_init(int argc, char **argv, char **groups){
    //trace("mysql_server_init", mysql);
    mysql_server_initFnType* tempFunction = (mysql_server_initFnType*)get_mysql_function("mysql_server_init");
    int tempValue = tempFunction(argc, argv, groups);
    return tempValue;
}

void MySQLClient::mysql_server_end(void){
    mysql_server_endFnType* tempFunction = (mysql_server_endFnType*)get_mysql_function("mysql_server_end");
    tempFunction();
}

MYSQL_PARAMETERS * MySQLClient::mysql_get_parameters(void){
    mysql_get_parametersFnType* tempFunction = (mysql_get_parametersFnType*)get_mysql_function("mysql_get_parameters");
    MYSQL_PARAMETERS * tempValue = tempFunction();
    return tempValue;
}

my_bool MySQLClient::mysql_thread_init(void){
    mysql_thread_initFnType* tempFunction = (mysql_thread_initFnType*)get_mysql_function("mysql_thread_init");
    my_bool tempValue = tempFunction();
    return tempValue;
}

void MySQLClient::mysql_thread_end(void){
    mysql_thread_endFnType* tempFunction = (mysql_thread_endFnType*)get_mysql_function("mysql_thread_end");
    tempFunction();
}

my_ulonglong MySQLClient::mysql_num_rows(MYSQL_RES *res){
    mysql_num_rowsFnType* mysql_num_rowsFn = (mysql_num_rowsFnType*)get_mysql_function("mysql_num_rows");
    my_ulonglong tempValue = mysql_num_rowsFn(res);
    return tempValue;
}

unsigned int MySQLClient::mysql_num_fields(MYSQL_RES *res){
    mysql_num_fieldsFnType* mysql_num_fieldsFn = (mysql_num_fieldsFnType*)get_mysql_function("mysql_num_fields");
    unsigned int tempValue = mysql_num_fieldsFn(res);
    return tempValue;
}

my_bool MySQLClient::mysql_eof(MYSQL_RES *res){
    mysql_eofFnType* tempFunction = (mysql_eofFnType*)get_mysql_function("mysql_eof");
    my_bool tempValue = tempFunction(res);
    return tempValue;
}

MYSQL_FIELD * MySQLClient::mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr){
    mysql_fetch_field_directFnType* tempFunction = (mysql_fetch_field_directFnType*)get_mysql_function("mysql_fetch_field_direct");
    MYSQL_FIELD * tempValue = tempFunction(res, fieldnr);
    return tempValue;
}

MYSQL_FIELD * MySQLClient::mysql_fetch_fields(MYSQL_RES *res){
    mysql_fetch_fieldsFnType* tempFunction = (mysql_fetch_fieldsFnType*)get_mysql_function("mysql_fetch_fields");
    MYSQL_FIELD * tempValue = tempFunction(res);
    return tempValue;
}

MYSQL_ROW_OFFSET MySQLClient::mysql_row_tell(MYSQL_RES *res){
    mysql_row_tellFnType* mysql_row_tellFn = (mysql_row_tellFnType*)get_mysql_function("mysql_row_tell");
    MYSQL_ROW_OFFSET tempValue = mysql_row_tellFn(res);
    return tempValue;
}

MYSQL_FIELD_OFFSET MySQLClient::mysql_field_tell(MYSQL_RES *res){
    mysql_field_tellFnType* tempFunction = (mysql_field_tellFnType*)get_mysql_function("mysql_field_tell");
    MYSQL_FIELD_OFFSET tempValue = tempFunction(res);
    return tempValue;
}

unsigned int MySQLClient::mysql_field_count(MYSQL *mysql){
    mysql_field_countFnType* tempFunction = (mysql_field_countFnType*)get_mysql_function("mysql_field_count");
    unsigned int tempValue = tempFunction(mysql);
    return tempValue;
}

my_ulonglong MySQLClient::mysql_affected_rows(MYSQL *mysql){
    mysql_affected_rowsFnType* tempFunction = (mysql_affected_rowsFnType*)get_mysql_function("mysql_affected_rows");
    my_ulonglong tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_sqlstate(MYSQL *mysql){
    mysql_sqlstateFnType* tempFunction = (mysql_sqlstateFnType*)get_mysql_function("mysql_sqlstate");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

unsigned int MySQLClient::mysql_warning_count(MYSQL *mysql){
    mysql_warning_countFnType* tempFunction = (mysql_warning_countFnType*)get_mysql_function("mysql_warning_count");
    unsigned int tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_info(MYSQL *mysql){
    mysql_infoFnType* tempFunction = (mysql_infoFnType*)get_mysql_function("mysql_info");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

unsigned long MySQLClient::mysql_thread_id(MYSQL *mysql){
    mysql_thread_idFnType* tempFunction = (mysql_thread_idFnType*)get_mysql_function("mysql_thread_id");
    unsigned long tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_character_set_name(MYSQL *mysql){
    mysql_character_set_nameFnType* tempFunction = (mysql_character_set_nameFnType*)get_mysql_function("mysql_character_set_name");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

int MySQLClient::mysql_set_character_set(MYSQL *mysql, const char *csname){
    //trace("mysql_set_character_set", mysql);
    mysql_set_character_setFnType* tempFunction = (mysql_set_character_setFnType*)get_mysql_function("mysql_set_character_set");
    int tempValue = tempFunction(mysql, csname);
    return tempValue;
}

my_bool MySQLClient::mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher){
    mysql_ssl_setFnType* tempFunction = (mysql_ssl_setFnType*)get_mysql_function("mysql_ssl_set");
    my_bool tempValue = tempFunction(mysql, key, cert, ca, capath, cipher);
    return tempValue;
}

const char * MySQLClient::mysql_get_ssl_cipher(MYSQL *mysql){
    mysql_get_ssl_cipherFnType* tempFunction = (mysql_get_ssl_cipherFnType*)get_mysql_function("mysql_get_ssl_cipher");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

my_bool MySQLClient::mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db){
    mysql_change_userFnType* tempFunction = (mysql_change_userFnType*)get_mysql_function("mysql_change_user");
    my_bool tempValue = tempFunction(mysql, user, passwd, db);
    return tempValue;
}

int MySQLClient::mysql_send_query(MYSQL *mysql, const char *q, unsigned long length){
    //trace("mysql_send_query", mysql);
    mysql_send_queryFnType* tempFunction = (mysql_send_queryFnType*)get_mysql_function("mysql_send_query");
    int tempValue = tempFunction(mysql, q, length);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_use_result(MYSQL *mysql){
    mysql_use_resultFnType* tempFunction = (mysql_use_resultFnType*)get_mysql_function("mysql_use_result");
    MYSQL_RES * tempValue = tempFunction(mysql);
    return tempValue;
}

my_bool MySQLClient::mysql_master_query(MYSQL *mysql, const char *q, unsigned long length){
    mysql_master_queryFnType* tempFunction = (mysql_master_queryFnType*)get_mysql_function("mysql_master_query");
    my_bool tempValue = tempFunction(mysql, q, length);
    return tempValue;
}

my_bool MySQLClient::mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length){
    mysql_master_send_queryFnType* tempFunction = (mysql_master_send_queryFnType*)get_mysql_function("mysql_master_send_query");
    my_bool tempValue = tempFunction(mysql, q, length);
    return tempValue;
}

my_bool MySQLClient::mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length){
    mysql_slave_queryFnType* tempFunction = (mysql_slave_queryFnType*)get_mysql_function("mysql_slave_query");
    my_bool tempValue = tempFunction(mysql, q, length);
    return tempValue;
}

my_bool MySQLClient::mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length){
    mysql_slave_send_queryFnType* tempFunction = (mysql_slave_send_queryFnType*)get_mysql_function("mysql_slave_send_query");
    my_bool tempValue = tempFunction(mysql, q, length);
    return tempValue;
}

void MySQLClient::mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset){

//    log.debug(string("FOO mysql=") + Util::toString((void*)mysql));
//    log.debug(string("FOO charset=") + Util::toString((void*)charset));
//    log.debug(string("FOO mysql=") + Util::toString((void*)mysql));

    mysql_get_character_set_infoFnType* tempFunction = (mysql_get_character_set_infoFnType*)get_mysql_function("mysql_get_character_set_info");

//    log.debug(string("FOO func=") + Util::toString((void*)tempFunction));
    tempFunction(mysql, charset);
}

void MySQLClient::mysql_enable_rpl_parse(MYSQL *mysql){
    mysql_enable_rpl_parseFnType* tempFunction = (mysql_enable_rpl_parseFnType*)get_mysql_function("mysql_enable_rpl_parse");
    tempFunction(mysql);
}

void MySQLClient::mysql_disable_rpl_parse(MYSQL *mysql){
    mysql_disable_rpl_parseFnType* tempFunction = (mysql_disable_rpl_parseFnType*)get_mysql_function("mysql_disable_rpl_parse");
    tempFunction(mysql);
}

int MySQLClient::mysql_rpl_parse_enabled(MYSQL *mysql){
    mysql_rpl_parse_enabledFnType* tempFunction = (mysql_rpl_parse_enabledFnType*)get_mysql_function("mysql_rpl_parse_enabled");
    int tempValue = tempFunction(mysql);
    return tempValue;
}

void MySQLClient::mysql_enable_reads_from_master(MYSQL *mysql){
    mysql_enable_reads_from_masterFnType* tempFunction = (mysql_enable_reads_from_masterFnType*)get_mysql_function("mysql_enable_reads_from_master");
    tempFunction(mysql);
}

void MySQLClient::mysql_disable_reads_from_master(MYSQL *mysql){
    mysql_disable_reads_from_masterFnType* tempFunction = (mysql_disable_reads_from_masterFnType*)get_mysql_function("mysql_disable_reads_from_master");
    tempFunction(mysql);
}

my_bool MySQLClient::mysql_reads_from_master_enabled(MYSQL *mysql){
    mysql_reads_from_master_enabledFnType* tempFunction = (mysql_reads_from_master_enabledFnType*)get_mysql_function("mysql_reads_from_master_enabled");
    my_bool tempValue = tempFunction(mysql);
    return tempValue;
}

enum mysql_rpl_type MySQLClient::mysql_rpl_query_type(const char* q, int len){
    mysql_rpl_query_typeFnType* tempFunction = (mysql_rpl_query_typeFnType*)get_mysql_function("mysql_rpl_query_type");
    enum mysql_rpl_type tempValue = tempFunction(q, len);
    return tempValue;
}

my_bool MySQLClient::mysql_rpl_probe(MYSQL *mysql){
    mysql_rpl_probeFnType* tempFunction = (mysql_rpl_probeFnType*)get_mysql_function("mysql_rpl_probe");
    my_bool tempValue = tempFunction(mysql);
    return tempValue;
}

int MySQLClient::mysql_set_master(MYSQL *mysql, const char* host, unsigned int port, const char* user, const char* passwd){
    mysql_set_masterFnType* tempFunction = (mysql_set_masterFnType*)get_mysql_function("mysql_set_master");
    int tempValue = tempFunction(mysql, host, port, user, passwd);
    return tempValue;
}

int MySQLClient::mysql_add_slave(MYSQL *mysql, const char* host, unsigned int port, const char* user, const char* passwd){
    mysql_add_slaveFnType* tempFunction = (mysql_add_slaveFnType*)get_mysql_function("mysql_add_slave");
    int tempValue = tempFunction(mysql, host, port, user, passwd);
    return tempValue;
}

int MySQLClient::mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level){
    mysql_shutdownFnType* tempFunction = (mysql_shutdownFnType*)get_mysql_function("mysql_shutdown");
    int tempValue = tempFunction(mysql, shutdown_level);
    return tempValue;
}

int MySQLClient::mysql_dump_debug_info(MYSQL *mysql){
    mysql_dump_debug_infoFnType* tempFunction = (mysql_dump_debug_infoFnType*)get_mysql_function("mysql_dump_debug_info");
    int tempValue = tempFunction(mysql);
    return tempValue;
}

int MySQLClient::mysql_refresh(MYSQL *mysql, unsigned int refresh_options){
    mysql_refreshFnType* tempFunction = (mysql_refreshFnType*)get_mysql_function("mysql_refresh");
    int tempValue = tempFunction(mysql, refresh_options);
    return tempValue;
}

int MySQLClient::mysql_kill(MYSQL *mysql,unsigned long pid){
    mysql_killFnType* tempFunction = (mysql_killFnType*)get_mysql_function("mysql_kill");
    int tempValue = tempFunction(mysql, pid);
    return tempValue;
}

int MySQLClient::mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option){
    mysql_set_server_optionFnType* tempFunction = (mysql_set_server_optionFnType*)get_mysql_function("mysql_set_server_option");
    int tempValue = tempFunction(mysql, option);
    return tempValue;
}

int MySQLClient::mysql_ping(MYSQL *mysql){
    mysql_pingFnType* tempFunction = (mysql_pingFnType*)get_mysql_function("mysql_ping");
    int tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_stat(MYSQL *mysql){
    mysql_statFnType* tempFunction = (mysql_statFnType*)get_mysql_function("mysql_stat");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_get_server_info(MYSQL *mysql){
    mysql_get_server_infoFnType* tempFunction = (mysql_get_server_infoFnType*)get_mysql_function("mysql_get_server_info");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

const char * MySQLClient::mysql_get_host_info(MYSQL *mysql){
    mysql_get_host_infoFnType* tempFunction = (mysql_get_host_infoFnType*)get_mysql_function("mysql_get_host_info");
    const char * tempValue = tempFunction(mysql);
    return tempValue;
}

unsigned long MySQLClient::mysql_get_server_version(MYSQL *mysql){
    mysql_get_server_versionFnType* tempFunction = (mysql_get_server_versionFnType*)get_mysql_function("mysql_get_server_version");
    unsigned long tempValue = tempFunction(mysql);
    return tempValue;
}

unsigned int MySQLClient::mysql_get_proto_info(MYSQL *mysql){
    mysql_get_proto_infoFnType* tempFunction = (mysql_get_proto_infoFnType*)get_mysql_function("mysql_get_proto_info");
    unsigned int tempValue = tempFunction(mysql);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_list_dbs(MYSQL *mysql,const char *wild){
    mysql_list_dbsFnType* tempFunction = (mysql_list_dbsFnType*)get_mysql_function("mysql_list_dbs");
    MYSQL_RES * tempValue = tempFunction(mysql, wild);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_list_tables(MYSQL *mysql,const char *wild){
    mysql_list_tablesFnType* tempFunction = (mysql_list_tablesFnType*)get_mysql_function("mysql_list_tables");
    MYSQL_RES * tempValue = tempFunction(mysql, wild);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_list_processes(MYSQL *mysql){
    mysql_list_processesFnType* tempFunction = (mysql_list_processesFnType*)get_mysql_function("mysql_list_processes");
    MYSQL_RES * tempValue = tempFunction(mysql);
    return tempValue;
}

int MySQLClient::mysql_options(MYSQL *mysql,enum mysql_option option, const char *arg){
    mysql_optionsFnType* tempFunction = (mysql_optionsFnType*)get_mysql_function("mysql_options");
    int tempValue = tempFunction(mysql, option, arg);
    return tempValue;
}

int MySQLClient::mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg){
    mysql_optionsFnType* tempFunction = (mysql_optionsFnType*)get_mysql_function("mysql_options");
    int tempValue = tempFunction(mysql, option, arg);
    return tempValue;
}

void MySQLClient::mysql_data_seek(MYSQL_RES *result, my_ulonglong offset){
    mysql_data_seekFnType* mysql_data_seekFn = (mysql_data_seekFnType*)get_mysql_function("mysql_data_seek");
    mysql_data_seekFn(result, offset);
}

MYSQL_ROW_OFFSET MySQLClient::mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset){
    mysql_row_seekFnType*mysql_row_seekFn = (mysql_row_seekFnType*)get_mysql_function("mysql_row_seek");
    MYSQL_ROW_OFFSET tempValue = mysql_row_seekFn(result, offset);
    return tempValue;
}

MYSQL_FIELD_OFFSET MySQLClient::mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset){
    mysql_field_seekFnType* mysql_field_seekFn = (mysql_field_seekFnType*)get_mysql_function("mysql_field_seek");
    MYSQL_FIELD_OFFSET tempValue = mysql_field_seekFn(result, offset);
    return tempValue;
}

MYSQL_ROW MySQLClient::mysql_fetch_row(MYSQL_RES *result){
    mysql_fetch_rowFnType* mysql_fetch_rowFn = (mysql_fetch_rowFnType*)get_mysql_function("mysql_fetch_row");
    MYSQL_ROW tempValue = mysql_fetch_rowFn(result);
    return tempValue;
}

unsigned long * MySQLClient::mysql_fetch_lengths(MYSQL_RES *result){
    mysql_fetch_lengthsFnType* mysql_fetch_lengthsFn = (mysql_fetch_lengthsFnType*)get_mysql_function("mysql_fetch_lengths");
    unsigned long * tempValue = mysql_fetch_lengthsFn(result);
    return tempValue;
}

MYSQL_FIELD * MySQLClient::mysql_fetch_field(MYSQL_RES *result){
    mysql_fetch_fieldFnType* tempFunction = (mysql_fetch_fieldFnType*)get_mysql_function("mysql_fetch_field");
    MYSQL_FIELD * tempValue = tempFunction(result);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_list_fields(MYSQL *mysql, const char *table, const char *wild){
    mysql_list_fieldsFnType* tempFunction = (mysql_list_fieldsFnType*)get_mysql_function("mysql_list_fields");
    MYSQL_RES * tempValue = tempFunction(mysql, table, wild);
    return tempValue;
}

void MySQLClient::mysql_debug(const char *debug){
    mysql_debugFnType* tempFunction = (mysql_debugFnType*)get_mysql_function("mysql_debug");
    tempFunction(debug);
}

void MySQLClient::myodbc_remove_escape(MYSQL *mysql,char *name){
    myodbc_remove_escapeFnType* tempFunction = (myodbc_remove_escapeFnType*)get_mysql_function("myodbc_remove_escape");
    tempFunction(mysql, name);
}

unsigned int MySQLClient::mysql_thread_safe(void){
    mysql_thread_safeFnType* tempFunction = (mysql_thread_safeFnType*)get_mysql_function("mysql_thread_safe");
    unsigned int tempValue = tempFunction();
    return tempValue;
}

my_bool MySQLClient::mysql_embedded(void){
    mysql_embeddedFnType* tempFunction = (mysql_embeddedFnType*)get_mysql_function("mysql_embedded");
    my_bool tempValue = tempFunction();
    return tempValue;
}

MYSQL_MANAGER* MySQLClient::mysql_manager_init(MYSQL_MANAGER* con){
    mysql_manager_initFnType* tempFunction = (mysql_manager_initFnType*)get_mysql_function("mysql_manager_init");
    MYSQL_MANAGER* tempValue = tempFunction(con);
    return tempValue;
}

MYSQL_MANAGER* MySQLClient::mysql_manager_connect(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port){
    mysql_manager_connectFnType* tempFunction = (mysql_manager_connectFnType*)get_mysql_function("mysql_manager_connect");
    MYSQL_MANAGER* tempValue = tempFunction(con, host, user, passwd, port);
    return tempValue;
}

void MySQLClient::mysql_manager_close(MYSQL_MANAGER* con){
    mysql_manager_closeFnType* tempFunction = (mysql_manager_closeFnType*)get_mysql_function("mysql_manager_close");
    tempFunction(con);
}

int MySQLClient::mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len){
    mysql_manager_commandFnType* tempFunction = (mysql_manager_commandFnType*)get_mysql_function("mysql_manager_command");
    int tempValue = tempFunction(con, cmd, cmd_len);
    return tempValue;
}

int MySQLClient::mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf, int res_buf_size){
    mysql_manager_fetch_lineFnType* tempFunction = (mysql_manager_fetch_lineFnType*)get_mysql_function("mysql_manager_fetch_line");
    int tempValue = tempFunction(con, res_buf, res_buf_size);
    return tempValue;
}

my_bool MySQLClient::mysql_read_query_result(MYSQL *mysql){
    mysql_read_query_resultFnType* tempFunction = (mysql_read_query_resultFnType*)get_mysql_function("mysql_read_query_result");
    my_bool tempValue = tempFunction(mysql);
    return tempValue;
}


int MySQLClient::mysql_stmt_fetch(MYSQL_STMT *stmt){
    mysql_stmt_fetchFnType* tempFunction = (mysql_stmt_fetchFnType*)get_mysql_function("mysql_stmt_fetch");
    int tempValue = tempFunction(stmt);
    return tempValue;
}

int MySQLClient::mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset){
    mysql_stmt_fetch_columnFnType* tempFunction = (mysql_stmt_fetch_columnFnType*)get_mysql_function("mysql_stmt_fetch_column");
    int tempValue = tempFunction(stmt, bind_arg, column, offset);
    return tempValue;
}

int MySQLClient::mysql_stmt_store_result(MYSQL_STMT *stmt){
    mysql_stmt_store_resultFnType* tempFunction = (mysql_stmt_store_resultFnType*)get_mysql_function("mysql_stmt_store_result");
    int tempValue = tempFunction(stmt);
    return tempValue;
}

unsigned long MySQLClient::mysql_stmt_param_count(MYSQL_STMT * stmt){
    mysql_stmt_param_countFnType* tempFunction = (mysql_stmt_param_countFnType*)get_mysql_function("mysql_stmt_param_count");
    unsigned long tempValue = tempFunction(stmt);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr){
    mysql_stmt_attr_setFnType* tempFunction = (mysql_stmt_attr_setFnType*)get_mysql_function("mysql_stmt_attr_set");
    my_bool tempValue = tempFunction(stmt, attr_type, attr);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_attr_get(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr){
    mysql_stmt_attr_getFnType* tempFunction = (mysql_stmt_attr_getFnType*)get_mysql_function("mysql_stmt_attr_get");
    my_bool tempValue = tempFunction(stmt, attr_type, attr);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd){
    mysql_stmt_bind_resultFnType* tempFunction = (mysql_stmt_bind_resultFnType*)get_mysql_function("mysql_stmt_bind_result");
    my_bool tempValue = tempFunction(stmt, bnd);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_reset(MYSQL_STMT * stmt){
    mysql_stmt_resetFnType* tempFunction = (mysql_stmt_resetFnType*)get_mysql_function("mysql_stmt_reset");
    my_bool tempValue = tempFunction(stmt);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_free_result(MYSQL_STMT *stmt){
    mysql_stmt_free_resultFnType* tempFunction = (mysql_stmt_free_resultFnType*)get_mysql_function("mysql_stmt_free_result");
    my_bool tempValue = tempFunction(stmt);
    return tempValue;
}

my_bool MySQLClient::mysql_stmt_send_long_data(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length){
    mysql_stmt_send_long_dataFnType* tempFunction = (mysql_stmt_send_long_dataFnType*)get_mysql_function("mysql_stmt_send_long_data");
    my_bool tempValue = tempFunction(stmt, param_number, data, length);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_stmt_result_metadata(MYSQL_STMT *stmt){
    mysql_stmt_result_metadataFnType* tempFunction = (mysql_stmt_result_metadataFnType*)get_mysql_function("mysql_stmt_result_metadata");
    MYSQL_RES * tempValue = tempFunction(stmt);
    return tempValue;
}

MYSQL_RES * MySQLClient::mysql_stmt_param_metadata(MYSQL_STMT *stmt){
    mysql_stmt_param_metadataFnType* tempFunction = (mysql_stmt_param_metadataFnType*)get_mysql_function("mysql_stmt_param_metadata");
    MYSQL_RES * tempValue = tempFunction(stmt);
    return tempValue;
}

const char * MySQLClient::mysql_stmt_sqlstate(MYSQL_STMT * stmt){
    mysql_stmt_sqlstateFnType* tempFunction = (mysql_stmt_sqlstateFnType*)get_mysql_function("mysql_stmt_sqlstate");
    const char * tempValue = tempFunction(stmt);
    return tempValue;
}

MYSQL_ROW_OFFSET MySQLClient::mysql_stmt_row_seek(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset){
    mysql_stmt_row_seekFnType* tempFunction = (mysql_stmt_row_seekFnType*)get_mysql_function("mysql_stmt_row_seek");
    MYSQL_ROW_OFFSET tempValue = tempFunction(stmt, offset);
    return tempValue;
}

MYSQL_ROW_OFFSET MySQLClient::mysql_stmt_row_tell(MYSQL_STMT *stmt){
    mysql_stmt_row_tellFnType* tempFunction = (mysql_stmt_row_tellFnType*)get_mysql_function("mysql_stmt_row_tell");
    MYSQL_ROW_OFFSET tempValue = tempFunction(stmt);
    return tempValue;
}

void MySQLClient::mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset){
    mysql_stmt_data_seekFnType* tempFunction = (mysql_stmt_data_seekFnType*)get_mysql_function("mysql_stmt_data_seek");
    tempFunction(stmt, offset);
}

my_ulonglong MySQLClient::mysql_stmt_num_rows(MYSQL_STMT *stmt){
    mysql_stmt_num_rowsFnType* tempFunction = (mysql_stmt_num_rowsFnType*)get_mysql_function("mysql_stmt_num_rows");
    my_ulonglong tempValue = tempFunction(stmt);
    return tempValue;
}

my_ulonglong MySQLClient::mysql_stmt_affected_rows(MYSQL_STMT *stmt){
    mysql_stmt_affected_rowsFnType* tempFunction = (mysql_stmt_affected_rowsFnType*)get_mysql_function("mysql_stmt_affected_rows");
    my_ulonglong tempValue = tempFunction(stmt);
    return tempValue;
}

my_ulonglong MySQLClient::mysql_stmt_insert_id(MYSQL_STMT *stmt){
    mysql_stmt_insert_idFnType* tempFunction = (mysql_stmt_insert_idFnType*)get_mysql_function("mysql_stmt_insert_id");
    my_ulonglong tempValue = tempFunction(stmt);
    return tempValue;
}

unsigned int MySQLClient::mysql_stmt_field_count(MYSQL_STMT *stmt){
    mysql_stmt_field_countFnType* tempFunction = (mysql_stmt_field_countFnType*)get_mysql_function("mysql_stmt_field_count");
    unsigned int tempValue = tempFunction(stmt);
    return tempValue;
}

my_bool MySQLClient::mysql_more_results(MYSQL *mysql){
    mysql_more_resultsFnType* mysql_more_resultsFn = (mysql_more_resultsFnType*)get_mysql_function("mysql_more_results");
    my_bool tempValue = mysql_more_resultsFn(mysql);
    return tempValue;
}

int MySQLClient::mysql_next_result(MYSQL *mysql){
    mysql_next_resultFnType* tempFunction = (mysql_next_resultFnType*)get_mysql_function("mysql_next_result");
    int tempValue = tempFunction(mysql);
    return tempValue;
}

int MySQLClient::mysql_create_db(MYSQL *mysql, const char *DB){
    mysql_create_dbFnType* tempFunction = (mysql_create_dbFnType*)get_mysql_function("mysql_create_db");
    int tempValue = tempFunction(mysql, DB);
    return tempValue;
}

int MySQLClient::mysql_drop_db(MYSQL *mysql, const char *DB){
    mysql_drop_dbFnType* tempFunction = (mysql_drop_dbFnType*)get_mysql_function("mysql_drop_db");
    int tempValue = tempFunction(mysql, DB);
    return tempValue;
}



} // namespace


