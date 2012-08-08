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
   
/*Includes*/
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <opensharding/OSPConnectionInfo.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>
#include <opensharding/OSPConnectionPool.h>
#include <logger/Logger.h>
#include <vector>
#include <util/Util.h>
#include <util/OSPConfig.h>

#define DEBUG log.isDebugEnabled()

#define LOCK_MUTEX boost::mutex::scoped_lock lock(mutex);

using namespace util;

namespace opensharding {

// TODO: Get value from config
const int  POOL_MODE_PREFORK = 1;
const int    POOL_MODE_DEDICATED = 2;
const int    POOL_MODE_FIXED_SIZE = 3;

const int       POOL_MODE = POOL_MODE_DEDICATED;

int OSPConnectionPool::nextPipeId = 1;
    
	logger::Logger &OSPConnectionPool::log = Logger::getLogger("OSPConnectionPool");
	
	/*Constructor*/
	OSPConnectionPool::OSPConnectionPool() {
		init();
	}
	
	
	/*Destructor*/
	OSPConnectionPool::~OSPConnectionPool(){
		LOCK_MUTEX

		// when the connection pool is deleted we want to delete all the connections
        for (vector<OSPConnection*>::iterator it=pool.begin(); it!=pool.end(); it++) {
            delete (OSPConnection*) *it;
        }

	}
	
	
	/*Initializer*/
	void OSPConnectionPool::init() {
		// TODO: Init will be used later to init a fixed size pool.
	}
	
	
	/**
	 * Create a connection.
	 */
	int OSPConnectionPool::do_create_connection_method(OSPConnectionInfo *connInfo){

		protocol = connInfo->getProtocol();
		
		//index = pool.size();
		if (protocol == PROTOCOL_TCP) {
			pool.push_back(dynamic_cast<OSPConnection*>(new OSPTCPConnection(connInfo->getHost(),
				connInfo->getPort())));
		}
		else if (protocol == PROTOCOL_PIPES) {
				pool.push_back(dynamic_cast<OSPConnection*>(new OSPNamedPipeConnection(connInfo,false,
                    OSPConnectionPool::nextPipeId++)));
		}
		else {
			log.error("Invalid protocol for Connection.");
            throw "Invalid protocol for Connection.";
		}
				
		if(DEBUG) log.debug(string("Returned the index of created connection. Index: ") 
			+ Util::toString(pool.size()));
			
		return pool.size()-1;
			
	}
	
	/*Returns the next pipe to be used*/
	OSPConnection*	OSPConnectionPool::getConnection(void* dbHandle, OSPConnectionInfo *connInfo) {
	 
		//Variable to catch the index
		int mapIndex;	 
		
		if( POOL_MODE == POOL_MODE_PREFORK) {
	 	
			if(pool.size()==0){
				LOCK_MUTEX
				mapIndex = do_create_connection_method(connInfo);
				connectionIndexByNameMap[dbHandle] = mapIndex;
			}	 
		
			return pool[0];
		
			if(DEBUG) log.debug(string("Returned connection to be used in POOL_MODE_PREFORK."));
	 	
			
	 	} else if(POOL_MODE==POOL_MODE_DEDICATED) {

			LOCK_MUTEX
			mapIndex = do_create_connection_method(connInfo);
			connectionIndexByNameMap[dbHandle] = mapIndex;

			if(DEBUG) log.debug(string("Returned POOL_MODE_DEDICATED Connection. Index: ") + Util::toString(mapIndex)); 

			return pool[mapIndex];
	 		 	
	 	}
	 	
		 else if(POOL_MODE==POOL_MODE_FIXED_SIZE){

			 // TODO: Not yet implemented.
			 // Note: This version may not be required.
			 
		} else {
			
			log.error(string("Invalid POOL_MODE Selected. POOL_MODE: ") + Util::toString(POOL_MODE));
		}
		
		
	}

	/**
	 * Close a connectin.
	 */
	void OSPConnectionPool::closeConnection(void* dbHandle) {

		LOCK_MUTEX
		if(connectionIndexByNameMap.count(dbHandle) > 0) {
			int pool_index = connectionIndexByNameMap[dbHandle];
			OSPConnection* ospConnection = pool[pool_index];
			connectionIndexByNameMap.erase(dbHandle);
			pool[pool_index] = NULL;
			delete ospConnection;
			
		} else {
			throw string("Invalid pool index. dbHandle: ") + Util::toString(dbHandle);
		}
		
	} 
	
}

