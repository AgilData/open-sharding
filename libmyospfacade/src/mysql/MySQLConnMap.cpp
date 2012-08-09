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
#include <util/MutexLock.h>
#include <mysql/MySQLConnectionInfo.h>

static pthread_mutex_t connmap_mutex = PTHREAD_MUTEX_INITIALIZER

#define LOCK_MUTEX MutexLock(&connmap_mutex);

using namespace std;
using namespace util;
using namespace logger;



Logger &MySQLConnMap::_log = Logger::getLogger("MySQLConnMap");

/*
bool MySQLConnMap::operator==(const ConnectInfo& A, const ConnectInfo& B)
{  

    bool value=false;
    if(A->osp_vendor == B->osp_vendor)
        if(A->virtual_host == B->virtual_host)
            if(A->host == B->host)
                if(A->osp_domain == B->osp_domain)
                    if(A->target_dbms == B->target_dbms)
                        if(A->target_schema_name == B->target_schema_name)
                            if(A->user == B->user)
                                if(A->passwd == B->passwd)
                                    if(A->port == B->port)
                                        if(A->unix_socket == B->unix_socket)
                                            if(A->clientflag == B->clientflag)
                                                if(A->protocol == B->protocol)
                                                    value=true;
    return value;

}
*/

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
    if (mysqlResMap.count(res)==0) {
        return NULL;
    }
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
    if (mysqlResToConnMap.count(res)==0) {
        return NULL;
    }
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

void MySQLConnMap::setConnectInfo(MYSQL *mysql, MySQLConnectionInfo *connectInfo) {
    LOCK_MUTEX
    mysqlToConnInfoMap[mysql] = connectInfo;
}

MySQLConnectionInfo* MySQLConnMap::getConnectInfo(MYSQL *mysql) {
    LOCK_MUTEX
    if (mysqlToConnInfoMap.count(mysql)==0) {
        return NULL;
    }
    return mysqlToConnInfoMap[mysql];
}

void MySQLConnMap::setErrorState(MYSQL *mysql, MySQLErrorState *errorState) {
    LOCK_MUTEX
    mysqlToErrorMap[mysql] = errorState;
}

MySQLErrorState *MySQLConnMap::getErrorState(MYSQL *mysql) {
    LOCK_MUTEX
    if (mysqlToErrorMap.count(mysql)==0) {
        return NULL;
    }
    return mysqlToErrorMap[mysql];
}

void MySQLConnMap::clearErrorState(MYSQL *mysql) {
    LOCK_MUTEX
    mysqlToErrorMap.erase(mysql);
}

