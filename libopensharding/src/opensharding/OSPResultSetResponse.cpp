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
#include <opensharding/OSPResultSetResponse.h>
#include <logger/Logger.h>

using namespace logger;

namespace opensharding {

/*static*/ Logger &OSPResultSetResponse::log = Logger::getLogger("OSPResultSetResponse");

OSPResultSetResponse::OSPResultSetResponse() {
    columnCount = 0;
    columnName = NULL;
    columnType = NULL;
    columnNameIndex = 0;
    columnTypeIndex = 0;
    fieldIndex = 0;
    currentRow = NULL;
}

OSPResultSetResponse::~OSPResultSetResponse() {
    if (columnName) {
        // delete columnName OSPStrings
        for (unsigned int i=0; i<columnNameIndex; i++) {
            delete columnName[i];
        }
        delete [] columnName;
        delete [] columnType;
    }
    list<OSPString**>::iterator it;
    for (it=resultRows.begin(); it!=resultRows.end(); it++) {
        OSPString **row = *it;
        for (unsigned int i=0; i<columnCount; i++) {
            if (row[i]) {
                delete row[i];
            }
        }
        delete [] row;
    }
}

void OSPResultSetResponse::write(OSPOutputStream *buffer) {
    // no need to implement this in C client
}

//TODO: HACK: assumes fields arrive in order

void OSPResultSetResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {


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
        case 4:
        {
            if (!currentRow) {
                currentRow = new OSPString*[columnCount];
            }

            //TODO: gross HACK magic value for NULL strings
            if (length==10
                && buffer[offset+0]=='_'
                && buffer[offset+1]=='!'
                && buffer[offset+2]=='_'
                && buffer[offset+3]=='N'
                && buffer[offset+4]=='U'
                && buffer[offset+5]=='L'
                && buffer[offset+6]=='L'
                && buffer[offset+7]=='_'
                && buffer[offset+8]=='!'
                && buffer[offset+9]=='_')
            {
                currentRow[fieldIndex++] = NULL;
            }
            else {

                // create copy of string data
                char *strdata = new char[length+1];
                memcpy(strdata, buffer+offset, length);
                strdata[length] = 0;

                // store this field
                currentRow[fieldIndex++] = new OSPString(strdata, 0, length, true);
            }

            if (fieldIndex == columnCount) {
                resultRows.push_back(currentRow);
                currentRow = NULL;
                fieldIndex = 0;
            }
            break;
        }
        default:
            throw "OSPResultSetResponse::setField() invalid fieldNum";
    }
}

void OSPResultSetResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 1:
            columnCount = (unsigned int) value;
            columnName = new OSPString*[columnCount];
            columnType = new int[columnCount];
            break;
        case 3:
            columnType[columnTypeIndex++] = value;
            break;
        default:
            throw "OSPResultSetResponse::setField() invalid fieldNum";
    }
}


}
