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

#include <myutil/MyOSPLogger.h>
#include <myutil/MyOSPConfig.h>
#include <logger/Logger.h>

using namespace std;
using namespace util;

namespace logger {

/*static*/ bool MyOSPLogger::init = false;
/*static*/ bool MyOSPLogger::shardAnalyze = false;

/*static*/ Logger &MyOSPLogger::getLogger(string name) {
	if(!init) {
		//configure logger
		MyOSPLogger::configure(MyOSPConfig::getConfFile());
		init = true;
	}
	return Logger::getLogger(name);
}

bool MyOSPLogger::isShardAnalyze() {
	if(!config_init) {
		shardAnalyze = (MyOSPConfig::getConfigMap()["shard.analyze.log"] == "TRUE");
		if(shardAnalyze) {
			string logDir;
			if((logDir = MyOSPConfig::getConfigMap()["shard.analyze.log.dir"]) == "" ) {
				logDir = string("/var/log/");
			}
			string fileName = logDir + string("/pqosp-analyze.log");
			shardAnalyzeLog = fopen(fileName.c_str(), "a");
			if(!shardAnalyzeLog) {
				cerr << "Failed to load shard analyze log file: " << fileName << endl;
			}
		}
		config_init=true;
	}
	return shardAnalyze;
}

} //namespace logger
