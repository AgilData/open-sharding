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

#include <string>
#include <map>
#include <stdio.h>

#include <util/OSPConfig.h>

using namespace std;

namespace util {

class MyOSPConfig : public OSPConfig {
	friend class OSPConfig;
private:
	static bool shardAnalyze, shardAnalyzeInit;
	static FILE * shardAnalyzeLog;

public:
    MyOSPConfig();
    virtual ~MyOSPConfig();

	static string getConfFile();
	static FILE * getAnalyzeLogFile();
	static bool isShardAnalyze();
};

} //end namespace util
#endif /* CONFIG_H_ */
