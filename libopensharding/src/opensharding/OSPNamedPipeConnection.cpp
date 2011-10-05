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
#include <opensharding/OSPNamedPipeConnection.h>

#include <stdio.h>

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPFileOutputStream.h>
#include <opensharding/OSPMessageDecoder.h>
#include <opensharding/OSPWireRequest.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPByteBuffer.h>
#include <util/Util.h>

#define DEBUG log.isDebugEnabled()

using namespace util;

namespace opensharding {

logger::Logger OSPNamedPipeConnection::log = Logger::getLogger("OSPNamedPipeConnection");

OSPNamedPipeConnection::OSPNamedPipeConnection(string requestPipeFilename, string responsePipeFilename) {

    if (DEBUG) log.debug(string("Opening request pipe ") + requestPipeFilename);
    requestPipe  = fopen(requestPipeFilename.c_str(), "wb");
    if (!requestPipe) {
        log.error("Failed to open request pipe for writing");
    }

    if (DEBUG) log.debug(string("Opening response pipe ") + responsePipeFilename);
    responsePipe = fopen(responsePipeFilename.c_str(), "rb");
    if (!responsePipe) {
        log.error("Failed to open response pipe for reading");
    }

    if (DEBUG) log.debug("Creating pipe I/O streams");

    // blocking read
    this->is = new OSPFileInputStream(responsePipe, 0);

    // non-blocking read
    //this->is = new OSPFileInputStream(responsePipe, 8192); //TODO: re-instate this

    this->os = new OSPFileOutputStream(requestPipe, 0); //TODO: specify buffer size

    bufferSize = 8192;
    buffer = new char[bufferSize];
}

OSPNamedPipeConnection::~OSPNamedPipeConnection() {

}

OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message) {
    return sendMessage(message, false);
}

OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message,  bool expectACK) {
    return sendMessage(message, expectACK, NULL);
}

OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message,  bool expectACK, OSPMessageConsumer *consumer) {

    //TODO: re-instate request IDs

    OSPWireRequest request(1, message->getMessageType(), message);

    OSPByteBuffer tempBuffer(1024);  // TODO: could calculate message length first
    request.write(&tempBuffer);

    // wrap the message in the messaging protocol
    OSPByteBuffer zbuffer(tempBuffer.getOffset() + 4);

    // message length
    zbuffer.writeInt(tempBuffer.getOffset());

    // message bytes
    zbuffer.writeBytes(tempBuffer.getBuffer(), 0, tempBuffer.getOffset());

    // write to pipe
    if (DEBUG) log.debug("Writing request to request pipe");
    os->writeBytes((char *) zbuffer.getBuffer(), 0, zbuffer.getOffset());
    if (expectACK) {
        log.debug("Flushing request pipe");
        os->flush();
    }

    // read responses
    OSPWireResponse *response = NULL;

    while (true) {

        if (DEBUG) log.debug("BEFORE read message length from response pipe");

        unsigned int messageLength = is->readInt();

        if (DEBUG) log.debug(string("AFTER read message length from response pipe - messageLength is ") + Util::toString((int)messageLength));

        // make sure our read buffer is large enough
        if (messageLength > bufferSize) {
            delete [] buffer;
            bufferSize = messageLength + 1024;
            buffer = new char[bufferSize];
        }

        // it is perfectly OK to have 0-length messages
        if (messageLength>0) {

            if (DEBUG) log.debug("BEFORE readBytes() from response pipe");
            is->readBytes(buffer, 0, messageLength);
            if (DEBUG) log.debug("AFTER readBytes() from response pipe");

            OSPByteBuffer byteBuffer(buffer, messageLength);

            OSPMessageDecoder decoder;
            response = new OSPWireResponse();
            decoder.decode(response, &byteBuffer);

            //response->read(&byteBuffer);

            bool isFinalResponse = response->isFinalResponse();

            if (consumer) {
                consumer->processMessage(response);
                //NOTE: consumer will destroy the response object so don't access it after this
            }

            if (isFinalResponse) {
                break;
            }
        }
        else {
            log.error("messageLength was <= 0 so aborting");
            break;
        }

        if (DEBUG) {
            log.debug("That was not the final message so expecting more responses for this request ....");
        }
    }

    if (DEBUG) log.debug("DONE Reading message from response pipe");

    // if there is no consumer, then we simple return the final message (normally we expect only one message
    // if there is no consumer)
    return response;
}

void OSPNamedPipeConnection::stop() {
    if (DEBUG) log.debug("Closing pipes");
    fclose(requestPipe);
    fclose(responsePipe);
}

} // namespace
