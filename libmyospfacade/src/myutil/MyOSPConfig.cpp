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

#include <fstream>
#include <iostream>
#include <map>

#include <util/Util.h>
#include <logger/Logger.h>
#include <util/OSPConfig.h>
#include <myutil/MyOSPConfig.h>

#define CONFDIR_ENV_VAR "MYOSP_CONF_DIR"
#define CONFDIR_DEFAULT "/etc/osp/"
#define CONF_FILE "myosp.conf"

using namespace std;

namespace util {

MyOSPConfig::MyOSPConfig() {
}

MyOSPConfig::~MyOSPConfig() {
}

/*static*/ string MyOSPConfig::getConfFile() {
	if(strcmp(configFile.c_str(), "") == 0) {
		const char * configDir = getenv(CONFDIR_ENV_VAR);
		if(configDir == NULL) {
			configDir = CONFDIR_DEFAULT;
		}
		configFile = string(configDir) + CONF_FILE;
	}
	return configFile;
}

/*static*/ FILE * MyOSPConfig::getAnalyzeLogFile() {
	if(shardAnalyze && !shardAnalyzeLog) {
		string logDir;
		if((logDir = MyOSPConfig::getConfigMap()["shard.analyze.log.dir"]) == "" ) {
			//TODO if no log file is defined, use stdout or stderr?
			shardAnalyzeLog = NULL;
		}
		else {
			string fileName = logDir + string("/myosp-analyze.log");
			shardAnalyzeLog = fopen(fileName.c_str(), "a");
			if(!shardAnalyzeLog) {
				cerr << "Failed to load shard analyze log file: " << fileName
					 << "\nUsing stderr" << endl;
			}
		}
	}
	return shardAnalyzeLog;
}

} //namespace util
