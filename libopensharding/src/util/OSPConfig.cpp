

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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>

#include <util/Util.h>
#include <logger/Logger.h>
#include <util/OSPConfig.h>

using namespace std;

namespace util {
/*static*/ bool OSPConfig::init=false;
/*static*/ map<string, string> OSPConfig::configMap;
/*static*/ vector<string> OSPConfig::ospHosts;
/*static*/ string OSPConfig::configFile = "";

OSPConfig::OSPConfig() {
}

OSPConfig::~OSPConfig() {
}

/*static*/ string OSPConfig::getConfFile() {
	return configFile;
}

/*static*/ map<string,string> OSPConfig::getConfigMap() {
	if(!init) {
		OSPConfig::loadConfigs(getConfFile());
		init=true;
	}
	return configMap;
}

/*static*/ void OSPConfig::loadConfigs(string fileName) {

	string line;
	ifstream confFile(fileName.c_str());
	if(confFile.is_open()) {
		while(!confFile.eof()) {
			getline(confFile, line);

			if(line.size()==0 || Util::trim(line)[0]=='#') {
				//blank lines or comments, ignore
				continue;
			}

			// look for '='
			int pos = line.find('=');
			if (pos<=0) {
				// ignore, not valid
				continue;
			}
			string className = Util::trim(line.substr(0,pos));
			string levelName = Util::trim(line.substr(pos+1));
			configMap[className]=levelName;
		}
		confFile.close();
		
		//Populate ospHosts vector with values from the config file.
		size_t pos1 = 0, pos2 = 0;
		string hosts = configMap[OSP_VIRTUAL_HOST_LIST];
		while( pos1 < hosts.length() && pos2 != string::npos )  {
			pos2 = hosts.find_first_of(',',pos1);
			ospHosts.push_back( Util::trim(hosts.substr(pos1,pos2-pos1)) );
			pos1 = pos2+1;
		}
	}
}
/**
*Added 
*Returns true if the pool connection size has been defined
*/
/*static*/ int OSPConfig::getMode() {
            string mode = getConfigMap()[MODE];
			if(strcmp(mode.c_str(),"PREFORK"))		return 1;
			if(strcmp(mode.c_str(),"DEDICATED"))		return 2;
			if(strcmp(mode.c_str(),"POOL"))			return 3;

			// error condition
			return 0;
	}

/**
 * Returns true if the host string is defined as an osp host in the config file.
 */
/*static*/ bool OSPConfig::isOspHost(string _host) {
	if(!init) {
		OSPConfig::loadConfigs(getConfFile());
		init=true;
	}
	bool ret = false;
	for(size_t i=0; i<ospHosts.size() && !ret; i++) {
		ret = (_host.compare(ospHosts[i]) == 0);
	}
	return ret;
}

/*static*/ string OSPConfig::getHostUrl(string virtual_host) {
	string key = virtual_host + ".url";
    string host_url = getConfigMap()[key];
    if(host_url == "") {
        throw Util::createException((string("No host.url defined for hostname ") + virtual_host).c_str());
    }
    return host_url;
}

/**
 * Parse host.url and return values in a vector
 */
/*static*/ vector<string> OSPConfig::parseVirtualHostUrl(string host_url){
	//TODO better checks on url syntax
	
    //virtual_host string format: 
    //[vendor]:[protocol]://[actual_host]:[port]/[domain]/[dbms]/[schema]?user=[username]&password=[password]
    //[vendor]:[protocol]:/path/to/file:/[domain]/[dbms]/[schema]?user=[username]&password=[password]

    //cerr << "OSPConfig::parseVirtualHostUrl(" << host_url << ")" << endl;
	
	vector<string> ret;
	size_t pos1=0;
	size_t pos2=host_url.find(":");
	size_t pos3=0;
	
	//push vendor name to ret[0]
	ret.push_back(host_url.substr(pos1, pos2-pos1));

	//Check that url defined a vendor name
	if (ret[0] == "") {
		throw Util::createException((string("The URL is invalid, vendor name is required. url: ") + host_url).c_str());
	}

	// parse protocol
	pos1=pos2+1;
	pos2=host_url.find(":", pos1);

	string protocol = host_url.substr(pos1, pos2-pos1);

	//push protocol to ret[1]
	ret.push_back(protocol);

	if (Util::toLower(protocol) == "socket") {

        // skip the ':'
        pos1=pos2+1;
        // find the next ':'
        pos2=host_url.find(":", pos1);

    	string socketFile = host_url.substr(pos1, pos2-pos1);
    	ret.push_back(socketFile);

        // skip the ':/' after filename
    	pos1=pos2+2;
	}
	else if (Util::toLower(protocol) == "pipes" || Util::toLower(protocol) == "tcp" || Util::toLower(protocol) == "socket") {

        if (host_url.substr(pos2, 3) != "://") {
            throw Util::createException((string("The URL is invalid, the host must start with '://'. url: ") + host_url).c_str());
        }

        //this time we skip 3 chars, '://'
        pos1=pos2+3;
        pos2=host_url.find("/", pos1);
        pos3=host_url.find(":", pos1);

        //push actual_host and port to ret[2] and ret[3]
        string host;
        string port;
        if(pos3 == string::npos || pos3 > pos2) {
            host = host_url.substr(pos1, pos2-pos1);
            //port not defined, value of 0 is translated to default by dbms-specific extension.
            port = "0";
        }
        else {
            host = host_url.substr(pos1, pos3-pos1);
            port = host_url.substr(pos3+1, pos2-pos3-1);
        }
        ret.push_back(host);
        ret.push_back(port);

        //Check that url defined a host name
        if (host == "") {
            throw Util::createException((string("The URL is invalid, host is required. url: ") + host_url).c_str());
        }

        // skip the '/' after port
    	pos1=pos2+1;
	}
	else {
		throw Util::createException((string("The URL is invalid, the protocol contains a non-valid value. Valid values: pipes|tcp|socket. value ") + protocol).c_str());
	}

	// parse domain
	pos2=host_url.find("/", pos1);
	if (pos2 == string::npos) {
		throw Util::createException((string("The URL is invalid, domain is required. url: ") + host_url).c_str());
	}

	//push domain to ret
	string domain = host_url.substr(pos1, pos2-pos1);
	if (domain == "") {
		throw Util::createException((string("The URL is invalid, domain is required. url: ") + host_url).c_str());
	}
	ret.push_back(domain);

	// parse dbms
	pos1=pos2+1;
	pos2=host_url.find("/", pos1);

	if (pos2 == string::npos) {
		// this the last item in the url
		string dbms = host_url.substr(pos1);
		ret.push_back(dbms);
		return ret;
	}

	//push dbms to ret
	string dbms = host_url.substr(pos1, pos2-pos1);
	if (dbms == "") {
		throw Util::createException((string("The URL is invalid, dbms is required. url: ") + host_url).c_str());
	}
	ret.push_back(dbms);

	// parse schema
	pos1=pos2+1;
	pos2=host_url.find("?", pos1);

	//push schema to ret
	string schema = "";
	if (pos2 == string::npos) {
		schema = host_url.substr(pos1);
		ret.push_back(schema);
	}
	else {
		schema = host_url.substr(pos1, pos2-pos1);
		ret.push_back(schema);
	}
	if (schema == "") {
		throw Util::createException((string("The URL is invalid, schema is required. url: ") + host_url).c_str());
	}
	//ignoring user= and password= for now.

	return ret;
}

/*static*/ string OSPConfig::getStringProperty(string name) {
    map<string,string> config = getConfigMap();
    return config[name];
}

/*static*/ int OSPConfig::getIntProperty(string name, int defaultValue) {
    string value = getStringProperty(name);
    if (value=="") {
        return defaultValue;
    }
    return Util::toInt(value);
}

/*static*/ bool OSPConfig::getBoolProperty(string name, bool defaultValue) {
    string value = getStringProperty(name);
    if (value=="") {
        return defaultValue;
    }
    return value == "true" || value=="TRUE";
}


} //end namespace util


