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
#ifndef __OSPResultSetRowResponse_h__
#define __OSPResultSetRowResponse_h__

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPByteBuffer.h>
#include <logger/Logger.h>

#include <list>
#include <string.h>
#include <vector>

using namespace std;
using namespace logger;

namespace opensharding {

class OSPResultSetRowResponse : public OSPMessage {
private:

    unsigned int       fieldIndex;
    OSPString        **currentRow;
    unsigned int      columnCount;
    vector<OSPString*> resultRows;
    
    static Logger &log;

public:
    OSPResultSetRowResponse();
    virtual ~OSPResultSetRowResponse();

    unsigned char getMessageType() { return 0xa7; }
    unsigned int getEstimatedEncodingLength() { return 0; }
    void          write(OSPOutputStream *);

    void setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length);
    void setField(int fieldNum, int value);

    unsigned int      getColumnCount() { return columnCount; }
    OSPString*       *getResultRow()  { return currentRow; }


};



}

#endif // __OSPResultSetRowResponse_h__
