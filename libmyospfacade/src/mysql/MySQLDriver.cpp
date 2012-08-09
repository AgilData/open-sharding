

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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <map>

// if we want to support all mysql symbols we need to *at least* import these headers
//#include <my_global.h>
//#include <m_ctype.h>

#include <mysql.h>
#include <errmsg.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>

#include <mysql/BuildInfo.h>
#include <mysql/MySQLClient.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLOSPConnection.h>
#include <mysql/MySQLNativeConnection.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>
#include <opensharding/OSPUnixSocketConnection.h>
#include <opensharding/OSPWireRequest.h>
#include <opensharding/OSPConnectRequest.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPErrorResponse.h>
#include <logger/Logger.h>
#include <myutil/MyOSPLogger.h>
#include <myutil/MyOSPConfig.h>
#include <util/Util.h>
#include <util/MutexLock.h>

#include <mysql/MySQLDriver.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLConnMap.h>
#include <mysql/MySQLErrorState.h>

using namespace mysql;
using namespace logger;
using namespace std;
using namespace util;
using namespace opensharding;

/* CONSTANTS */

static int PROTOCOL_TCP = 1;
static int PROTOCOL_PIPES = 2;
static int PROTOCOL_UNIX_SOCKET = 3;

/* GLOBAL VARIABLES */

static Logger &xlog = MyOSPLogger::getLogger("MySQLDriver");
static unsigned int Pid = 0;

static bool bannerDisplayed = false;

/* map for mysql structure that we created in mysql_init so we can delete them in mysql_close */
static map<MYSQL*, bool> *mysqlAllocMap = new map<MYSQL*, bool>();


#define LOCK_MUTEX MutexLock(&connmap_mutex);

/* Mapping of MYSQL structues to wrapper structures */
static MySQLConnMap *_mysqlResourceMap = NULL;

/* Temporary global variable for next named pipe number for this process */
static int nextPipeNo = 1;

static pthread_mutex_t MySQLDriver_init_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t MySQLDriver_resource_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Wrapper around libmysqlclient.so */
static MySQLClient *mysqlclient = NULL;


const char *client_errors[]=
{
  "Unknown MySQL error",
  "Can't create UNIX socket (%d)",
  "Can't connect to local MySQL server through socket '%-.100s' (%d)",
  "Can't connect to MySQL server on '%-.100s' (%d)",
  "Can't create TCP/IP socket (%d)",
  "Unknown MySQL server host '%-.100s' (%d)",
  "MySQL server has gone away",
  "Protocol mismatch; server version = %d, client version = %d",
  "MySQL client ran out of memory",
  "Wrong host info",
  "Localhost via UNIX socket",
  "%-.100s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync; you can't run this command now",
  "Named pipe: %-.32s",
  "Can't wait for named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't open named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't set state of named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't initialize character set %-.32s (path: %-.100s)",
  "Got packet bigger than 'max_allowed_packet' bytes",
  "Embedded server",
  "Error on SHOW SLAVE STATUS:",
  "Error on SHOW SLAVE HOSTS:",
  "Error connecting to slave:",
  "Error connecting to master:",
  "SSL connection error",
  "Malformed packet",
  "This client library is licensed only for use with MySQL servers having '%s' license",
  "Invalid use of null pointer",
  "Statement not prepared",
  "No data supplied for parameters in prepared statement",
  "Data truncated",
  "No parameters exist in the statement",
  "Invalid parameter number",
  "Can't send long data for non-string/non-binary data types (parameter: %d)",
  "Using unsupported buffer type: %d  (parameter: %d)",
  "Shared memory: %-.100s",
  "Can't open shared memory; client could not create request event (%lu)",
  "Can't open shared memory; no answer event received from server (%lu)",
  "Can't open shared memory; server could not allocate file mapping (%lu)",
  "Can't open shared memory; server could not get pointer to file mapping (%lu)",
  "Can't open shared memory; client could not allocate file mapping (%lu)",
  "Can't open shared memory; client could not get pointer to file mapping (%lu)",
  "Can't open shared memory; client could not create %s event (%lu)",
  "Can't open shared memory; no answer from server (%lu)",
  "Can't open shared memory; cannot send request event to server (%lu)",
  "Wrong or unknown protocol",
  "Invalid connection handle",
  "Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)",
  "Row retrieval was canceled by mysql_stmt_close() call",
  "Attempt to read column without prior row fetch",
  "Prepared statement contains no metadata",
  "Attempt to read a row while there is no result set associated with the statement",
  "This feature is not implemented yet",
  "Lost connection to MySQL server at '%s', system error: %d",
  "Statement closed indirectly because of a preceeding %s() call",
  "The number of columns in the result set differs from the number of bound buffers. You must reset the statement, rebind the result set columns, and execute the statement again",
  ""
};

/* GLOBAL METHODS */

MySQLConnMap* getResourceMap() {
    MutexLock lock(&MySQLDriver_resource_mutex);
    if (_mysqlResourceMap==NULL) {
        _mysqlResourceMap = new MySQLConnMap();
    }
    return _mysqlResourceMap;
}

MySQLClient* getMySQLClient() {
    MutexLock lock(&MySQLDriver_init_mutex);
    if (!mysqlclient) {
        mysqlclient = new MySQLClient();
        if (!mysqlclient->init()) { xlog.error("Failed to init mysqlClient"); }
    }
    return mysqlclient;
}

int getNextNamedPipeID() {
    MutexLock lock(&MySQLDriver_init_mutex);
    return nextPipeNo++;
}

// this method called from mysql_server_init and shows banner
void banner() {
    if (!bannerDisplayed) {
        Logger::configure("/etc/myosp-log.properties");
        if (xlog.isDebugEnabled()) {
            xlog.debug(string("Open Sharding MySQL Driver") +
                       string(" (libmyosp) version ") +
                       WRAPPER_VERSION + " (" + WRAPPER_BUILD_TSTAMP ")");
        }
        bannerDisplayed = true;
    }
}

void trace(const char *name) {
    if (xlog.isTraceEnabled()) {
        xlog.trace(string("In method '") + name + "'");
    }
}

void trace(const char *name, MYSQL *mysql) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL=" + Util::toString(mysql) + ")");
    }
}

void trace(const char *name, MYSQL_RES *res) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL_RES=" + Util::toString(res) + ")");
    }
}

void trace(const char *name, MYSQL_STMT *stmt) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL_STMT=" + Util::toString(stmt) + ")");
    }
}

string getLogPrefix(MYSQL *mysql) {
    string ret = "[mysql=" + Util::toString(mysql) + "] ";
    return ret;
}

#define LOGVERSION "1.1"
#define DRIVERNAME "MyOSP"
#define DBMS "MySQL"
void log_entry_for_analyser(string domain, void *connId,
              int stmtId, string methodSignature, string* params,
              int nParams, string returnVal,
                            struct timeval *tstart, struct timeval *tend) {
  // [LOGSTART][logVersion][driverName][domain][dbms][pid][threadId][connId][stmtId][start][dur][methodSignature(){params}{returnVal}]\n[LOGEND]
    struct timeval tdiff;
    timersub(tend, tstart, &tdiff);
    char sdiff[64];
    sprintf(sdiff, "%ld", tdiff.tv_sec*1000 + tdiff.tv_usec/1000);
    long long ts = (tstart->tv_sec * 1000) + (tstart->tv_usec/1000);
    if(!Pid) Pid=getpid(); //Pid is a global unsigned int.
  unsigned int ThreadId = (unsigned int) pthread_self();
    stringstream ss;
    ss   << "[LOGSTART]"
       << "[" << LOGVERSION      << "]"
       << "[" << DRIVERNAME      << "]"
       << "[" << domain        << "]"  //TODO domain will be an empty string for now.
       << "[" << DBMS          << "]"
         << "[" << Pid                   << "]"
         << "[" << ThreadId        << "]"
         << "[" << Util::toString(connId) << "]"
         << "[" << stmtId                << "]"
         << "[" << ts                    << "]"
         << "[" << sdiff                 << "]"
     << "[" << methodSignature
     << Util::buildParamList(params, nParams, SQUIGGLY_BRACKETS, false)
     << "{" << returnVal             << "}]" << endl
         << "[LOGEND]"                 << endl;

    // output to the log
    try {//TODO:Change Logging
      if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_SYSLOG) {
      syslog(LOG_DEBUG, "%s", ss.str().c_str());
      }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_STDERR) {
      cerr << ss.str();
        }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_STDOUT) {
      cout << ss.str();
        }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_FILE) {
      fprintf(MyOSPConfig::getAnalyzeLogFile(), "%s", ss.str().c_str());
      fflush(MyOSPConfig::getAnalyzeLogFile());
        }
    }
    catch (...) {
      cerr << "Unable to write to log file in " << OSPConfig::getConfigMap()["shard.analyze.log.dir"] << endl;
    }
}

void cleanup() {
    //TODO: close resources? do we care? this gets called just before the process exist
}

int setErrorState(MYSQL *mysql, int _errno, const char *_error,
                  const char *_sqlstate) {

    xlog.error(
        string("Setting error state to ")
        + Util::toString(_errno)
        + string("; ")
        + string(_error)
    );

    getResourceMap()->setErrorState(mysql, new MySQLErrorState(_errno,
                                   _error, _sqlstate));

    return -1;
}

MySQLAbstractConnection *getConnection(MYSQL *mysql, bool createIfNotExist) {
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(mysql);
    
    /*
    if (xlog.isTraceEnabled()) {
        //xlog.trace(
            string("getConnection(MYSQL=")
            + Util::toString((void*)mysql)
            + string(") returning ")
            + string(conn?"a connection object":"NULL")
        );
    }
    */

    return conn;
}

#define valid(x) (x ? true : false)

/*
extern "C" CHARSET_INFO *default_charset_info = NULL; //TODO: init this

extern "C" my_bool my_init() {
    //TODO: delegate to real driver
    return true;
}

extern "C" CHARSET_INFO *get_charset_by_csname(const char *cs_name, uint cs_flags, int my_flags) {
    //TODO: delegate to real driver
    return NULL;
}

extern "C" void *my_malloc(size_t Size, int MyFlags) {
    //TODO: delegate to real driver
    return NULL;
}

extern "C" void my_no_flags_free(void *ptr) {
    //TODO: delegate to real driver
}
*/

MYSQL *mysql_init(MYSQL *mysql) {
    //trace("mysql_init", mysql);

    // show product banner and configure logging
    banner();

    // the API docs say we have to allocate the pointer if it does not already exist
    if (mysql == NULL) {
        mysql = new MYSQL();
        // keep track of this because we have to delete it later
        (*mysqlAllocMap)[mysql] = true;
    }

    // call mysql_init on real driver
    mysql = getMySQLClient()->mysql_init(mysql);

    return mysql;
}

unsigned int mysql_errno(MYSQL *mysql) {
    //trace("mysql_errno", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {
        //xlog.trace(string("mysql_errno() returning local error state ") + Util::toString((int)errorState->my_errno));
        unsigned int ret =  errorState->my_errno;

        if (xlog.isDebugEnabled() || ret>0) {
            xlog.debug(string("mysql_errno returning ") + Util::toString((int)ret));
        }

        // HACK to avoid Python "error totally whack" error messages
        if (ret>CR_MAX_ERROR) {
            return CR_UNKNOWN_ERROR; // 2000
        }

        return ret;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        unsigned int ret = conn->mysql_errno(mysql);

        if (xlog.isDebugEnabled() || ret>0) {
            xlog.debug(string("mysql_errno returning ") + Util::toString((int)ret));
        }

        // HACK to avoid Python "error totally whack" error messages
        if (ret>CR_MAX_ERROR) {
            return CR_UNKNOWN_ERROR; // 2000
        }

        return ret;
    } else {
        // no error
        if (xlog.isDebugEnabled()) {
        }
        //HACK: but this could problems with customers
//            xlog.debug("mysql_errno() called but there is no error and no connection! Simulating 2006 / MySQL server has gone away");
//        return CR_SERVER_GONE_ERROR; // 2006
            xlog.debug("mysql_errno() called but there is no error and no connection! Faking success and returning 0");
        return 0; // 2006
    }
}

const char *mysql_error(MYSQL *mysql) {
    //trace("mysql_error", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_error returning cached errorState"));
        }

        return errorState->my_error;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        const char *ret = conn->mysql_error(mysql);
        if (ret == NULL) {
            ret = "";
        }

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_error returning ") + string(ret));
        }

        return ret;
    } else {
        // no error
//        if (xlog.isDebugEnabled()) {
//            xlog.debug("mysql_error() called but there is no error and no connection! Simulating 2006 / MySQL server has gone away");
//        }
        //HACK:
//        return "MySQL server has gone away";
            xlog.debug("mysql_error() called but there is no error and no connection! Faking success and returning empty string");
        return "";
    }
}

const char * mysql_sqlstate(MYSQL *mysql) {
    //trace("mysql_sqlstate", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {
        return errorState->my_sqlstate;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        return "TBD";
    }
    return conn->mysql_sqlstate(mysql);
}

/******************************************************************/
/*************Added function do_osp_connect****************/
/******************************************************************/

int do_osp_connect(MYSQL *mysql, const char *db, MySQLConnectionInfo *info, MySQLAbstractConnection *conn)
{
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("do_osp_connect(\"") + Util::toString(mysql) + string(",") + string(db) + string("\")"));
    }


    try {

        if (db!=NULL) {
            if (strlen(db) == 0) {
                setErrorState(mysql, CR_UNKNOWN_ERROR, "ERROR: database name is blank", "OSP05");
                return -1;
            }
        }

        //Check for osp: in database name for attempts to use deprecated functionality.
        if (strncmp(db, "osp:", 4)==0) {
            //setErrorState writes message to xlog.error()
            setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB, use of 'osp:dbname' in database string of the myosp driver is deprecated. [4]", "OSP01");
            return -1;
        }


        if (info == NULL) {
            xlog.error("No ConnInfo in map");
            setErrorState(mysql, CR_UNKNOWN_ERROR, "No ConnInfo in map", "OSP01");
            return -1;
        }
    
        if (xlog.isDebugEnabled()) {
             xlog.debug("Creating OSP connection");
        }

        if (xlog.isDebugEnabled()) xlog.debug("Getting next named pipe ID");
        int pipeNo = getNextNamedPipeID();
        if (xlog.isDebugEnabled()) xlog.debug(string("Got next named pipe ID: ") + Util::toString(pipeNo));

        if (xlog.isDebugEnabled()) xlog.debug("Creating OSPNamedPipeConnection");

        // create a dedicated named pipe connection
        OSPConnection* ospNetworkConnection;
        if (info->getProtocol() == PROTOCOL_TCP) {
             ospNetworkConnection = new OSPTCPConnection(info->getHost(), info->getPort());
        }
        else if (info->getProtocol() == PROTOCOL_PIPES) {
             ospNetworkConnection = new OSPNamedPipeConnection(info, pipeNo);
        }
        else if (info->getProtocol() == PROTOCOL_UNIX_SOCKET) {
             ospNetworkConnection = new OSPUnixSocketConnection(info);
        }
        else {
            throw Util::createException("UNSUPPORTED PROTOCOL");
        }

        if (xlog.isDebugEnabled()) xlog.debug("Created OSPNamedPipeConnection OK");

        // create MySQL OSP connection object
        try {
            /*CHANGED*/
            if (xlog.isDebugEnabled()) xlog.debug("Creating MySQLOSPConnection OK");
            conn = new MySQLOSPConnection(mysql, info->host, info->port, db, info->user, info->passwd, getResourceMap(), ospNetworkConnection);
            if (xlog.isDebugEnabled()) xlog.debug("Created MySQLOSPConnection OK");
        }
        catch (...) {
            setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error", "OSP01");
            return -1;
        }

        // store mapping from the MYSQL structure to the OSP connection
        getResourceMap()->setConnection(mysql, conn);

        // success
        getResourceMap()->clearErrorState(mysql);

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("do_osp_connect(\"") + Util::toString(mysql) + string(",") + string(db) + string("\") SUCCESS"));
        }


    } catch (const char *exception) {
        xlog.error(string("do_osp_connect() failed due to exception: ") + exception);
        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
        return -1;
    } catch (...) {
        xlog.error(string("do_osp_connect(") + string(db==NULL?"NULL":db) + string(") failed due to exception"));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
        return -1;
    }

    return 0;

}

/********************************************************************************/
/****This is only a temporary fix, the new 2.6 MyOSP will not have this issue.***/
/********************************************************************************/
MYSQL *mysql_real_connect(MYSQL *mysql, const char *_host, const char *_user,
    const char *_passwd, const char *db, unsigned int port,
    const char *unix_socket, unsigned long clientflag) {
    //trace("mysql_real_connect", mysql);
    try {

        if (db!=NULL) {
            //Check for osp: in database name for attempts to use deprecated functionality.
            if (strncmp(db, "osp:", 4)==0) {
                //setErrorState writes message to xlog.error()
                setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB, use of 'osp:dbname' in database string of the myosp driver is deprecated. [4]", "OSP01");
                return NULL;
            }
        }
        
        if (!getMySQLClient()->init()) {
          setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to load MySQL driver", "OSP01");
          xlog.error("failed to init mysqlClient");
          return NULL;
        }

        MySQLConnectionInfo *info = new MySQLConnectionInfo();

        // these fields are populated from arguments to this function regardless of URL
        info->virtual_host=(_host==NULL ? string("") : string(_host));
        info->user = _user==NULL ? string("") : string(_user);
        info->passwd = _passwd==NULL ? string("") : string(_passwd);
        info->unix_socket = unix_socket;
        info->clientflag = clientflag;

        bool ospMode = MyOSPConfig::isOspHost(info->virtual_host);
        
        string databaseName = (db ? string(db) : string(""));
        
        if(ospMode) {
           
            string host_url;
           
            vector<string> conn_info;
            
            //ignoring all but actual_host, port, and schema for now.
            try {

                xlog.debug(string("mysql_real_connect() host = ") + string(_host==NULL ? "NULL" : _host));
                host_url = MyOSPConfig::getHostUrl(_host);

                xlog.debug(string("MyOSPConfig::getHostUrl() returned ") + host_url);
                conn_info = MyOSPConfig::parseVirtualHostUrl(host_url);

                xlog.debug("MyOSPConfig::parseVirtualHostUrl() returned:");
                vector<string>::iterator it;
                for ( it=conn_info.begin() ; it < conn_info.end(); it++ ) {
                    xlog.debug(string("\t") + (*it));
                }

            }
            catch (char* e) {
                setErrorState(mysql, CR_UNKNOWN_ERROR, e, "OSP01");
                return mysql;
            }

            int urlIndex = 0;

            info->osp_vendor = conn_info[urlIndex++];
            string protocolName = conn_info[urlIndex++];

            unsigned int protocol;
            if (protocolName == "pipes") {
                info->protocol = PROTOCOL_PIPES;
            }
            else if (protocolName == "tcp") {
                info->protocol = PROTOCOL_TCP;
            }
            else if (protocolName == "unix") {
                info->protocol = PROTOCOL_UNIX_SOCKET;
            }
            else {
                xlog.error("Invalid protocol");
                throw "Invalid protocol";
            }

            if (info->protocol == PROTOCOL_UNIX_SOCKET) {

                //TODO:
                //info->socket = conn_info[urlIndex++];

            }
            else {

                info->host = conn_info[urlIndex++];
                string portString = conn_info[urlIndex++];

                unsigned int real_port;
                if (portString == "0") {
                    xlog.debug("Port was 0, defaulting to 4545");
                    real_port = 4545;
                }
                else {
                    stringstream ss(portString);
                    ss >> real_port;
                }
            }

            info->osp_domain         = conn_info[urlIndex++];
            info->target_dbms        = conn_info[urlIndex++];
            info->target_schema_name = conn_info[urlIndex++];

            if (databaseName != "") {
                // if a database name was supplied as an argument to this function then this takes precedence
                info->target_schema_name = databaseName;
            }

            if (xlog.isDebugEnabled()) {
                xlog.debug(string("mysql_real_connect(")
                + Util::toString(mysql) + string(", ")
                + string("virtual-host=") + info->virtual_host + string(", ")
                + string("real-host=") + info->host + string(", ")
                + string("port=") + Util::toString(info->port) + string(", ")
                + string("user=") + info->user + string(", ")
                + string("osp_vendor=") + info->osp_vendor + string(",")
                + string("protocol=") + Util::toString(protocol) + string(",")
                + string("target_dbms=") + info->target_dbms + string(",")
                + string("db=") + (databaseName=="" ? "NULL" : databaseName.c_str()) 
                + string(")")
                ); 
            }

            MySQLConnectionInfo *old_info = getResourceMap()->getConnectInfo(mysql);
           
            if (old_info) {
                delete old_info;
            }

            MySQLAbstractConnection *conn = getConnection(mysql, false);

            if (conn) {

                // In real connect we always close.
                conn->mysql_close(mysql);

                // erase all resources for this mysql handle
                getResourceMap()->erase(mysql);

                // delete the connection
                delete conn;

                // reset reference
                conn = NULL;

            }

            // store the connection info in the map again
            getResourceMap()->setConnectInfo(mysql, info);
           

            if(MyOSPConfig::isShardAnalyze()) {
                struct timeval tstart; gettimeofday(&tstart, NULL);
                    if (-1 == do_osp_connect(mysql, databaseName.c_str(), info, conn)) {
                        setErrorState(mysql, 9001, "Failed to connect to DB [1]", "OSP01");
                        return NULL;

                }
                struct timeval tend;   gettimeofday(&tend, NULL);
                string * params = new string[8];
                params[0] = Util::toString(mysql);
                params[1] = (_host ? _host : "NULL");
                params[2] = (_user ? _user : "NULL");
                params[3] = "hidden"; //Don't log the password value
                params[4] = (db ? db : "NULL");
                params[5] = Util::toString(port);
                params[6] = (unix_socket ? unix_socket : "NULL");
                params[7] = Util::toString(clientflag);
                log_entry_for_analyser("", (void *) mysql, 0,
                        "mysql_real_connect(MYSQL *mysql, const char *_host, const char *_user, "
                        "const char *_passwd, const char *db, unsigned int port, const char *unix_socket, "
                        "unsigned long clientflag)", params, 8,"", &tstart, &tend);
                delete [] params;

            }   
            else {
                  if (-1 == do_osp_connect(mysql, databaseName.c_str(), info, conn)) {
                      setErrorState(mysql, 9001, "Failed to connect to DB [1]", "OSP01");
                      xlog.error("Call to do_osp_connect FAILED");
                      return NULL;
                  }
                  else {
                      xlog.debug("Call to do_osp_connect SUCCESS");
                  }
            }
        }
        else { //This is also known as delegate mode...

            info->host = _host==NULL ? string("") : string(_host);
            info->user = _user==NULL ? string("") : string(_user);
            info->passwd = _passwd==NULL ? string("") : string(_passwd);
            info->port = port;
            info->unix_socket = unix_socket;
            info->clientflag = clientflag;

            if (xlog.isDebugEnabled()) {
                xlog.debug(string("mysql_real_connect(")
                + Util::toString(mysql) + string(", ")
                + string("virtual-host=") + info->virtual_host + string(", ")
                + string("real-host=") + info->host + string(", ")
                + string("port=") + Util::toString(info->port) + string(", ")
                + string("user=") + info->user + string(", ")
                + string("db=") + (databaseName=="" ? "NULL" : databaseName.c_str()) 
                + string(")")
                ); 
            }

            MySQLConnectionInfo *old_info = getResourceMap()->getConnectInfo(mysql);
            
            if (old_info) {
                delete old_info;
            }

            // store connection info so it can be retrieved in mysql_select_db in separate call
            getResourceMap()->setConnectInfo(mysql, info);

            if(MyOSPConfig::isShardAnalyze()) {
                struct timeval tstart; gettimeofday(&tstart, NULL);
                
                if (db != NULL) {

                    if (xlog.isDebugEnabled()) {
                        xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\")"));
                    }

                    MySQLAbstractConnection *conn = NULL;

                    try {

                        // get the connection info that should have been stored in the previous call to mysql_real_connect()
                        info = getResourceMap()->getConnectInfo(mysql);

                        if (info == NULL) {
                            xlog.error("No ConnInfo in map");
                            setErrorState(mysql, CR_UNKNOWN_ERROR, "No ConnInfo in map", "OSP01");
                            return NULL;
                        }

                        if (mysql->db!=NULL) {
                            if (strlen(mysql->db) == 0) {
                                setErrorState(mysql, CR_UNKNOWN_ERROR, "ERROR: database name is blank", "OSP05");
                            return NULL;
                            }
                            //Check for osp: in database name for attempts to use deprecated functionality.
                            if (strncmp(mysql->db, "osp:", 4)==0) {
                                //setErrorState writes message to xlog.error()
                                setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB, use of 'osp:dbname' in database string of the myosp driver is deprecated. [4]", "OSP01");
                                return NULL;
                            }
                        }

                        if (xlog.isDebugEnabled()) {
                            xlog.debug("Creating native connection");
                        }

                        // create native connection
                        conn = new MySQLNativeConnection(mysql, getMySQLClient(), getResourceMap());

                        // store mapping from the MYSQL structure to the native connection
                        getResourceMap()->setConnection(mysql, conn);

                        if (!conn->connect(
                                info->host.c_str(),
                                info->user.c_str(),
                                info->passwd.c_str(),
                                db,
                                info->port==0 ? 3306 : info->port,
                                info->unix_socket,
                                info->clientflag
                            )) {
                            setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [1]", "OSP01");
                            return NULL;
                        }
                           // success
                        getResourceMap()->clearErrorState(mysql);

                        if (xlog.isDebugEnabled()) {
                            xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\") SUCCESS"));
                        }


                    } catch (const char *exception) {
                        xlog.error(string("mysql_real_connect() failed due to exception: ") + exception);
                        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
                        return NULL;
                    } catch (...) {
                        xlog.error(string("mysql_real_connect()(") + string(db==NULL?"NULL":db) + string(") failed due to exception"));
                        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
                        return NULL;
                    }
                }
                struct timeval tend;   gettimeofday(&tend, NULL);
                string * params = new string[8];
                params[0] = Util::toString(mysql);
                params[1] = (_host ? _host : "NULL");
                params[2] = (_user ? _user : "NULL");
                params[3] = "hidden"; //Don't log the password value
                params[4] = (db ? db : "NULL");
                params[5] = Util::toString(port);
                params[6] = (unix_socket ? unix_socket : "NULL");
                params[7] = Util::toString(clientflag);
                log_entry_for_analyser("", (void *) mysql, 0,
                        "mysql_real_connect(MYSQL *mysql, const char *_host, const char *_user, "
                        "const char *_passwd, const char *db, unsigned int port, const char *unix_socket, "
                        "unsigned long clientflag)", params, 8, "", &tstart, &tend);
                delete [] params;
            }
            else {

                if (db && string(db) != "") {

                    if (xlog.isDebugEnabled()) {
                        xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\")"));
                    }

                    MySQLAbstractConnection *conn = NULL;

                    try {

                        // get the connection info that should have been stored in the previous call to mysql_real_connect()
                        MySQLConnectionInfo *info = getResourceMap()->getConnectInfo(mysql);

                        if (info == NULL) {
                            xlog.error("No ConnInfo in map");
                            setErrorState(mysql, CR_UNKNOWN_ERROR, "No ConnInfo in map", "OSP01");
                            return NULL;
                        }

                        if (mysql->db!=NULL) {
                            
                            if (strlen(mysql->db) == 0) {
                                setErrorState(mysql, CR_UNKNOWN_ERROR, "ERROR: database name is blank", "OSP05");
                                return NULL;
                            }

                            //Check for osp: in database name for attempts to use deprecated functionality.
                            if (strncmp(mysql->db, "osp:", 4)==0) {
                                //setErrorState writes message to xlog.error()
                                setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB, use of 'osp:dbname' in database string of the myosp driver is deprecated. [4]", "OSP01");
                                return NULL;
                            }
                        }

                        if (xlog.isDebugEnabled()) {
                            xlog.debug("Creating native connection");
                        }

                        // create native connection
                        conn = new MySQLNativeConnection(mysql, getMySQLClient(), getResourceMap());

                        // store mapping from the MYSQL structure to the native connection
                        getResourceMap()->setConnection(mysql, conn);

                        if (!conn->connect(
                                info->host.c_str(),
                                info->user.c_str(),
                                info->passwd.c_str(),
                                db,
                                info->port==0 ? 3306 : info->port,
                                info->unix_socket,
                                info->clientflag
                            )) {
                            setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [1]", "OSP01");
                            return NULL;
                        }
                
                        // success
                        getResourceMap()->clearErrorState(mysql);

                        if (xlog.isDebugEnabled()) {
                            xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\") SUCCESS"));
                        }


                    } catch (const char *exception) {
                        xlog.error(string("mysql_real_connect() failed due to exception: ") + exception);
                        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
                        return NULL;
                    } catch (...) {
                        xlog.error(string("mysql_real_connect()(") + string(db==NULL?"NULL":db) + string(") failed due to exception"));
                        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
                        return NULL;
                    }
        
                }
            }
        }

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_real_connect() SUCCESS Returning ") + Util::toString((void*)mysql));
        }

        return mysql;

    } catch (const char *ex1) {
        xlog.error(string("mysql_real_connect() failed: ") + string(ex1));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB [2]", "OSP01");
        return NULL;
    } catch (...) {
        xlog.error(string("mysql_real_connect() failed due to exception"));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB [3]", "OSP01");
        return NULL;
    }
}

/**************************************/
/*****New Method for Mysql_Select_DB***/
/**************************************/
int mysql_select_db(MYSQL *mysql, const char *db) {
    int result;

    if (db==NULL) {
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Passed NULL database name", "OSP01");
        xlog.error("failed to init mysqlClient");
        return -1;
    }
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\")"));
    }

    // get the connection info that should have been stored in the previous call to mysql_real_connect()
    MySQLConnectionInfo *info = getResourceMap()->getConnectInfo(mysql);

    if (info == NULL) {
        xlog.error("No ConnInfo in map");
        setErrorState(mysql, CR_UNKNOWN_ERROR, "No ConnInfo in map", "OSP01");
        return -1;
    }
    
    MySQLAbstractConnection *conn = getConnection(mysql, false);

    if (!conn)
    {
      xlog.error("No connection from getConnection");
      setErrorState(mysql, CR_UNKNOWN_ERROR, "No Connection", "OSP01");
      return -1;
    }

    bool ospMode = MyOSPConfig::isOspHost(info->virtual_host);

    if (ospMode){

        //Done for strcmp
        const char * schema_name = info->target_schema_name.c_str();

        if (strcmp(schema_name, db)==0) {
            if (xlog.isDebugEnabled()) {
                    xlog.debug("mysql_select_db() re-using existing connection");
                }
            // success
            return 0;
        }
        else
        {
            
            if(MyOSPConfig::isShardAnalyze()) {
                struct timeval tstart; gettimeofday(&tstart, NULL);
            
                xlog.info(
                    string("mysql_select_db() switching from ")
                    + (mysql->db ? string(mysql->db) : string("NULL"))
                    + string(" to ")
                    + (db ? string(db) : string("NULL"))
                );

                // a different db is being selected so we need to close the old connection now
                conn->mysql_close(mysql);

                // erase all resources for this mysql handle
                getResourceMap()->erase(mysql);

                // delete the connection
                delete conn;

                // reset reference
                conn = NULL;

                // store the connection info in the map again
                getResourceMap()->setConnectInfo(mysql, info);
            

                result = do_osp_connect(mysql,db, info, conn);

                struct timeval tend; gettimeofday(&tend, NULL);
                string * params = new string[2];
                params[0] = Util::toString(mysql);
                params[1] = (db ? db : "NULL");
                log_entry_for_analyser("", (void *) mysql, 0,
                        "mysql_select_db(MYSQL *mysql, const char *db)",
                        params, 2, "", &tstart, &tend);
                delete [] params;
                return result;
            }
            else {

                  xlog.info(
                      string("mysql_select_db() switching from ")
                      + (mysql->db ? string(mysql->db) : string("NULL"))
                      + string(" to ")
                      + (db ? string(db) : string("NULL"))
                  );

                  // a different db is being selected so we need to close the old connection now
                  conn->mysql_close(mysql);

                  // erase all resources for this mysql handle
                  getResourceMap()->erase(mysql);

                  // delete the connection
                  delete conn;

                  // reset reference
                  conn = NULL;

                  // store the connection info in the map again
                  getResourceMap()->setConnectInfo(mysql, info);
                  

                result = do_osp_connect(mysql,db, info, conn);

                return result;
            }
        }
    }
    else { //This is also delegate mode
        if(MyOSPConfig::isShardAnalyze()) {
            struct timeval tstart; gettimeofday(&tstart, NULL);
            //ADD NEW CODE
            try
            {
              if (xlog.isDebugEnabled()) {
                  xlog.debug("Creating native connection");
              }

              conn->mysql_select_db(mysql,db);

              // store mapping from the MYSQL structure to the native connection
              getResourceMap()->setConnection(mysql, conn);

              if (!conn->connect(
                      info->host.c_str(),
                      info->user.c_str(),
                      info->passwd.c_str(),
                      db,
                      info->port==0 ? 3306 : info->port,
                      info->unix_socket,
                      info->clientflag
              )) {
                  setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [1]", "OSP01");
                  result = -1;
              }
               // success
              getResourceMap()->clearErrorState(mysql);

              if (xlog.isDebugEnabled()) {
                  xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\") SUCCESS"));
              }

              result = 0;

            } catch (const char *exception) {
              xlog.error(string("mysql_select_db() failed due to exception: ") + exception);
              setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
              result = -1;
            } catch (...) {
              xlog.error(string("mysql_select_db(") + string(db==NULL?"NULL":db) + string(") failed due to exception"));
              setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
              result = -1;
            }

            //END NEW CODE
            struct timeval tend; gettimeofday(&tend, NULL);
            string * params = new string[2];
            params[0] = Util::toString(mysql);
            params[1] = (db ? db : "NULL");
            log_entry_for_analyser("", (void *) mysql, 0,
                    "mysql_select_db(MYSQL *mysql, const char *db)",
                    params, 2, "", &tstart, &tend);
            delete [] params;
            return result;
        }
        else {
            try {
              if (xlog.isDebugEnabled()) {
                  xlog.debug("Creating native connection");
              }

              conn->mysql_select_db(mysql,db);

              // store mapping from the MYSQL structure to the native connection
              getResourceMap()->setConnection(mysql, conn);

              if (!conn->connect(
                      info->host.c_str(),
                      info->user.c_str(),
                      info->passwd.c_str(),
                      db,
                      info->port==0 ? 3306 : info->port,
                      info->unix_socket,
                      info->clientflag
              )) {
                  setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [1]", "OSP01");
                  return -1;
              }

              // success
              getResourceMap()->clearErrorState(mysql);

              if (xlog.isDebugEnabled()) {
                  xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + string(db) + string("\") SUCCESS"));
              }

              return 0;

            } catch (const char *exception) {
              xlog.error(string("mysql_select_db() failed due to exception: ") + exception);
              setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
              return -1;
            } catch (...) {
              xlog.error(string("mysql_select_db(") + string(db==NULL?"NULL":db) + string(") failed due to exception"));
              setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
              return-1;
            }
        }
    }
}


my_bool mysql_autocommit(MYSQL *mysql, my_bool auto_mode) {
    //trace("mysql_autocommit", mysql);
    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_autocommit(" + Util::toString(auto_mode) + ")");
    }
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_autocommit() returning false because no connection found for mysql handle ") + Util::toString((void*)mysql));
        return false;
    }

    return conn->mysql_autocommit(mysql, auto_mode);
}

MYSQL_STMT * mysql_stmt_init(MYSQL *mysql) {
    //trace("mysql_stmt_init", mysql);
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_stmt_init() returning NULL because no connection found for mysql handle ") + Util::toString((void*)mysql));
        return NULL;
    }
    MYSQL_STMT *stmt = conn->mysql_stmt_init(mysql);
    //TODO: stmtMap[stmt] =
    return stmt;
}

int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query,
        unsigned long length) {
    //trace("mysql_stmt_prepare", stmt);

    return false;

    /*
    if (!valid(stmt)) {
        return false;
    }
    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
    return dbsStmt->parentConn->mysql_stmt_prepare(stmt, query, length);
    */
}

int mysql_stmt_execute(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_execute", stmt);
    return false;

    /*
    if (!valid(stmt)) {
        return false;
    }
    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
    return dbsStmt->parentConn->mysql_stmt_execute(stmt);
    */
}

int mysql_query(MYSQL *mysql, const char *sql) {
    return mysql_real_query(mysql, sql, sql==NULL ? 0 : strlen(sql));
}

int mysql_real_query(MYSQL *mysql, const char *sql, unsigned long length) {
    //trace("mysql_real_query", mysql);

    //struct timeval tvStart, tvEnd;
    //gettimeofday(&tvStart, NULL);

    if (xlog.isDebugEnabled()) {
        xlog.debug(getLogPrefix(mysql) + "mysql_real_query(" + string(sql) + ")");
    }

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_real_query() returning -1 because no connection found for mysql handle ") + Util::toString((void*)mysql));
        return -1;
    }

    int result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_real_query(mysql, sql, length);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[3];
      params[0] = Util::toString(mysql);
      params[1] = (sql ? Util::toEscapedStringLiteral(sql) : "NULL");
      params[2] = Util::toString(length);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_real_query(MYSQL *mysql, const char *sql, unsigned long length)",
          params, 3, "", &tstart, &tend);
      delete [] params;
    }
    else {
      result = conn->mysql_real_query(mysql, sql, length);
    }

    //gettimeofday(&tvEnd, NULL);

/*
    if (xlog.isDebugEnabled()) {
        if (result!=0) {
            // output details of error
            int _errno = mysql_errno(mysql);
            const char *_error = mysql_error(mysql);
            string msg = string("mysql_real_query(") + string(sql) + string(" ) took ") + Util::getElapsedTimeString(&tvStart, &tvEnd)
                        + string(" second(s) and is returning ") + Util::toString(result)
                        + string(" (FAILURE); errno=") + Util::toString(_errno)
                        + string("; error=") + string(_error);
            xlog.warn(msg);
        }
        else {
                // output details of successful query
                string msg = string("mysql_real_query(") + string(sql) + string(" ) took ") + Util::getElapsedTimeString(&tvStart, &tvEnd)
                            + string(" second(s) and is returning 0 (SUCCESS)");
                xlog.debug(msg);
        }
    }
    */
    
    return result;
}

int mysql_send_query(MYSQL *mysql, const char *sql, unsigned long length) {
    //trace("mysql_send_query", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error("mysql_send_query() returning -1 because no connection found");
        return -1;
    }
    return conn->mysql_send_query(mysql, sql, length);
}

MYSQL_RES * mysql_store_result(MYSQL *mysql) {
    ////trace("mysql_store_result BEGIN", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_store_result but there is no current connection");
        return NULL;
    }
    MYSQL_RES *result = conn->mysql_store_result(mysql);
    getResourceMap()->setConnection(result, conn);
    ////trace("mysql_store_result END", mysql);
    return result;
}

my_bool mysql_more_results(MYSQL *mysql) {
    //trace("mysql_more_results", mysql);
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_more_results but there is no current connection");
        return false;
    }
    return conn->mysql_more_results(mysql);
}

my_bool mysql_commit(MYSQL * mysql) {
    //trace("mysql_commit", mysql);
    /*
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_commit(") + Util::toString(mysql) + string(")"));
    }
    */

//    struct timeval tvStart, tvEnd;
//    gettimeofday(&tvStart, NULL);

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_commit but there is no current connection");
        return false;
    }

    my_bool result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_commit(mysql) ;
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_commit(MYSQL * mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
    }
    else {
        result = conn->mysql_commit(mysql);
    }

    //gettimeofday(&tvEnd, NULL);

    /*
    if (xlog.isDebugEnabled()) {
        // Zero if successful. Nonzero if an error occurred.
        if (ret) {
            string msg = string("mysql_commit() took ")
                + Util::getElapsedTimeString(&tvStart, &tvEnd)
                + string(" second(s) and is returning FAILURE"
            );
            xlog.debug(msg);
        }
        else {
            string msg = string("mysql_commit() took ")
                + Util::getElapsedTimeString(&tvStart, &tvEnd)
                + string(" second(s) and is returning SUCCESS"
            );
            xlog.debug(msg);
        }
    }
    */

    return result;
}

my_bool mysql_rollback(MYSQL * mysql) {
    //trace("mysql_rollback", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_rollback but there is no current connection");
        return false;
    }

    my_bool result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_rollback(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_rollback(MYSQL * mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
      return result;
    }
    else {
        return conn->mysql_rollback(mysql);
    }
}

void mysql_close(MYSQL *mysql) {

    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_close()");
    }

    //trace("mysql_close", mysql);
    if (!valid(mysql)) {
        // this is quite common, so we don't want to show a warning for this
        if (xlog.isDebugEnabled()) {
            xlog.debug("mysql_close() called with an invalid MYSQL handle");
        }
        return;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        // this is quite common, so we don't want to show a warning for this
        if (xlog.isDebugEnabled()) {
            xlog.debug("Call to mysql_close but there is no current connection");
        }
        return;
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("BEFORE remove from map");
    }

    MySQLConnectionInfo *info = getResourceMap()->getConnectInfo(mysql);
    // remove from the map
    getResourceMap()->erase(mysql);
    getResourceMap()->eraseResults(conn);

    // Delete info only after erase method.
    if (info) {
        delete info;
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER remove from map, BEFORE delegate mysql_close()");
    }

    // close the connection
    if(MyOSPConfig::isShardAnalyze()) {
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      struct timeval tstart; gettimeofday(&tstart, NULL);
        conn->mysql_close(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_close(MYSQL *mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
    }
    else {
        // TODO: This should be done through the pool. The pool should be global.
        conn->mysql_close(mysql);
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER delegate mysql_close(), BEFORE delete connection object");
    }

    // delete the connection
    delete conn;

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER delete connection object. END of mysql_close().");
    }

    // we delete the mysql structure if it was created by us in mysql_init or mysql_connect (we are required
    // to do this according to the MySQL documentation)
    if (mysqlAllocMap->find(mysql) != mysqlAllocMap->end()) {
        mysqlAllocMap->erase(mysql);
        delete mysql;
    }

}

my_bool mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd) {
    //trace("mysql_stmt_bind_param", stmt);

    return false;

//    if (!valid(stmt)) {
//        return false;
//    }
//    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
//    return dbsStmt->parentConn->mysql_stmt_bind_param(stmt, bnd);
}

my_bool mysql_stmt_close(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_close", stmt);

    return false;
//    if (!valid(stmt)) {
//        return false;
//    }
//    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
//    return dbsStmt->parentConn->mysql_stmt_close(stmt);
}
/*
 int mysql_library_init(int argc, char **argv, char **groups){
 banner();
 //trace("mysql_library_init");
 return 0;
 }

 void mysql_library_end(void){
 //trace("mysql_library_end - UNLOADING Driver");
 cleanup();
 }
 */

bool serverInitDone = false;

int mysql_server_init(int argc, char **argv, char **groups) {
    //trace("mysql_server_init");

    //TODO: add mutex

    if (serverInitDone) {
        return 0;
    }

    // show product banner and configure logging
    banner();

    if (!getMySQLClient()->init()) {
        xlog.error("mysql_server_init() failed to init mysqlclient");
        return -1;
    }

    serverInitDone = true;

    // return success code
    return 0;
}

void mysql_server_end(void) {
    //trace("mysql_server_end");
    cleanup();
}

MYSQL_PARAMETERS * mysql_get_parameters(void) {
    //trace("mysql_get_parameters");
    mysql_get_parametersFnType* tempFunction = (mysql_get_parametersFnType*)getMySQLClient()->get_mysql_function("mysql_get_parameters");
    MYSQL_PARAMETERS * tempValue = tempFunction();
    return tempValue;
}


unsigned int mysql_field_count(MYSQL *mysql) {
    //trace("mysql_field_count", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_count but there is no current connection");
        return 0;
    }
    return conn->mysql_field_count(mysql);
}

my_ulonglong mysql_affected_rows(MYSQL *mysql) {
    //trace("mysql_affected_rows", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_affected_rows but there is no current connection");
        return 0;
    }
    my_ulonglong ret = conn->mysql_affected_rows(mysql);
    if (xlog.isTraceEnabled()) {
        //xlog.trace(string("mysql_affected_rows() returning ") + Util::toString((int)ret));
    }
    return ret;
}

my_ulonglong mysql_insert_id(MYSQL *mysql) {
    //trace("mysql_insert_id", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_insert_id but there is no current connection");
        return 0;
    }

    my_ulonglong result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_insert_id(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_insert_id(MYSQL *mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
      return result;
    }
    else {
      return conn->mysql_insert_id(mysql);
    }
}

unsigned int mysql_warning_count(MYSQL *mysql) {
    //trace("mysql_warning_count", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_warning_count but there is no current connection");
        return 0;
    }
    return conn->mysql_warning_count(mysql);
}

const char * mysql_info(MYSQL *mysql) {
    //trace("mysql_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_info but there is no current connection");
        return NULL;
    }
    return conn->mysql_info(mysql);
}

unsigned long mysql_thread_id(MYSQL *mysql) {
    //trace("mysql_thread_id", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_thread_id but there is no current connection");
        return 0;
    }
    return conn->mysql_thread_id(mysql);
}

const char * mysql_character_set_name(MYSQL *mysql) {
    //trace("mysql_character_set_name", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_character_set_name but there is no current connection");
        return NULL;
    }
    return conn->mysql_character_set_name(mysql);
}

int mysql_set_character_set(MYSQL *mysql, const char *csname) {
    //trace("mysql_set_character_set", mysql);
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("set_character_set(") + string(csname) + string(")"));
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_set_character_set but there is no current connection");
        return 0;
    }
    return conn->mysql_set_character_set(mysql, csname);
}

void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset) {
    //trace("mysql_get_character_set_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_character_set_info but there is no current connection");
        return;
    }
    conn->mysql_get_character_set_info(mysql, charset);
}

int mysql_ping(MYSQL *mysql) {
    //trace("mysql_ping", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_ping but there is no current connection");
        // return non-zero to indicate the connection is bad
        return -1;
    }
    return conn->mysql_ping(mysql);
}

const char * mysql_stat(MYSQL *mysql) {
    //trace("mysql_stat", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_stat but there is no current connection");
        return NULL;
    }
    return conn->mysql_stat(mysql);
}

const char * mysql_get_server_info(MYSQL *mysql) {
    //trace("mysql_get_server_info", mysql);
    const char *ret = NULL;
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        ret = conn->mysql_get_server_info(mysql);
    }
    if (!ret) {
        // fail-safe to prevent segv
        ret = "5.1.32";
    }
    return ret;
}

const char * mysql_get_client_info(void) {
    //trace("mysql_get_client_info");
    mysql_get_client_infoFnType* tempFunction = (mysql_get_client_infoFnType*)getMySQLClient()->get_mysql_function("mysql_get_client_info");
    const char * tempValue = tempFunction();
    return tempValue;
}

unsigned long mysql_get_client_version(void) {
    //trace("mysql_get_client_version");
    mysql_get_client_versionFnType* tempFunction = (mysql_get_client_versionFnType*)getMySQLClient()->get_mysql_function("mysql_get_client_version");
    unsigned long tempValue = tempFunction();
    //xlog.trace(string("mysql_get_client_version() returning ") + Util::toString((int)tempValue));
    return tempValue;
}

const char * mysql_get_host_info(MYSQL *mysql) {
    //trace("mysql_get_host_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_host_info but there is no current connection");
        return NULL;
    }
    return conn->mysql_get_host_info(mysql);
}

unsigned long mysql_get_server_version(MYSQL *mysql) {
    //trace("mysql_get_server_version");
    unsigned long ret = 0;
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        ret = conn->mysql_get_server_version(mysql);
    }
    return ret;
}

unsigned int mysql_get_proto_info(MYSQL *mysql) {
    //trace("mysql_get_proto_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_proto_info but there is no current connection");
        return 0;
    }
    return conn->mysql_get_proto_info(mysql);
}

MYSQL_RES * mysql_list_dbs(MYSQL *mysql, const char *wild) {
    //trace("mysql_list_dbs", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_dbs but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_dbs(mysql, wild);
}

MYSQL_RES * mysql_list_tables(MYSQL *mysql, const char *wild) {
    //trace("mysql_list_tables", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_tables but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_tables(mysql, wild);
}

MYSQL_RES * mysql_list_processes(MYSQL *mysql) {
    //trace("mysql_list_processes", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_processes but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_processes(mysql);
}

// this is the signature for MySQL prior to 5.1.18
int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg) {
    // mysql_options modifies the MYSQL struct so we can just delegate to the real driver for this
    return getMySQLClient()->mysql_options(mysql, option, arg);
    /*
    //trace("mysql_options", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        if (xlog.isDebugEnabled()) xlog.debug("Ignoring call to mysql_options and simulating success because there is no connection");
        return 0;
    }
    return conn->mysql_options(mysql, option, arg);
    */
}

// this is the signature for MySQL 5.1.18 and later
int mysql_options(MYSQL *mysql, enum mysql_option option, const void *arg) {
    // mysql_options modifies the MYSQL struct so we can just delegate to the real driver for this
    return getMySQLClient()->mysql_options(mysql, option, arg);
    //trace("mysql_options", mysql);
    /*
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        if (xlog.isDebugEnabled()) xlog.debug("Ignoring call to mysql_options and simulating success because there is no connection");
        return 0;
    }
    return conn->mysql_options(mysql, option, arg);
    */
}

unsigned long mysql_escape_string(char *to, const char *from,
        unsigned long from_length) {
    //trace("mysql_escape_string");

    return getMySQLClient()->mysql_escape_string(to, from, from_length);
}

unsigned long mysql_hex_string(char *to, const char *from,
        unsigned long from_length) {
    //trace("mysql_hex_string");
    // delegate directly to MySQL driver
//    //boost::mutex::scoped_lock lock(nonConnMutex);
//    return getNonConn()->mysql_hex_string(to, from, from_length);
    return -1;
}

unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
        const char *from, unsigned long length) {

    //trace("mysql_real_escape_string", mysql);

    return getMySQLClient()->mysql_real_escape_string(mysql, to, from, length);
}

int mysql_next_result(MYSQL *mysql) {
    //trace("mysql_next_result", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_next_result but there is no current connection");
        return -1;
    }
    return conn->mysql_next_result(mysql);
}


void mysql_debug(const char *debug) {
    //trace("mysql_debug");
    xlog.debug(string("mysql_debug(\"") + string(debug==NULL?"":debug) + string("\""));
}

MYSQL_RES * mysql_use_result(MYSQL *mysql) {
    //trace("mysql_use_result", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_use_result but there is no current connection");
        return NULL;
    }
    return conn->mysql_use_result(mysql);
}


int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option) {
    //trace("mysql_set_server_option", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_set_server_option but there is no current connection");
        return 0;
    }
    return conn->mysql_set_server_option(mysql, option);
}

//---------------------------------------------------------------------------------------------------
//TODO: the following methods all have MYSQL_RES* arguments and should delegate to ResultSet object
// rather than to a Connection object!!
//---------------------------------------------------------------------------------------------------

void mysql_free_result(MYSQL_RES *res) {
    //trace("mysql_free_result", res);
    if (res==NULL) {
        return;
    }
    MySQLAbstractResultSet *rs = getResourceMap()->getResultSet(res);
    if (rs == NULL) {
        xlog.warn(string("Call to mysql_free_result(") + Util::toString((void*)res) + string(") but there is no result set mapped to that MYSQL_RES*"));
        return;
    }

    // remove from map
    getResourceMap()->erase(res);

    // delegate to result set class
    rs->mysql_free_result(res);

    // delete the result set class
    delete rs;
}

void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset) {
    //trace("mysql_data_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_data_seek but there is no current connection");
        return;
    }
    conn->mysql_data_seek(result, offset);
}

MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset) {
    //trace("mysql_row_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_row_seek but there is no current connection");
        return NULL;
    }
    return conn->mysql_row_seek(result, offset);
}

MYSQL_RES * mysql_list_fields(MYSQL *mysql, const char *table, const char *wild) {
    //trace("mysql_list_fields", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_list_fields");
    return NULL;
}

void myodbc_remove_escape(MYSQL *mysql, char *name) {
    //trace("myodbc_remove_escape", mysql);
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: myodbc_remove_escape");
}

unsigned int mysql_thread_safe(void) {
    //trace("mysql_thread_safe");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_safe");
    return 0;
}

my_bool mysql_embedded(void) {
    //trace("mysql_embedded");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_embedded");
    return false;
}

MYSQL_MANAGER* mysql_manager_init(MYSQL_MANAGER* con) {
    //trace("mysql_manager_init");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_init");
    return NULL;
}

MYSQL_MANAGER* mysql_manager_connect(MYSQL_MANAGER* con, const char* host,
        const char* user, const char* passwd, unsigned int port) {
    //trace("mysql_manager_connect");
//    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_connect");
    return NULL;
}

void mysql_manager_close(MYSQL_MANAGER* con) {
    //trace("mysql_manager_close");
//    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_close");
}

int mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len) {
    //trace("mysql_manager_command");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_command");
    return -1;
}

int mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf,
        int res_buf_size) {
    //trace("mysql_manager_fetch_line");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_fetch_line");
    return -1;
}

my_bool mysql_read_query_result(MYSQL *mysql) {
    //trace("mysql_read_query_result", mysql);
    if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_read_query_result");
    return false;
}

int mysql_stmt_fetch(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_fetch");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_fetch");
    return -1;
}

int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,
        unsigned int column, unsigned long offset) {
    //trace("mysql_stmt_fetch_column");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_fetch_column");
    return -1;
}

int mysql_stmt_store_result(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_store_result");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_store_result");
    return -1;
}

unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_param_count");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_param_count");
    return 0;
}

my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, const void *attr) {
    //trace("mysql_stmt_attr_set");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_attr_set");
    return 0;
}

my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, void *attr) {
    //trace("mysql_stmt_attr_get");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_attr_get");
    return false;
}

my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd) {
    //trace("mysql_stmt_bind_result");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_bind_result");
    return false;
}

my_bool mysql_stmt_reset(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_reset");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_reset");
    return false;
}

my_bool mysql_stmt_free_result(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_free_result");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_free_result");
    return false;
}

my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt, unsigned int param_number,
        const char *data, unsigned long length) {
    //trace("mysql_stmt_send_long_data");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_send_long_data");
    return false;
}

MYSQL_RES * mysql_stmt_result_metadata(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_result_metadata");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_result_metadata");
    return NULL;
}

MYSQL_RES * mysql_stmt_param_metadata(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_param_metadata");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_param_metadata");
    return NULL;
}

unsigned int mysql_stmt_errno(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_errno");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_errno");
    return -1;
}

const char * mysql_stmt_error(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_error");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_error");
    return NULL;
}

const char * mysql_stmt_sqlstate(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_sqlstate");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_sqlstate");
    return NULL;
}

MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt, MYSQL_ROW_OFFSET offset) {
    //trace("mysql_stmt_row_seek");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_row_seek");
    return NULL;
}

MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_row_tell");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_row_tell");
    return NULL;
}

void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset) {
    //trace("mysql_stmt_data_seek");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_data_seek");
}

my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_num_rows");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_num_rows");
    return 0;
}

my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_affected_rows");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_affected_rows");
    return 0;
}

my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_insert_id");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_insert_id");
    return 0;
}

unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_field_count");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_field_count");
    return -1;
}

MYSQL * mysql_connect(MYSQL *mysql, const char *host, const char *user,
        const char *passwd) {
    //trace("mysql_connect", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_connect");
    return NULL;
}

int mysql_create_db(MYSQL *mysql, const char *DB) {
    //trace("mysql_create_db", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_create_db");
    return -1;
}

int mysql_drop_db(MYSQL *mysql, const char *DB) {
    //trace("mysql_drop_db", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_drop_db");
    return -1;
}

my_bool mysql_thread_init(void) {
    //trace("mysql_thread_init");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_init");
    return false;
}

void mysql_thread_end(void) {
    //trace("mysql_thread_end");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_end");
}

my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
        const char *ca, const char *capath, const char *cipher) {
    //trace("mysql_ssl_set", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_ssl_set");
    return false;
}

const char * mysql_get_ssl_cipher(MYSQL *mysql) {
    //trace("mysql_get_ssl_cipher", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_get_ssl_cipher");
    return NULL;
}

my_bool mysql_change_user(MYSQL *mysql, const char *user, const char *passwd,
        const char *db) {
    //trace("mysql_change_user", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_change_user");
    return false;
}

my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length) {
    //trace("mysql_master_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_master_query");
    return false;
}

my_bool mysql_master_send_query(MYSQL *mysql, const char *q,
        unsigned long length) {
    //trace("mysql_master_send_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_master_query");
    return false;
}

my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length) {
    //trace("mysql_slave_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_slave_query");
    return false;
}

my_bool mysql_slave_send_query(MYSQL *mysql, const char *q,
        unsigned long length) {
    //trace("mysql_slave_send_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_slave_query");
    return false;
}

void mysql_enable_rpl_parse(MYSQL* mysql) {
    //trace("mysql_enable_rpl_parse", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_enable_rpl_parse");
}

void mysql_disable_rpl_parse(MYSQL* mysql) {
    //trace("mysql_disable_rpl_parse", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_disable_rpl_parse_rpl_parse");
}

int mysql_rpl_parse_enabled(MYSQL* mysql) {
    //trace("mysql_rpl_parse_enabled", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_parse_enabled");
    return 0;
}

void mysql_enable_reads_from_master(MYSQL* mysql) {
    //trace("mysql_enable_reads_from_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_enable_reads_from_master");
}

void mysql_disable_reads_from_master(MYSQL* mysql) {
    //trace("mysql_disable_reads_from_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_disable_reads_from_master");
}

my_bool mysql_reads_from_master_enabled(MYSQL* mysql) {
    //trace("mysql_reads_from_master_enabled", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_reads_from_master_enabled");
    return false;
}

enum mysql_rpl_type mysql_rpl_query_type(const char* q, int len) {
    //trace("mysql_rpl_query_type");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_query_type");
    //TODO: not sure what to return here
    return MYSQL_RPL_MASTER;
}

my_bool mysql_rpl_probe(MYSQL* mysql) {
    //trace("mysql_rpl_probe", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_probe");
    return false;
}

int mysql_set_master(MYSQL* mysql, const char* host, unsigned int port,
        const char* user, const char* passwd) {
    //trace("mysql_set_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_master");
    return 0;
}

int mysql_add_slave(MYSQL* mysql, const char* host, unsigned int port,
        const char* user, const char* passwd) {
    //trace("mysql_add_slave", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_add_slave");
    return 0;
}

int mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level) {
    //trace("mysql_shutdown", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_shutdown");
    return 0;
}

int mysql_dump_debug_info(MYSQL *mysql) {
    //trace("mysql_dump_debug_info", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_dump_debug_info");
    return 0;
}

int mysql_refresh(MYSQL *mysql, unsigned int refresh_options) {
    //trace("mysql_refresh", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_refresh");
    return 0;
}

int mysql_kill(MYSQL *mysql, unsigned long pid) {
    //trace("mysql_kill", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_kill");
    return 0;
}

void mysql_set_local_infile_handler(MYSQL *mysql,
    int (*local_infile_init)(void **, const char *, void *),
    int (*local_infile_read)(void *, char *, unsigned int),
    void (*local_infile_end)(void *),
    int (*local_infile_error)(void *, char*, unsigned int),
    void *)
{
    // no implementation
    //trace("mysql_set_local_infile_handler");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_local_infile_handler");
}

void mysql_set_local_infile_default(MYSQL *mysql) {
    // no implementation
    //trace("mysql_set_local_infile_default");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_local_infile_default");
}

my_ulonglong mysql_num_rows(MYSQL_RES *res) {
    //trace("mysql_num_rows", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) {
            xlog.debug("mysql_num_rows() returning 0 (no connection)");
        }
        return 0;
    }
    my_ulonglong ret = conn->mysql_num_rows(res);
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_num_rows() returning ") + Util::toString((long int)ret));
    }
    return ret;
}

unsigned int mysql_num_fields(MYSQL_RES *res) {
    //trace("mysql_num_fields", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_num_fields but there is no current connection");
        return 0;
    }
    return conn->mysql_num_fields(res);
}

my_bool mysql_eof(MYSQL_RES *res) {
    //trace("mysql_eof", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_eof but there is no current connection");
        return 0;
    }
    return conn->mysql_eof(res);
}

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result) {
    //trace("mysql_fetch_row BEGIN", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_row but there is no current connection");
        return NULL;
    }
    MYSQL_ROW ret = conn->mysql_fetch_row(result);
    //trace("mysql_fetch_row END", result);
    return ret;
}

unsigned long * mysql_fetch_lengths(MYSQL_RES *result) {
    //trace("mysql_fetch_lengths", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_lengths but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_lengths(result);
}

MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result,
        MYSQL_FIELD_OFFSET offset) {
    //trace("mysql_field_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_seek but there is no current connection");
        return 0;
    }
    return conn->mysql_field_seek(result, offset);
}

MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result) {
    //trace("mysql_fetch_field", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_field but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_field(result);
}

MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr) {
    //trace("mysql_fetch_field_direct", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_field_direct but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_field_direct(res, fieldnr);
}

MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res) {
    //trace("mysql_fetch_fields", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_fields but there is no current connection");
        return NULL;
    }
    MYSQL_FIELD * ret = conn->mysql_fetch_fields(res);

    //BEGIN DEBUG CODE
    /*
    unsigned int n = mysql_num_fields(res);
    for (unsigned int i=0; i<n; i++) {
        //logger.info << "Field #" << i << endl;
        //logger.info << "Field Name: " << i << ret[i].name << endl;
    }
    */
    //END DEBUG CODE

    return ret;
}

MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res) {
    //trace("mysql_row_tell", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_row_tell but there is no current connection");
        return 0;
    }
    return conn->mysql_row_tell(res);
}

MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res) {
    //trace("mysql_field_tell", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_tell but there is no current connection");
        return 0;
    }
    return conn->mysql_field_tell(res);
}

