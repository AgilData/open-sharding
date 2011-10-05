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


#ifndef DBSMYSQLCONNMAP_H_
#define DBSMYSQLCONNMAP_H_

#include <unistd.h>
#include <map>
#include <boost/thread/mutex.hpp>
//#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <mysql.h>

#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLAbstractResultSet.h>
#include <mysql/MySQLErrorState.h>
#include <logger/Logger.h>

using namespace std;
using namespace logger;

struct ConnectInfo {
    string host, user, passwd;
    unsigned int port;
    const char *unix_socket;
    unsigned long clientflag;
};


class MySQLConnMap {
private:

    // logger
    static Logger _log;

    /* process ID where this map was created. This is used to detect unsafe use of connections between
      processes as can happen with FastCGI/Django if the application opens connections during the import
      phase */
    pid_t pid;

    /* Map of MYSQL* to connection information */
    map<MYSQL*, ConnectInfo*> mysqlToConnInfoMap;

    /* Map of MYSQL* to corresponding MySQLAbstractConnection* */
    map<MYSQL*, MySQLAbstractConnection*> mysqlToConnMap;

    /* Map of MYSQL_RES* to corresponding MySQLAbstractResultSet* */
    map<MYSQL_RES*, MySQLAbstractResultSet*> mysqlResMap;

    /* Map of MYSQL_RES* to corresponding MySQLAbstractConnection* */
    map<MYSQL_RES*, MySQLAbstractConnection*> mysqlResToConnMap; // THIS IS DEPRECATED

    map<MYSQL*, MySQLErrorState*> mysqlToErrorMap;

    // mutex for thread safety
//    boost::interprocess::interprocess_mutex mutex;
    boost::mutex mutex;

public:
    MySQLConnMap();
    ~MySQLConnMap();
    
    void setConnection(MYSQL *mysql, MySQLAbstractConnection *conn);
    MySQLAbstractConnection *getConnection(MYSQL *mysql);
    void erase(MYSQL *mysql);

    void setResultSet(MYSQL_RES*, MySQLAbstractResultSet *rs);
    MySQLAbstractResultSet* getResultSet(MYSQL_RES*);
    void erase(MYSQL_RES *);

    // DEPRECATED:
    void setConnection(MYSQL_RES*, MySQLAbstractConnection *conn);
    // DEPRECATED:
    MySQLAbstractConnection *getConnection(MYSQL_RES*);

    void setConnectInfo(MYSQL *mysql, ConnectInfo *connectInfo);
    ConnectInfo * getConnectInfo(MYSQL *mysql);

    void setErrorState(MYSQL *mysql, MySQLErrorState *errorState);
    MySQLErrorState *getErrorState(MYSQL *mysql);
    void clearErrorState(MYSQL *mysql);

    void eraseResults(MySQLAbstractConnection *conn);

};

#endif
