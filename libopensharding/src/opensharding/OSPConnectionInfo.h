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
#ifndef __OSPConnectionInfo_h__
#define __OSPConnectionInfo_h__

#include<string>

using namespace std;

namespace opensharding{
	class OSPConnectionInfo {
	
		public:
		//const int PROTOCOL_TCP = 1;
		//const int PROTOCOL_PIPES = 2;
		
		string osp_vendor, virtual_host, host, osp_domain, target_dbms, target_schema_name, user, passwd;
		unsigned int port;
		int protocol;
		OSPConnectionInfo();
		~OSPConnectionInfo();
		unsigned int getPort() {return port;}
		int getProtocol() {return protocol;}
		string getHost() {return host;}
        string getVirtualHost() {return virtual_host;}
        string getOspDomain() {return osp_domain;}
        string getTargetDms() {return target_dbms;}
        string getTargetSchemaName() {return target_schema_name;}
        string getUser() {return user;}
        string getPassword() {return passwd;}
		
		};
		
		
	}
    
#endif /* OSPConnectionInfo_H_ */
