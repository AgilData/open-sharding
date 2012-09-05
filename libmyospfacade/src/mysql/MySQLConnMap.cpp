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

#include <unistd.h>
#include <map>
#include <mysql.h>
#include <mysql/MySQLConnMap.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLAbstractResultSet.h>
#include <logger/Logger.h>
#include <util/Util.h>

using namespace std;
using namespace util;
using namespace logger;

#define LOCK_MUTEX ;

Logger &MySQLConnMap::_log = Logger::getLogger("MySQLConnMap");

MySQLConnMap::MySQLConnMap() {
    pid = getpid();
    if (_log.isTraceEnabled()) {
        _log.trace(string(string("[") + Util::toString((void*)this)) + string("] NEW INSTANCE IN PROCESS ID ") + Util::toString((long)pid));
    }
}

MySQLConnMap::~MySQLConnMap() {
}

void MySQLConnMap::setConnection(MYSQL *mysql, MySQLAbstractConnection *conn) {

    if (_log.isTraceEnabled()) {
        _log.trace(string(string("[") + Util::toString((void*)this)) + string("] setConnection(MYSQL=") + Util::toString((void *)mysql) + string(", conn=") + Util::toString((void *)conn) + string(")"));
    }

    if (pid != getpid()) {
        _log.warn("Map was created in process " + Util::toString((long)pid)
            + " but call to setConnection(" + Util::toString((void*)mysql)
            + ") was made from process " + Util::toString((long)getpid())
        );
    }

    LOCK_MUTEX
    mysqlToConnMap[mysql] = conn;
}

/**
 * Convert MYSQL* to MySQLAbstractConnection*
 */
MySQLAbstractConnection *MySQLConnMap::getConnection(MYSQL *mysql) {
    LOCK_MUTEX

    MySQLAbstractConnection *ret = mysqlToConnMap[mysql];

    if (_log.isTraceEnabled()) {
        _log.trace(string(string("[") + Util::toString((void*)this)) + string("] getConnection(MYSQL=") + Util::toString((void *)mysql) + string(") returning ") + Util::toString((void *)ret));
    }

    if (pid != getpid()) {
        _log.warn("Map was created in process " + Util::toString((long)pid)
            + " but call to getConnection(" + Util::toString((void*)mysql)
            + ") was made from process " + Util::toString((long)getpid())
        );
    }

    return ret;
}

void MySQLConnMap::setResultSet(MYSQL_RES* res, MySQLAbstractResultSet *rs) {
    LOCK_MUTEX
    mysqlResMap[res] = rs;
}

MySQLAbstractResultSet* MySQLConnMap::getResultSet(MYSQL_RES* res) {
    LOCK_MUTEX
    return mysqlResMap[res];
}

// DEPRECATED:
void MySQLConnMap::setConnection(MYSQL_RES* res, MySQLAbstractConnection *conn) {
    LOCK_MUTEX
    mysqlResToConnMap[res] = conn;
}

// DEPRECATED:
MySQLAbstractConnection *MySQLConnMap::getConnection(MYSQL_RES *res) {
    LOCK_MUTEX
    return mysqlResToConnMap[res];
}


void MySQLConnMap::erase(MYSQL *mysql) {
    if (_log.isTraceEnabled()) {
        _log.trace(string("[") + Util::toString((void*)this) + string("] erase(MYSQL=") + Util::toString((void *)mysql) + string(")"));
    }

    if (pid != getpid()) {
        _log.warn("Map was created in process " + Util::toString((long)pid)
            + " but call to erase(" + Util::toString((void*)mysql)
            + ") was made from process " + Util::toString((long)getpid())
        );
    }

    LOCK_MUTEX

    mysqlToConnMap[mysql] = NULL;
    mysqlToErrorMap[mysql] = NULL;
    mysqlToConnInfoMap[mysql] = NULL;

    mysqlToConnMap.erase(mysql);
    mysqlToErrorMap.erase(mysql);
    mysqlToConnInfoMap.erase(mysql);


    // validation
}

void MySQLConnMap::erase(MYSQL_RES *res) {
    if (_log.isTraceEnabled()) {
        _log.trace(string("[") + Util::toString((void*)this)
            + string("] erase(MYSQL_RES=") + Util::toString((void *)res) + string(")"));
    }
    LOCK_MUTEX
    mysqlResToConnMap.erase(res);
}

void MySQLConnMap::eraseResults(MySQLAbstractConnection *conn) {
    map<MYSQL_RES*, MySQLAbstractConnection*>::iterator it;
    while (true) {
        bool found = false;
        for (it=mysqlResToConnMap.begin(); it!=mysqlResToConnMap.end(); it++) {
            if ((*it).second == conn) {
                mysqlResToConnMap.erase((*it).first);
                // erase() breaks the iterator wo we need to loop again
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
    }
}

void MySQLConnMap::setConnectInfo(MYSQL *mysql, ConnectInfo *connectInfo) {
    LOCK_MUTEX
    mysqlToConnInfoMap[mysql] = connectInfo;
}

ConnectInfo* MySQLConnMap::getConnectInfo(MYSQL *mysql) {
    LOCK_MUTEX
    return mysqlToConnInfoMap[mysql];
}

void MySQLConnMap::setErrorState(MYSQL *mysql, MySQLErrorState *errorState) {
    LOCK_MUTEX
    mysqlToErrorMap[mysql] = errorState;
}

MySQLErrorState *MySQLConnMap::getErrorState(MYSQL *mysql) {
    LOCK_MUTEX
    return mysqlToErrorMap[mysql];
}

void MySQLConnMap::clearErrorState(MYSQL *mysql) {
    LOCK_MUTEX
    mysqlToErrorMap.erase(mysql);
}


void MySQLConnMap::setOSPConn(string dbName, OSPConnection *ospConn) {
    LOCK_MUTEX
    ospConnMap[dbName] = ospConn;

}

OSPConnection* MySQLConnMap::getOSPConn(string dbName) {
    LOCK_MUTEX
    return ospConnMap[dbName];
}



