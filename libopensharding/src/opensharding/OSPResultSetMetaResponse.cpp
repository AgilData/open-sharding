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
#include <opensharding/OSPResultSetMetaResponse.h>
#include <logger/Logger.h>

using namespace logger;

namespace opensharding {

/*static*/ Logger &OSPResultSetMetaResponse::log = Logger::getLogger("OSPResultSetMetaResponse");

OSPResultSetMetaResponse::OSPResultSetMetaResponse() {
    columnCount = 0;
    tableName = NULL;
    columnName = NULL;
    columnType = NULL;
    tableNameIndex = 0;
    columnNameIndex = 0;
    columnTypeIndex = 0;
}

OSPResultSetMetaResponse::~OSPResultSetMetaResponse() {
    if (tableName) {
		// delete tableName OSPStrings
        for (unsigned int i=0; i<tableNameIndex; i++) {
            delete tableName[i];
        }
        delete [] tableName;
    }
    if (columnName) {
        // delete columnName OSPStrings
        for (unsigned int i=0; i<columnNameIndex; i++) {
            delete columnName[i];
        }
        delete [] columnName;
        delete [] columnType;
    }
}

void OSPResultSetMetaResponse::write(OSPOutputStream *buffer) {
    // no need to implement this in C client
}

//TODO: HACK: assumes fields arrive in order

void OSPResultSetMetaResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {


    switch (fieldNum) {
        case 2:
        {
            // create copy of string data
            char *strdata = new char[length+1];
            memcpy(strdata, buffer+offset, length);
            strdata[length] = 0;
            columnName[columnNameIndex++] = new OSPString(strdata, 0, length, true);
            break;
        }
        case 5:
        {
            // create copy of string data
            char *strdata = new char[length+1];
            memcpy(strdata, buffer+offset, length);
            strdata[length] = 0;
            tableName[tableNameIndex++] = new OSPString(strdata, 0, length, true);
            break;
        }
        default:
            // ignore new fields - we don't care
            break;
    }
}

void OSPResultSetMetaResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 1:
            columnCount = (unsigned int) value;
            tableName = new OSPString*[columnCount];
            columnName = new OSPString*[columnCount];
            columnType = new int[columnCount];
            for(unsigned int i=0; i<columnCount; i++) {
            	//initialize pointers to NULL to avoid uninitialized values.
            	tableName[i]=NULL;
            	columnName[i]=NULL;
            	columnType[i] = 0;
            }
            break;
        case 3:
            columnType[columnTypeIndex++] = value;
            break;
        default:
            // ignore new fields - we don't care
            break;
    }
}


}
