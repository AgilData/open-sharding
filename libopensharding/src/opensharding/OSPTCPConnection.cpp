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
 *  SocketSender.cpp
 *  cmessaging
 *
 *  Created by Andy Grove on 11/23/08.
 *  Copyright 2008 Prelude Innovations. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>


#include <opensharding/OSPMessage.h>
#include <opensharding/OSPByteBuffer.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPWireRequest.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPMessageDecoder.h>

#include <util/Util.h>
#include <logger/Logger.h>

#define    TCP_NODELAY     1

using namespace util;

namespace opensharding {

logger::Logger &OSPTCPConnection::log = Logger::getLogger("OSPTCPConnection");

OSPTCPConnection::OSPTCPConnection(string host, int port) {
    this->host = host;
    this->port = port;

    requestID = 0;

    hSocket = 0;
}

OSPTCPConnection::~OSPTCPConnection() {
    this->stop();
}

void OSPTCPConnection::stop() {
    if (hSocket) {
        close(hSocket);
        hSocket = 0;
    }
}



int OSPTCPConnection::getSocket(string host, int port) {

    if (hSocket) {
        // already connected
        return hSocket;
    }

    string key = host + string(":") + Util::toString(port);

    if (log.isDebugEnabled()) {
        log.debug(string("connect() key=") + key);
    }

    struct hostent *he;
    if (log.isDebugEnabled()) {
        log.debug(("Opening new socket to ") + key);
    }

    if ((hSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        log.error(string("Failed to create socket; errno=") + Util::toString(errno));
        throw Util::createException("Failed to create socket");
    }

    struct timeval tv;
    tv.tv_sec = 600; // 10 minutes
    tv.tv_usec = 0;

    if (log.isDebugEnabled()) {
        log.debug("Setting socket timeout option");
    }

    if (setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)) {
        log.error(string("setsockopt() failed; errno=") + Util::toString(errno));
        throw Util::createException("setsockopt() failed");
    }

    int optval = 1;
    int optlen = sizeof(optval);
    if(setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        log.error(string("failed to set keep-alive on socket; errno=") + Util::toString(errno));
    }

    // set TCP_NODELAY so that bytes are sent immediately
    int tcpNoDelayFlag = 1;
    if (setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &tcpNoDelayFlag,  sizeof(int))) {
        log.error(string("Failed to set TCP_NODELAY; errno=") + Util::toString(errno));
    }

    if (log.isDebugEnabled()) {
        log.debug("Calling gethostbyname()");
    }

    he = gethostbyname(host.c_str());
    if (!he) {
        log.error(string("gethostbyname failed to resolve host ") + host + string("; h_errno=") + Util::toString(h_errno));
        throw Util::createException("gethostbyname failed");
    }

    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = *((struct in_addr *) he->h_addr);
    bzero(&(their_addr.sin_zero), 8);

    if (log.isDebugEnabled()) {
        log.debug("Connecting socket");
    }

    if (connect(hSocket, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        string msg = string("Failed to connect to ") + key + string("; errno=") + Util::toString(errno);
        log.error(msg);
        throw Util::createException("Failed to connect to socket");
    }

    return hSocket;
}

bool OSPTCPConnection::read(int hSocket, char *buf, int offset, int length) {
    int bytesRead = 0;
    while (bytesRead < length) {
        int n = recv(hSocket, buf+offset+bytesRead, length-bytesRead, 0);
        if (n>0) {
            if (log.isDebugEnabled()) {
                log.debug(string("Read ") + Util::toString(n) + " byte(s) OK");
            }
            bytesRead += n;
        }
        else if (n==-1) {
            log.debug("Error reading from socket (EOF)");
            return false;
        }
        else if (n==0) {
            log.debug("Error reading from socket (0 bytes)");
            return false;
        }
    }
    return true;
}

OSPMessage* OSPTCPConnection::sendMessage(OSPMessage *message) {
    return sendMessage(message, false, NULL);
}

OSPMessage* OSPTCPConnection::sendMessage(OSPMessage *message, bool expectACK) {
    return sendMessage(message, expectACK, NULL);
}

OSPMessage* OSPTCPConnection::sendMessage(OSPMessage *message, bool expectACK, OSPMessageConsumer *messageConsumer) {

    // increment sequence number (request ID)
    requestID++;

    // create wire request
    OSPWireRequest request(requestID, message->getMessageType(), message);

    // encode wire request
    OSPByteBuffer tempBuffer(1024);
    request.write(&tempBuffer);

    // wrap the message in the messaging protocol
    OSPByteBuffer buffer(tempBuffer.getOffset() + 10);

    char messageHeader[4];
    messageHeader[0] = 1; // protocol
    messageHeader[1] = 1; // version
    messageHeader[2] = 1; // final request message
    messageHeader[3] = 0;

    buffer.writeBytes(messageHeader, 0, 4); // message header
    buffer.writeShort(message->getMessageType()); // message type
    buffer.writeInt(tempBuffer.getOffset()); // message length

    // message bytes
    buffer.writeBytes(tempBuffer.getBuffer(), 0, tempBuffer.getOffset());

    string key = string(host) + string(":") + Util::toString(port);

    if (log.isDebugEnabled()) {
        log.debug(string("sendMessage() to ") + key + string("; expectACK=") + (expectACK?"true":"false"));
    }

    if (log.isDebugEnabled()) {
        log.debug("SocketSender::sendMessage() BEGIN");
    }

    string error = "";
    bool sendOK = false;

    getSocket(host, port);
    if (hSocket<=0) {
        log.error("Failed to send message - could not establish connection");
        throw Util::createException("sendfailed");
    }

    if (log.isDebugEnabled()) {
        log.debug(string("sendMessage() before send()"));
    }

#ifdef __APPLE__
    int b = send(hSocket, (char *) buffer.getBuffer(), buffer.getOffset(), SO_NOSIGPIPE);
#else
    int b = send(hSocket, (char *) buffer.getBuffer(), buffer.getOffset(), MSG_NOSIGNAL);
#endif
    if (log.isDebugEnabled()) {
        log.debug(string("sendMessage() sent ") + Util::toString(b) + string(" bytes"));
    }

    if (b == -1) {
        error = string("Error sending message - send() returned -1; errno=") + Util::toString(errno);
    }
    else if (b != (int) buffer.getOffset()) {

        // failed to write all the bytes - this probably means the connection got closed at the other end
        error = string("Error sending message - send() returned wrong byte count (expected=")
                    + Util::toString((int) buffer.getOffset())
                    + string("; actual=") + Util::toString(b) + string(")");



        // close the socket and remove from map
        log.debug(string("Closing socket ") + key);
        close(hSocket);
        hSocket = 0;
    }
    else {
        sendOK = true;
        error = "";
    }

    if (!sendOK) {
        log.error(string("Failed to send message due to error: ") + error);
        throw Util::createException("sendfailed");
    }

    if (expectACK) {
        bool finalMessage;
        if (messageConsumer) {
            // expect multiple responses
            finalMessage = false;
            while (!finalMessage) {
                OSPMessage *message = readResponse(hSocket, key, &finalMessage);
                messageConsumer->processMessage(message);
            }
            return NULL;
        }
        else {
            // expect single response
            if (log.isDebugEnabled()) {
                log.debug("Expecting single response");
            }
            OSPMessage *message = readResponse(hSocket, key, &finalMessage);
            if (!finalMessage) {
                log.error("Expected a single response, but this was not the final message");
            }
            return message;
        }
    }
    else {
        if (log.isDebugEnabled()) {
            log.debug("Expecting no response");
        }
        return NULL;
    }
}


OSPMessage* OSPTCPConnection::readResponse(int hSocket, string key, bool *finalMessage) {

    if (log.isDebugEnabled()) {
        log.debug("readResponse()");
    }

    // default
    *finalMessage = true;

    string error = "";

    bool protocolError = false;

    OSPMessage* ret = readResponseMessage(hSocket, finalMessage);

    if (ret) {
        // valid response
        return ret;
    }
    else {
        error = "Failed to read response message (see earlier error messages for more info)";
        protocolError = true;
    }

    if (error != "") {

        // log error showing server host:port too
        log.error(string(error) + string(" (") + key + string(")"));

        if (protocolError) {
            // close the socket and remove from map
            log.warn(string("Closing socket ") + key + string(" after protocol error"));
            close(hSocket);
            hSocket = 0;
        }

        throw Util::createException("FAIL_ACK");
    }

    if (log.isDebugEnabled()) {
        log.debug("readResponse() END");
    }

    return NULL;
}

OSPMessage* OSPTCPConnection::readResponseMessage(int hSocket, bool *finalMessage) {

    if (log.isDebugEnabled()) {
        log.debug("readResponseMessage()");
    }

    //TODO: make these class-level buffers? does this class need to be thread-safe?
    char header[10];

    // read header and message length
    if (!read(hSocket, header, 0, 10)) {
        log.error("Failed to read message header");
        return NULL;
    }

    // first 6 bytes of header are ignored for now

    int messageBytes = OSPByteBuffer::readInt(header, 6);
    if (messageBytes<0 || messageBytes>1*1024*1024) {
        log.error(string("Invalid response messageLength: ") + Util::toString((int)messageBytes));
        return NULL;
    }

    char messageData[messageBytes];

    if (!read(hSocket, messageData, 0, messageBytes)) {
        log.error("Failed to read response body");
        return NULL;
    }

    // decode the message
    OSPWireResponse *message = new OSPWireResponse();

    OSPByteBuffer bb(messageData, messageBytes);

    OSPMessageDecoder decoder;
    decoder.decode(message, &bb);

    *finalMessage = message->isFinalResponse();

    //message->read(&bb);

    if (log.isDebugEnabled()) {
        log.debug("readResponseMessage() returning valid message");
    }

    return message;

}

} // namespace
