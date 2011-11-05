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

//TODO increase this
#define FLUSH_LIMIT 0 //Lines to let into buffer before flushing.

namespace logger {

/*static*/ bool MyOSPLogger::init = false;

/*static*/ Logger &MyOSPLogger::getLogger(string name) {
	if(!init) {
		//Load configs into static map in super class Logger
		MyOSPLogger::configure(MyOSPConfig::getConfFile());
		init = true;
	}
	return Logger::getLogger(name);
}

} //namespace logger
