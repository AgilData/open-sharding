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

#include "MySQLNativeResultSet.h"

#include <mysql.h>

#include <mysql/MySQLClient.h>
#include <mysql/MySQLAbstractResultSet.h>

using namespace mysql;

MySQLNativeResultSet::MySQLNativeResultSet(MySQLClient *mysqlClient) {
    this->mysqlClient = mysqlClient;
}

MySQLNativeResultSet::~MySQLNativeResultSet() {
}

void MySQLNativeResultSet::mysql_free_result(MYSQL_RES *res) {
    mysqlClient->mysql_free_result(res);
}

MYSQL_ROW MySQLNativeResultSet::mysql_fetch_row(MYSQL_RES *res) {
    return mysqlClient->mysql_fetch_row(res);
}

unsigned long * MySQLNativeResultSet::mysql_fetch_lengths(MYSQL_RES *res) {
    return mysqlClient->mysql_fetch_lengths(res);
}

MYSQL_FIELD_OFFSET MySQLNativeResultSet::mysql_field_seek(MYSQL_RES *res, MYSQL_FIELD_OFFSET offset) {
    return mysqlClient->mysql_field_seek(res, offset);
}

MYSQL_FIELD * MySQLNativeResultSet::mysql_fetch_field(MYSQL_RES *res) {
    return mysqlClient->mysql_fetch_field(res);
}

void MySQLNativeResultSet::mysql_data_seek(MYSQL_RES *res, my_ulonglong offset) {
    return mysqlClient->mysql_data_seek(res, offset);
}

MYSQL_ROW_OFFSET MySQLNativeResultSet::mysql_row_seek(MYSQL_RES *res, MYSQL_ROW_OFFSET offset) {
    return mysqlClient->mysql_row_seek(res, offset);
}

my_ulonglong MySQLNativeResultSet::mysql_num_rows(MYSQL_RES *res) {
    return mysqlClient->mysql_num_rows(res);
}

unsigned int MySQLNativeResultSet::mysql_num_fields(MYSQL_RES *res) {
    return mysqlClient->mysql_num_fields(res);
}

my_bool MySQLNativeResultSet::mysql_eof(MYSQL_RES *res) {
    return mysqlClient->mysql_eof(res);
}

MYSQL_FIELD * MySQLNativeResultSet::mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr) {
    return mysqlClient->mysql_fetch_field_direct(res, fieldnr);
}

MYSQL_FIELD * MySQLNativeResultSet::mysql_fetch_fields(MYSQL_RES *res) {
    return mysqlClient->mysql_fetch_fields(res);
}

MYSQL_ROW_OFFSET MySQLNativeResultSet::mysql_row_tell(MYSQL_RES *res) {
    return mysqlClient->mysql_row_tell(res);
}

MYSQL_FIELD_OFFSET MySQLNativeResultSet::mysql_field_tell(MYSQL_RES *res) {
    return mysqlClient->mysql_field_tell(res);
}


