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
/*
 *  SocketSender.h
 *  cmessaging
 *
 *  Created by Andy Grove on 11/23/08.
 *  Copyright 2008 Prelude Innovations. All rights reserved.
 *
 */

#ifndef __OSPWinsock2TCPConnection_h__
#define __OSPWinsock2TCPConnection_h__

#define BUFFSIZE 64000

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <map>
#include <string>
#include <string.h>
#include <sys/types.h>

#include <WinSock2.h>

/*
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
*/

#include <logger/Logger.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPByteBuffer.h>

namespace opensharding {

/**
 * This class is NOT thread-safe. Each thread must have its own OSPWinsock2TCPConnection for now.
 */
class OSPWinsock2TCPConnection : public OSPConnection {
private:

    static logger::Logger &log;

    string host;
    int port;

    // the socket handle
    SOCKET hSocket;

    int requestID;

private:
    int getSocket(string host, int port);
    bool read(int hSocket, char *buf, int offset, int length);

    OSPMessage* readResponse(int, string, bool *);
    OSPMessage* readResponseMessage(int, bool *);

public:

    OSPWinsock2TCPConnection(string host, int port);
    ~OSPWinsock2TCPConnection();

    OSPMessage* sendMessage(OSPMessage*);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK, OSPMessageConsumer *);

    void stop();

};

}

#endif

