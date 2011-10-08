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


#ifndef DBSMYSQLABSTRACTCONNECTION_H_
#define DBSMYSQLABSTRACTCONNECTION_H_

#include <logger/Logger.h>

#include <mysql.h>

using namespace logger;

class MySQLAbstractConnection {
private:
    // logger
    static Logger &_log;

public:
    MySQLAbstractConnection();
    virtual ~MySQLAbstractConnection();

    // internal methods
    int notImplementedInt(const char *);
    void *notImplementedPtr(const char *);
    void setStale();
    virtual bool connect(const char *, const char *, const char *, const char *, unsigned int port, const char *unix_socket, unsigned long clientflag)=0;

    // MySQL API methods
    virtual my_bool mysql_autocommit(MYSQL * _mysql, my_bool auto_mode)=0;
    virtual MYSQL_STMT * mysql_stmt_init(MYSQL *_mysql)=0;
    virtual int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query, unsigned long length)=0;
    virtual int mysql_stmt_execute(MYSQL_STMT *stmt)=0;
    virtual int mysql_query(MYSQL *_mysql, const char *sql)=0;
    virtual MYSQL_RES * mysql_store_result(MYSQL *_mysql)=0;
    virtual my_bool mysql_commit(MYSQL * _mysql)=0;
    virtual void mysql_close(MYSQL *_mysql)=0;
    virtual int mysql_server_init(int argc, char **argv, char **groups)=0;
    virtual void mysql_server_end(void)=0;
    virtual MYSQL_PARAMETERS * mysql_get_parameters(void)=0;
    virtual my_bool mysql_thread_init(void)=0;
    virtual void mysql_thread_end(void)=0;
    virtual my_ulonglong mysql_num_rows(MYSQL_RES *res)=0;
    virtual unsigned int mysql_num_fields(MYSQL_RES *res)=0;
    virtual my_bool mysql_eof(MYSQL_RES *res)=0;
    virtual MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr)=0;
    virtual MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res)=0;
    virtual MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res)=0;
    virtual MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res)=0;
    virtual unsigned int mysql_field_count(MYSQL *mysql)=0;
    virtual my_ulonglong mysql_affected_rows(MYSQL *mysql)=0;
    virtual my_ulonglong mysql_insert_id(MYSQL *mysql)=0;
    virtual unsigned int mysql_errno(MYSQL *mysql)=0;
    virtual const char * mysql_error(MYSQL *mysql)=0;
    virtual const char * mysql_sqlstate(MYSQL *mysql)=0;
    virtual unsigned int mysql_warning_count(MYSQL *mysql)=0;
    virtual const char * mysql_info(MYSQL *mysql)=0;
    virtual unsigned long mysql_thread_id(MYSQL *mysql)=0;
    virtual const char * mysql_character_set_name(MYSQL *mysql)=0;
    virtual int mysql_set_character_set(MYSQL *mysql, const char *csname)=0;
    virtual my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher)=0;
    virtual const char * mysql_get_ssl_cipher(MYSQL *mysql)=0;
    virtual my_bool mysql_change_user(MYSQL *mysql, const char *user,  const char *passwd, const char *db)=0;
    virtual int mysql_select_db(MYSQL *mysql, const char *db)=0;
    virtual int mysql_send_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual int mysql_real_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual MYSQL_RES * mysql_use_result(MYSQL *mysql)=0;
    virtual my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual my_bool mysql_master_send_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual my_bool mysql_slave_send_query(MYSQL *mysql, const char *q, unsigned long length)=0;
    virtual void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset)=0;
    virtual void mysql_enable_rpl_parse(MYSQL* mysql)=0;
    virtual void mysql_disable_rpl_parse(MYSQL* mysql)=0;
    virtual int mysql_rpl_parse_enabled(MYSQL* mysql)=0;
    virtual void mysql_enable_reads_from_master(MYSQL* mysql)=0;
    virtual void mysql_disable_reads_from_master(MYSQL* mysql)=0;
    virtual my_bool mysql_reads_from_master_enabled(MYSQL* mysql)=0;
    virtual enum mysql_rpl_type mysql_rpl_query_type(const char* q, int len)=0;
    virtual my_bool mysql_rpl_probe(MYSQL* mysql)=0;
    virtual int mysql_set_master(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd)=0;
    virtual int mysql_add_slave(MYSQL* mysql, const char* host, unsigned int port, const char* user, const char* passwd)=0;
    virtual int mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level)=0;
    virtual int mysql_dump_debug_info(MYSQL *mysql)=0;
    virtual int mysql_refresh(MYSQL *mysql, unsigned int refresh_options)=0;
    virtual int mysql_kill(MYSQL *mysql,unsigned long pid)=0;
    virtual int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option)=0;
    virtual int mysql_ping(MYSQL *mysql)=0;
    virtual const char * mysql_stat(MYSQL *mysql)=0;
    virtual const char * mysql_get_server_info(MYSQL *mysql)=0;
    virtual const char * mysql_get_client_info(void)=0;
    virtual unsigned long mysql_get_client_version(void)=0;
    virtual const char * mysql_get_host_info(MYSQL *mysql)=0;
    virtual unsigned long mysql_get_server_version(MYSQL *mysql)=0;
    virtual unsigned int mysql_get_proto_info(MYSQL *mysql)=0;
    virtual MYSQL_RES * mysql_list_dbs(MYSQL *mysql,const char *wild)=0;
    virtual MYSQL_RES * mysql_list_tables(MYSQL *mysql,const char *wild)=0;
    virtual MYSQL_RES * mysql_list_processes(MYSQL *mysql)=0;

    virtual int mysql_options(MYSQL *mysql,enum mysql_option option, const char *arg)=0;
    virtual int mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg)=0;

    virtual void mysql_free_result(MYSQL_RES *result)=0;
    virtual void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset)=0;
    virtual MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset)=0;
    virtual MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset)=0;
    virtual MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)=0;
    virtual unsigned long * mysql_fetch_lengths(MYSQL_RES *result)=0;
    virtual MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result)=0;
    virtual MYSQL_RES * mysql_list_fields(MYSQL *mysql, const char *table, const char *wild)=0;
    virtual unsigned long mysql_escape_string(char *to,const char *from, unsigned long from_length)=0;
    virtual unsigned long mysql_hex_string(char *to,const char *from, unsigned long from_length)=0;
    virtual unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length)=0;
    virtual void mysql_debug(const char *debug)=0;
    virtual void myodbc_remove_escape(MYSQL *mysql,char *name)=0;
    virtual unsigned int mysql_thread_safe(void)=0;
    virtual my_bool mysql_embedded(void)=0;
    virtual MYSQL_MANAGER* mysql_manager_init(MYSQL_MANAGER* con)=0;
    virtual MYSQL_MANAGER* mysql_manager_connect(MYSQL_MANAGER* con, const char* host, const char* user, const char* passwd, unsigned int port)=0;
    virtual void mysql_manager_close(MYSQL_MANAGER* con)=0;
    virtual int mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len)=0;
    virtual int mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf, int res_buf_size)=0;
    virtual my_bool mysql_read_query_result(MYSQL *mysql)=0;
    virtual int mysql_stmt_fetch(MYSQL_STMT *stmt)=0;
    virtual int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,  unsigned int column, unsigned long offset)=0;
    virtual int mysql_stmt_store_result(MYSQL_STMT *stmt)=0;
    virtual unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt)=0;
    virtual my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, const void *attr)=0;
    virtual my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt, enum enum_stmt_attr_type attr_type, void *attr)=0;
    virtual my_bool mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd)=0;
    virtual my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd)=0;
    virtual my_bool mysql_stmt_close(MYSQL_STMT * stmt)=0;
    virtual my_bool mysql_stmt_reset(MYSQL_STMT * stmt)=0;
    virtual my_bool mysql_stmt_free_result(MYSQL_STMT *stmt)=0;
    virtual my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt,  unsigned int param_number, const char *data, unsigned long length)=0;
    virtual MYSQL_RES * mysql_stmt_result_metadata(MYSQL_STMT *stmt)=0;
    virtual MYSQL_RES * mysql_stmt_param_metadata(MYSQL_STMT *stmt)=0;
    virtual unsigned int mysql_stmt_errno(MYSQL_STMT * stmt)=0;
    virtual const char * mysql_stmt_error(MYSQL_STMT * stmt)=0;
    virtual const char * mysql_stmt_sqlstate(MYSQL_STMT * stmt)=0;
    virtual MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,  MYSQL_ROW_OFFSET offset)=0;
    virtual MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt)=0;
    virtual void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset)=0;
    virtual my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt)=0;
    virtual my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt)=0;
    virtual my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt)=0;
    virtual unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt)=0;
    virtual my_bool mysql_rollback(MYSQL * mysql)=0;
    virtual my_bool mysql_more_results(MYSQL *mysql)=0;
    virtual int mysql_next_result(MYSQL *mysql)=0;
    virtual int mysql_create_db(MYSQL *mysql, const char *DB)=0;
    virtual int mysql_drop_db(MYSQL *mysql, const char *DB)=0;

};

#endif /* DBSMYSQLABSTRACTCONNECTION_H_ */
