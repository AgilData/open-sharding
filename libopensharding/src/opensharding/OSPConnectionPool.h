/**
 * Copyright (c) 2011, CodeFutures Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __OSPConnectionPool_h__
#define __OSPConnectionPool_h__

#include <opensharding/OSPConnection.h>
#include <opensharding/OSPConnectionInfo.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>
#include <map>
#include <util/OSPConfig.h>
#include <util/Util.h>

#define DEFAULT_POOL_SIZE = 1; 

#define pool_size = -1;


namespace opensharding {
	class OSPConnectionPool {
		private:
            
        int protocol; 
		unsigned int index;
		boost::mutex mutex;
      

		OSPConnectionInfo *connInfo;
		map<void*,int> connectionIndexByNameMap;
		vector<OSPConnection*> pool;
        static logger::Logger &log;
		
		int do_create_connection_method();
		void init();
			
		public:
        static const int PROTOCOL_TCP = 1;
        static const int PROTOCOL_PIPES = 2;
			
		OSPConnectionPool(OSPConnectionInfo *connInfo);
		~OSPConnectionPool();
		
		OSPConnection* getConnection(void* dbHandle);
		void closeConnection(void* dbHandle);
	
	};
}

#endif // __OSPConnectionPool_h__	
