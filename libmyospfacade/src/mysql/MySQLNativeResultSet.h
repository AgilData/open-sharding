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

#ifndef MYSQLNATIVERESULTSET_H_
#define MYSQLNATIVERESULTSET_H_

#include <mysql.h>

#include <mysql/MySQLClient.h>
#include <mysql/MySQLAbstractResultSet.h>

using namespace mysql;

class MySQLNativeResultSet : public MySQLAbstractResultSet {
private:
    MySQLClient *mysqlClient;

public:
    MySQLNativeResultSet(MySQLClient *mysqlClient);
    virtual ~MySQLNativeResultSet();

    void mysql_free_result(MYSQL_RES *res);
    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
    unsigned long * mysql_fetch_lengths(MYSQL_RES *result); 
    MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result, MYSQL_FIELD_OFFSET offset);
    MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result);
    void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
    MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
    my_ulonglong mysql_num_rows(MYSQL_RES *res);
    unsigned int mysql_num_fields(MYSQL_RES *res);
    my_bool mysql_eof(MYSQL_RES *res);
    MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr);
    MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res);
    MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res);
    MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res);

};

#endif /* MYSQLNATIVERESULTSET_H_ */
