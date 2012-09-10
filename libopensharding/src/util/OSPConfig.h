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

#ifndef OSPCONFIG_H_
#define OSPCONFIG_H_

#include <string>
#include <map>
#include <vector>
#include <stdio.h>

#define OSP_VIRTUAL_HOST_LIST "osp.virtual.host.list"
//Added
#define MODE "osp.conn.mode"
#define CONN_POOL_SIZE "osp.conn.pool.size"

using namespace std;

namespace util {

class OSPConfig {
protected:
	static bool init;
	static map<string, string> configMap;
	static string configFile;
	static vector<string> ospHosts;
	static vector<string> hostUrl;

public:
    OSPConfig();
    virtual ~OSPConfig();

	static string getConfFile();
    static void loadConfigs(string fileName);
    static map<string, string> getConfigMap();
    static bool isOspHost(string host);
    static string getHostUrl(string host);
    static vector<string> parseVirtualHostUrl(string host_url);
    static int getMode();

    static string   getStringProperty(string name);
    static int      getIntProperty(string name, int defaultValue);
    static bool     getBoolProperty(string name, bool defaultValue);

};

} //end namespace util

#endif /* OSPCONFIG_H_ */
