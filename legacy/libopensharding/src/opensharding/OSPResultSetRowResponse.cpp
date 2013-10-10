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
#include <opensharding/OSPResultSetRowResponse.h>
#include <logger/Logger.h>
#include <util/Util.h>

using namespace logger;
using namespace util;

namespace opensharding {

/*static*/ Logger &OSPResultSetRowResponse::log = Logger::getLogger("OSPResultSetRowResponse");

OSPResultSetRowResponse::OSPResultSetRowResponse() {
    columnCount = 0;
    fieldIndex = 0;
    currentRow = NULL;
}

OSPResultSetRowResponse::~OSPResultSetRowResponse() {
    if (currentRow) {
        for (unsigned int i=0; i<columnCount; i++) {
            if (currentRow[i]) {
                delete currentRow[i];
                currentRow[i] = NULL;
            }
        }
        delete [] currentRow;
        currentRow = NULL;
    }
}

void OSPResultSetRowResponse::write(OSPOutputStream *buffer) {
    // no need to implement this in C client
}

//TODO: HACK: assumes fields arrive in order

void OSPResultSetRowResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {


    switch (fieldNum) {
        case 2:
        {
            if (!currentRow) {
                throw Util::createException("OSPResultSetRowResponse::setField() NULL currentRow");
            }

            if (fieldIndex >= columnCount) {
                throw Util::createException("OSPResultSetRowResponse::setField() fieldIndex too large");
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

            break;
        }
        default:
            // ignore new fields - we don't care
            break;
    }
}

void OSPResultSetRowResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 1:
            columnCount = (unsigned int) value;
            currentRow = new OSPString*[columnCount];
            for(unsigned int i=0; i<columnCount; i++) {
            	//initialize pointers to NULL to avoid uninitialized values.
            	currentRow[i]=NULL;
            }
            break;
        default:
            // ignore new fields - we don't care
            break;
    }
}


}
