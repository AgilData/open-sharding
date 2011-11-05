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

#include <util/Util.h>
#include <logger/Logger.h>
#include <util/OSPConfig.h>

using namespace std;

namespace util {
/*static*/ bool OSPConfig::init=false;
/*static*/ bool OSPConfig::shardAnalyze=false;
/*static*/ map<string,string> OSPConfig::configMap;
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

	//Add default values
	configMap["osp.mode"]="delegate";

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
	}
	shardAnalyze = Util::equalsIgnoreCase(configMap["shard.analyze.log"], "TRUE");
}

/*static*/ bool OSPConfig::isShardAnalyze() {
	return shardAnalyze;
}

} //end namespace util


