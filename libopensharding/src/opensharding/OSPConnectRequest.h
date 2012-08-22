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
#ifndef __OSPConnectRequest_h__
#define __OSPConnectRequest_h__

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPOutputStream.h>
#include <opensharding/OSPByteBuffer.h>

namespace opensharding {

class OSPConnectRequest : public OSPMessage {
private:
    string database;
    string user;
    string password;

    string requestPipe;
    string responsePipe;

    bool autocommit;
    bool createStatement;

public:
    OSPConnectRequest(string database, string user, string password, bool autocommit, bool createStatement);
    virtual ~OSPConnectRequest();

    unsigned char getMessageType() { return 0x01; }
    bool isAutoCommit() { return autocommit; }

    unsigned int getEstimatedEncodingLength();
    void write(OSPOutputStream *);

    // setField implementation not required for request messages since C only encodes them
    void setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {}
    void setField(int fieldNum, int value) {}

    void setRequestPipe(string requestPipe)   { this->requestPipe  = requestPipe; }
    void setResponsePipe(string responsePipe) { this->responsePipe = responsePipe; }
};

}

#endif // __OSPConnectRequest_h__
