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

#ifndef MYOSPCONFIG_H_
#define MYOSPCONFIG_H_

#define ANALYZE_LOG_OUTPUT_STDERR 1
#define ANALYZE_LOG_OUTPUT_STDOUT 2
#define ANALYZE_LOG_OUTPUT_SYSLOG 3
#define ANALYZE_LOG_OUTPUT_FILE 4

#define OSP_CLIENT_HOST_LIST "osp.client.host.list"

#include <string>
#include <map>
#include <stdio.h>

#include <util/OSPConfig.h>

using namespace std;

namespace util {

class MyOSPConfig : public OSPConfig {
	friend class OSPConfig;
private:
	static int shardAnalyze;
	static bool shardAnalyzeInit;
	static FILE * shardAnalyzeLog;

public:
    MyOSPConfig();
    virtual ~MyOSPConfig();

	static string getConfFile();
	static FILE * getAnalyzeLogFile();
	static int isShardAnalyze();

    static string   getStringProperty(string name);
    static int      getIntProperty(string name, int defaultValue);
    static bool     getBoolProperty(string name, bool defaultValue);
};

} //end namespace util
#endif /* CONFIG_H_ */
