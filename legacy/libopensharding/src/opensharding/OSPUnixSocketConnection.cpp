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
#include <opensharding/OSPUnixSocketConnection.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPFileOutputStream.h>
#include <opensharding/OSPMessageDecoder.h>
#include <opensharding/OSPWireRequest.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPByteBuffer.h>
#include <opensharding/OSPByteBuffer.h>
#include <opensharding/OSPConnectRequest.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPErrorResponse.h>
#include <util/Util.h>

#define DEBUG log.isDebugEnabled()

using namespace util;

namespace opensharding {

logger::Logger &OSPUnixSocketConnection::log = Logger::getLogger("OSPUnixSocketConnection");

OSPUnixSocketConnection::OSPUnixSocketConnection(OSPConnectionInfo *info) {
    requestBuffer = NULL;
    requestBuffer2 = NULL;
    buffer = NULL;
	init(info);
}

OSPUnixSocketConnection::~OSPUnixSocketConnection() {
    stop();
}

void OSPUnixSocketConnection::init(OSPConnectionInfo *info)
{
    // open socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log.error(string("ERROR opening unix domain socket"));
        perror("ERROR opening unix domain socket");
        throw Util::createException("ERROR opening unix domain socket");
    }

    // bind socket
    struct sockaddr_un my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, info->socket_file.c_str(), sizeof(my_addr.sun_path) - 1);

    // connect
    if (connect(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        log.error(string("ERROR connecting to unix socket: ") + info->socket_file);
        perror("ERROR connecting to unix socket");
        throw Util::createException("ERROR connecting to unix socket");
    }

    is = new OSPFileInputStream(sockfd, 4096);
    os = new OSPFileOutputStream(sockfd, 0);

    requestBuffer = new OSPByteBuffer(8192);

    requestBuffer2 = new OSPByteBuffer(8192);

    bufferSize = 8192;
    buffer = new char[bufferSize];

    nextRequestID = 1;
}


int OSPUnixSocketConnection::makeNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
        log.error("Failed to set O_NONBLOCK");
        return -1;
    }
    return 0;
}


OSPMessage* OSPUnixSocketConnection::sendMessage(OSPMessage *message) {
    return sendMessage(message, false);
}


OSPMessage* OSPUnixSocketConnection::sendMessage(OSPMessage *message,  bool expectACK) {
    return sendMessage(message, expectACK, NULL);
}


OSPMessage* OSPUnixSocketConnection::sendMessage(OSPMessage *message,  bool expectACK, OSPMessageConsumer *consumer) {

    int requestID = sendOnly(message, expectACK);

    // read responses
    OSPWireResponse *response = NULL;

    unsigned int count = 0;
    while (true) {

        if (DEBUG) log.debug("BEFORE read response");

        response = dynamic_cast<OSPWireResponse*>(waitForResponse());

        if (response->getRequestID() != requestID) {
            log.error(
                    "Received response #" + Util::toString(response->getRequestID()) +
                    " for request #" + Util::toString(requestID)
            );
            response = NULL;
        }

        count++;
        if (DEBUG) log.debug(string("sendMessage (non-threaded) got response number ") + Util::toString(count) + string("; RequestID=") + Util::toString(requestID));

        if (response == NULL) {
            break;
        }

        bool isFinalResponse = response->isFinalResponse();

        if (consumer) {
            consumer->processMessage(response);
            //NOTE: consumer will destroy the response object so don't access it after this
        }

        if (isFinalResponse) {
            if (DEBUG) {
                log.debug("Received final response.");
            }

            break;
        }

        if (DEBUG) {
            log.debug("That was not the final message so expecting more responses for this request ....");
        }
    } // while

    if (DEBUG) log.debug("DONE Reading message(s) from response pipe");

    // if there is no consumer, then we simply return the final message (there should only
    // be one message when there is no consumer)
    return response;

}


int OSPUnixSocketConnection::sendOnly(OSPMessage *message, bool flush) {
    return doSendOnly(message, flush);
}

int OSPUnixSocketConnection::doSendOnly(OSPMessage *message, bool flush) {

    // not likely to happen, but just to be safe
    if (nextRequestID >= 2000000) {
        nextRequestID = 1;
    }

    // allocate next request ID
    int requestID = nextRequestID++;
    int messageType = message->getMessageType();

    if (DEBUG) log.debug("Sending message to resquest pipe, RequestID=" + Util::toString(requestID));

    // encode the message into the first temporary memory buffer
    requestBuffer->reset();
    message->write(requestBuffer);

    // now use a second buffer to encode the OSPWireRequest

    if (DEBUG) log.debug("BEFORE WRITE HEADER");

    // first the OSP protocol
    requestBuffer2->reset();
    requestBuffer2->writeByte(1);        // protocol
    requestBuffer2->writeByte(2);        // protocol version
    requestBuffer2->writeInt(requestID); // sequence number
    requestBuffer2->writeByte(1);        // final request message?
    requestBuffer2->writeByte(0);        // reserved
    requestBuffer2->writeShort(1); // message type for OSPWireRequest
    requestBuffer2->writeInt(0); // placeholder for message length

    if (DEBUG) log.debug(string("AFTER WRITE HEADER"));
    requestBuffer2->dump();

    // now the OSPWireRequest
    requestBuffer2->writeInt(1, requestID);
    requestBuffer2->writeInt(2, messageType);
    requestBuffer2->writeBytes(99+messageType, requestBuffer->getBuffer(), 0, requestBuffer->getOffset()); // this is the encoded request from the first buffer
    int messageLength = requestBuffer2->getOffset() - 14;

    if (DEBUG) log.debug(string("AFTER WRITE MESSAGE"));
    requestBuffer2->dump();

    // re-write header info
    requestBuffer2->reset();
    requestBuffer2->skip(10);
    requestBuffer2->writeInt(messageLength); // message length of OSPWireRequest

    if (DEBUG) log.debug(string("AFTER RE-WRITE HEADER WITH MESSAGE LENGTH"));
    requestBuffer2->dump();

    // write to output stream
    if (DEBUG) log.debug(string("Writing request to unix socket. Message length = ") + Util::toString(messageLength) + " + 14 byte header");
    requestBuffer2->dump();

    os->writeBytes((char *) requestBuffer2->getBuffer(), 0, messageLength + 14);

    // flush the pipe if we are waiting for a response
    if (flush) {
        if (DEBUG) log.debug("Flushing request");
        os->flush();
    }
    else {
        if (DEBUG) log.debug("NOT flushing request");
    }

    return requestID;
}

/**
 * Reads a single OSPWireResponse response message from the response pipe. Note that this method is not thread-safe
 * and does not need to be. It can be called directly from the application thread if we are using dedicated pipes
 * per database connection. If we are in multi-threaded mode, then a single thread will be calling this method to read
 * responses from the pipe and dispatch them to the waiting application threads.
 */
OSPMessage* OSPUnixSocketConnection::waitForResponse() {

    OSPWireResponse *response = NULL;

    if (DEBUG) log.debug("BEFORE read message header from unix socket");

    char messageHeader[14];

    is->readBytes(messageHeader, 0, 14);

    // this field isn't currently used
    bool finalMessage = messageHeader[6] != 0;

    // this field isn't currently used
    unsigned int sequenceNumber = OSPByteBuffer::readShort(messageHeader, 2);

    // this field isn't currently used
    unsigned int messageTypeID = OSPByteBuffer::readShort(messageHeader, 8);

    // this field isn't currently used
    unsigned int messageLength = OSPByteBuffer::readInt(messageHeader, 10);

    if (DEBUG) log.debug(string("AFTER read message header from response pipe - messageLength is ") + Util::toString((int)messageLength));

    // make sure our read buffer is large enough
    if (messageLength > bufferSize) {
        delete [] buffer;
        bufferSize = messageLength + 1024;
        buffer = new char[bufferSize];
    }

    // it is perfectly OK to have 0-length messages
    if (messageLength > 0) {

        if (DEBUG) log.debug("BEFORE readBytes() from response pipe");
        is->readBytes(buffer, 0, messageLength);
        if (DEBUG) log.debug("AFTER readBytes() from response pipe");

        /*
        if (TRACE) {
            Util::dump(buffer, messageLength, 80);
        }
        */

        // create byte buffer to wrap the existing buffer (NOTE: this does not perform a memcpy)
        OSPByteBuffer byteBuffer(buffer, messageLength);

        // create the wire response object
        response = new OSPWireResponse();
        response->setRequestID(sequenceNumber);

        // decode the data from the buffer into the newly created object
        OSPMessageDecoder decoder;
        decoder.decode(response, &byteBuffer);

    }
    else {
        log.error("messageLength was <= 0");
    }

    if (DEBUG) log.debug("DONE Reading message from response pipe");

    return response;
}


void OSPUnixSocketConnection::stop() {

    if (DEBUG) log.debug("Closing pipes");

    if (requestBuffer) {
        delete requestBuffer;
        requestBuffer = NULL;
    }

    if (requestBuffer2) {
        delete requestBuffer2;
        requestBuffer2 = NULL;
    }

    if (buffer) {
        delete [] buffer;
        buffer = NULL;
    }

    if (is) {
        delete is;
        is = NULL;
    }

    if (os) {
        delete os;
        os = NULL;
    }

    close(sockfd);
    sockfd = 0;
}


} // namespace
