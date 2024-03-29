/* Copyright (C) 2010-2011 CodeFutures Corporation

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


#include <stdio.h>
#include <iostream>
#include <fstream>

#include <logger/Logger.h>

#include <mysql/MySQLAbstractConnection.h>

using namespace logger;

Logger &MySQLAbstractConnection::_log = Logger::getLogger("MySQLAbstractConnection");


MySQLAbstractConnection::MySQLAbstractConnection() {
}

MySQLAbstractConnection::~MySQLAbstractConnection() {
}

int MySQLAbstractConnection::notImplementedInt(const char *s) {
    if (_log.isTraceEnabled()) {
        _log.trace(string("Call to unimplemented method ") + string(s));
    }
    return -1;
}

void *MySQLAbstractConnection::notImplementedPtr(const char *s) {
    if (_log.isTraceEnabled()) {
        _log.trace(string("Call to unimplemented method ") + string(s));
    }
    return NULL;
}

void MySQLAbstractConnection::setStale()
{

}
