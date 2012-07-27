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
#include <sys/types.h>
#include <sys/stat.h>

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

#define AVOID_COPY 0

using namespace util;

namespace opensharding {

logger::Logger &OSPNamedPipeConnection::log = Logger::getLogger("OSPNamedPipeConnection");



// threadedResponseFifo must be set to true for multithreaded apps
OSPNamedPipeConnection::OSPNamedPipeConnection(string requestPipeFilename, string responsePipeFilename, bool threadedResponseFifo) {

    this->requestPipeFilename  = requestPipeFilename;
    this->responsePipeFilename = responsePipeFilename;

    m_fifosCreated = false;
    m_fifosOpened = false;

    requestPipe = NULL;
    responsePipe = NULL;

    m_threadedResponseFifo = threadedResponseFifo;

    is = NULL;
    os = NULL;

    bufferSize = 8192;
    buffer = new char[bufferSize];

    m_thread=NULL;

    nextRequestID = 1;
}


OSPNamedPipeConnection::~OSPNamedPipeConnection() {
    this->stop();
}


int OSPNamedPipeConnection::makeFifos() {

    boost::unique_lock<boost::mutex> lock(m_mutex);

    if (DEBUG) log.debug(string("Creating FIFO ") + requestPipeFilename);

    // delete first just in case there was an issue before
    unlink(requestPipeFilename.c_str());

    umask(0);
    if (0 != mkfifo(requestPipeFilename.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH)) {
        return OSPNP_CREATE_REQUEST_PIPE_ERROR;
    }

    if (DEBUG) log.debug(string("Creating FIFO ") + responsePipeFilename);

    // delete first just in case there was an issue before
    unlink(responsePipeFilename.c_str());

    umask(0);
    if (0 != mkfifo(responsePipeFilename.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH)) {

        // important that we delete the request pipe
        unlink(requestPipeFilename.c_str());

        return OSPNP_CREATE_RESPONSE_PIPE_ERROR;
    }

    this->m_fifosCreated = true;

    return OSPNP_SUCCESS;
}



void OSPNamedPipeConnection::startResponseThread() {
    m_thread=new boost::thread(boost::ref(*this));
}



int OSPNamedPipeConnection::openFifos() {

    boost::unique_lock<boost::mutex> lock(m_mutex);

    if (DEBUG) log.debug(string("Opening request pipe ") + requestPipeFilename);
    requestPipe  = fopen(requestPipeFilename.c_str(), "wb");
    if (!requestPipe) {
        log.error("Failed to open request pipe for writing");
        return OSPNP_OPEN_REQUEST_PIPE_ERROR;
    }

    if (DEBUG) log.debug(string("Opening response pipe ") + responsePipeFilename);
    responsePipe = fopen(responsePipeFilename.c_str(), "rb");
    if (!responsePipe) {
        log.error("Failed to open response pipe for reading");
        fclose(requestPipe);
        return OSPNP_OPEN_RESPONSE_PIPE_ERROR;
    }

    if (DEBUG) log.debug("Creating pipe I/O streams");

    //TODO: we are not using a buffered output stream - this will cause excessive calls to the kernel i/o routines
    this->is = new OSPFileInputStream(responsePipe, 8192);

    //TODO: we are not using a buffered output stream - this will cause excessive calls to the kernel i/o routines
   //The out put stream encodes each message into a single byte array making it efficient 
    this->os = new OSPFileOutputStream(requestPipe, 0);

    this->m_fifosOpened = true;

    if (m_threadedResponseFifo && (m_thread == NULL)) {
        startResponseThread();
    }

    return OSPNP_SUCCESS;
}


OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message) {
    return sendMessage(message, false);
}


OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message,  bool expectACK) {
    return sendMessage(message, expectACK, NULL);
}


OSPMessage* OSPNamedPipeConnection::sendMessage(OSPMessage *message,  bool expectACK, OSPMessageConsumer *consumer) {

    int requestID = sendOnly(message, expectACK);

    // read responses
    OSPWireResponse *response = NULL;

    while (true) {

        if (DEBUG) log.debug("BEFORE read message length from response pipe");

        if (m_threadedResponseFifo) {
            boost::unique_lock<boost::mutex> lock(m_mutex);

            // if there is nothing in the queue that matches this messages request id, relinquish control to other threads
            while (!((m_responses.count(requestID) > 0) && (m_responses[requestID].size() > 0))) {
              m_cond.wait(lock);
            }

            // Grab the response (to return to the caller or send to the consumer), then remove queue entry
            response = m_responses[requestID].front();
            m_responses[requestID].pop();

            if (m_responses[requestID].size() == 0) {
                // If queue empty, remove map entry for that request id.
                // TODO: Probably best to move this out of the loop for efficiency.
                m_responses.erase(m_responses.find(requestID)); 
            }

            if (DEBUG) log.debug("sendMessage (threaded) got response, RequestID=" + Util::toString(requestID));

            m_cond.notify_all();
        }
        else {
            // no background thread, wait for message directly
            response = dynamic_cast<OSPWireResponse*>(waitForResponse());
            if (DEBUG) log.debug("sendMessage (non-threaded) got response, RequestID=" + Util::toString(requestID));
        }

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



int OSPNamedPipeConnection::sendOnly(OSPMessage *message, bool flush) {

    boost::unique_lock<boost::mutex> lock(m_send_mutex);

    // not likely to happen, but just to be safe
    if (nextRequestID >= OSPNP_MAX_REQUEST_ID) {
        nextRequestID = 1;
    }

    // allocate next request ID
    int requestID = nextRequestID++;

    if (DEBUG) log.debug("Sending message to resquest pipe, RequestID=" + Util::toString(requestID));

    OSPWireRequest request(requestID, message->getMessageType(), message);

    // encode the message into a temporary memory buffer
    OSPByteBuffer tempBuffer(message->getEstimatedEncodingLength());
    request.write(&tempBuffer);

    int messageLength = tempBuffer.getOffset();

#ifdef AVOID_COPY

    //TODO: OSPFileOutputStream is not implemented correctly yet

    os->writeInt(messageLength);
    os->writeBytes((char *) tempBuffer.getBuffer(), 0, tempBuffer.getOffset());

#else

    // wrap the message in the messaging protocol
    OSPByteBuffer zbuffer(tempBuffer.getOffset() + 4);

    // message length
    zbuffer.writeInt(messageLength);

    // message bytes
    zbuffer.writeBytes(tempBuffer.getBuffer(), 0, tempBuffer.getOffset());

    // write to pipe
    if (DEBUG) log.debug("Writing request to request pipe");
    os->writeBytes((char *) zbuffer.getBuffer(), 0, zbuffer.getOffset());

#endif

    // flush the pipe if we are waiting for a response
    if (flush) {
        log.debug("Flushing request pipe");
        os->flush();
    }

    return requestID;
}



OSPMessage* OSPNamedPipeConnection::waitForResponse() {

    OSPWireResponse *response = NULL;

    if (DEBUG) log.debug("BEFORE read message length from response pipe");

    unsigned int messageLength = is->readInt();

    if (DEBUG) log.debug(string("AFTER read message length from response pipe - messageLength is ") + Util::toString((int)messageLength));

// THREAD SAFE?  MAYBE SHOULD MAKE BUFFER LOCAL.  SEEMS LIKE ONLY USED BY ONE THREAD.
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

        OSPByteBuffer byteBuffer(buffer, messageLength);

        OSPMessageDecoder decoder;
        response = new OSPWireResponse();
        decoder.decode(response, &byteBuffer);

    if (DEBUG) log.debug("After reading message from response pipe, RequestID=" + Util::toString(response->getRequestID()));

        //response->read(&byteBuffer);
    }
    else {
        log.error("messageLength was <= 0");
    }

    if (DEBUG) log.debug("DONE Reading message from response pipe");

    return response;
}


void OSPNamedPipeConnection::operator () () {

    do
    {
        OSPWireResponse *response = NULL;

        if (DEBUG) log.debug("Background thread, waiting for response");

        response = dynamic_cast<OSPWireResponse*>(waitForResponse());

        if (DEBUG) log.debug("Background thread, got response");

        if (response != NULL) {
            boost::unique_lock<boost::mutex> lock(m_mutex);
 
            int requestID = response->getRequestID();

            if (DEBUG) log.debug("Background thread adding to map, RequestID=" + Util::toString(requestID));

            m_responses[requestID].push(response);

            // Let other threads get the response
            m_cond.notify_all();
//            m_cond.wait(lock);
        }
        else {
            if (DEBUG) log.debug("Background thread, NULL response");
        }
    } while(true);
}



void OSPNamedPipeConnection::stop() {

    if (m_threadedResponseFifo && (m_thread != NULL)) {
        m_thread->join();
        delete m_thread;
    }

    if (DEBUG) log.debug("Closing pipes");

    // TODO: Add flag to indicate to indicate shutdown to background thread
    if (requestPipe) {
        fclose(requestPipe);
        requestPipe = NULL;
    }

    if (responsePipe) {
        fclose(responsePipe);
        responsePipe = NULL;
    }

    // delete files
    unlink(requestPipeFilename.c_str());
    unlink(responsePipeFilename.c_str());

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
}


} // namespace
