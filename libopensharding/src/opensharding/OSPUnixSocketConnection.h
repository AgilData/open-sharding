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
#ifndef __OSPUnixSocketConnection_h__
#define __OSPUnixSocketConnection_h__

#include <map>
#include <queue>
#include <stdio.h>

#include <opensharding/OSPConnectionInfo.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPFileInputStream.h>
#include <opensharding/OSPFileOutputStream.h>
#include <opensharding/OSPByteBuffer.h>
#include <logger/Logger.h>

namespace opensharding {

class OSPUnixSocketConnection : public OSPConnection {
private:

    int socketFD;
    FILE *socketFile;

    OSPFileInputStream *is;
    OSPFileOutputStream *os;

    int nextRequestID;

    char *buffer;
    unsigned int bufferSize;

    static logger::Logger &log;

private:
    int makeNonBlocking(int fd);
    int doSendOnly(OSPMessage *message, bool flush);


public:
    OSPUnixSocketConnection(OSPConnectionInfo *info);
    ~OSPUnixSocketConnection();
    
    void init(OSPConnectionInfo *info);

    OSPMessage* sendMessage(OSPMessage*);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK, OSPMessageConsumer *);

    int sendOnly(OSPMessage *message, bool flush);
    OSPMessage* waitForResponse();

    void stop();
};

} // namespace

#endif // __OSPUnixSocketConnection_h__
