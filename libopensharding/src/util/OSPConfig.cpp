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
 * Returns true if the host string is defined as an osp host in the config file.
 */
/*static*/ bool OSPConfig::isOspHost(string _host) {
	if(!init) {
		OSPConfig::loadConfigs(getConfFile());
		init=true;
	}

	bool ret = false;
	for(size_t i=0; i<ospHosts.size() && !ret; i++) {
		//cerr << "ospHost #" << i << ": " << ospHosts[i] << endl;
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
	
	try {
        vector<string> ret;
        size_t pos1=0;
        size_t pos2=host_url.find(":");
        size_t pos3=0;
    
        //push vendor name to ret[0]
        ret.push_back(host_url.substr(pos1, pos2-pos1));
    
        //Check that url defined a vendor name
        if (ret[0] == "") {
            throw 23;
        }
        
        pos1=pos2+1;
        pos2=host_url.find(":", pos1);
    
        //push protocol to ret[1]
        ret.push_back(host_url.substr(pos1, pos2-pos1));
    
        if (strncmp(host_url.substr(pos1, 3), "://") != 0) {
            //Catch 23 at end of try to consolidate throws
            throw 23;
        }
    
        //this time we skip 3 chars, '://'
        pos1=pos2+3;
        pos2=host_url.find("/", pos1);
        pos3=host_url.find(":", pos1);
    
        //push actual_host and port to ret[2] and ret[3]
        if(pos3 == string::npos || pos3 > pos2) {
            ret.push_back(host_url.substr(pos1, pos2-pos1));
            //port not defined, value of 0 is translated to default by rdms-specific extension.
            ret.push_back("0");
        }
        else {
            ret.push_back(host_url.substr(pos1, pos3-pos1));
            ret.push_back(host_url.substr(pos3+1, pos2-pos3-1));
        }
    
        //Check that url defined a host name
        if (ret[2] == "") {
            throw 23;
        }
        
        pos1=pos2+1;
        pos2=host_url.find("/", pos1);
    
        //push domain to ret[4]
        ret.push_back(host_url.substr(pos1, pos2-pos1));
        pos1=pos2+1;
        pos2=host_url.find("/", pos1);
    
        //push dbms to ret[5]
        ret.push_back(host_url.substr(pos1, pos2-pos1));
        pos1=pos2+1;
        pos2=host_url.find("?", pos1);
    
        //push schema to ret[6]
        if (pos2 == string::npos) {
            ret.push_back(string(""));
        }
        else {
            ret.push_back(host_url.substr(pos1, pos2-pos1));
        }
        //ignoring user= and password= for now.
    
        return ret;
    }
    catch (23) {
        throw Util::createException(string("Invalid host url: ") + host_url + string("\nSee myosp.sample.conf for proper syntax"));
        return NULL;
    }
}

} //end namespace util


