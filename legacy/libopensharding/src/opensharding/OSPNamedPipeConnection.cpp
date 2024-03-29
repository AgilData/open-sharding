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
#include <opensharding/OSPByteBuffer.h>
#include <opensharding/OSPConnectRequest.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPErrorResponse.h>
#include <util/Util.h>

#define DEBUG log.isDebugEnabled()

using namespace util;

namespace opensharding {

logger::Logger &OSPNamedPipeConnection::log = Logger::getLogger("OSPNamedPipeConnection");

OSPNamedPipeConnection::OSPNamedPipeConnection(OSPConnectionInfo *info, int pipeId){
	int initRetVal = init(info, pipeId);
	if (initRetVal != 0) {
	    log.error("Failed to create OSPNamedPipeConnection");
	    throw "Failed to create OSPNamedPipeConnection";
	}
}

OSPNamedPipeConnection::~OSPNamedPipeConnection() {
    stop();
}

int OSPNamedPipeConnection::init(OSPConnectionInfo *info, int pipeId)
{
    this->pipeId = pipeId;

	// construct filename for request pipe
	char lcpRequestPipeName[256];
    sprintf(lcpRequestPipeName,  "%s/mysqlosp_%d_%d_request.fifo",  P_tmpdir, getpid(), pipeId);
	this->requestPipeFilename  = lcpRequestPipeName;

	// construct filename for response pipe
	char lcpResponsePipeName[256];
    sprintf(lcpResponsePipeName, "%s/mysqlosp_%d_%d_response.fifo", P_tmpdir, getpid(), pipeId);
    this->responsePipeFilename = lcpResponsePipeName;

    m_fifosCreated = false;
    m_fifosOpened = false;

    requestPipe = NULL;
    responsePipe = NULL;

    is = NULL;
    os = NULL;

    bufferSize = 8192;
    buffer = new char[bufferSize];

    //m_thread=NULL;

    nextRequestID = 1;
    
    int npRetval = this->makeFifos();
	
    if (npRetval != OSPNP_SUCCESS) {
        log.error(string("Failed to create named pipes"));
        return -1;
    }

    // now use a TCP connection to tell DbsClient about this new named pipe and set up the server thread to process it
    OSPTCPConnection *ospTcpConn = new OSPTCPConnection(info->host, info->port==0 ? 4545 : info->port);
    OSPConnectRequest request("OSP_CONNECT", "OSP_CONNECT", "OSP_CONNECT", false); // magic values
    request.setRequestPipe(this->getRequestPipeFilename());
    request.setResponsePipe(this->getResponsePipeFilename());       
    OSPWireResponse* wireResponse = NULL;
    try {
        wireResponse = dynamic_cast<OSPWireResponse*>(ospTcpConn->sendMessage(&request, true));
        if (wireResponse->isErrorResponse()) {
            OSPErrorResponse* response = dynamic_cast<OSPErrorResponse*>(wireResponse->getResponse());
			log.error(string("OSP Error: ") + Util::toString(response->getErrorCode()) + string(": ") + response->getErrorMessage());
			delete wireResponse;
			ospTcpConn->stop();
			delete ospTcpConn;
            
			// important that we delete the pipes as the next attempt will try and create them again
			// now handled by OSPNamedPipeConnection
			//                        unlink(requestPipeName);
			//                        unlink(responsePipeName);
            
			return -1;
		}
	} catch (...) {
		log.error("OSP communication error - OSP process dead?");
        
		// important that we delete the pipes as the next attempt will try and create them again
		// now handled by OSPNamedPipeConnection
		//                    unlink(requestPipeName);
		//                    unlink(responsePipeName);
        
		return -1;
	}
	
    // now connect via named pipes
	OSPConnectResponse* response = dynamic_cast<OSPConnectResponse*>(wireResponse->getResponse());

    // log the returned info
	if (DEBUG) {
		log.debug("TCP Response RequestPipeFileName=" + response->getRequestPipeFilename() + ", ResponsePipeFilename=" + response->getResponsePipeFilename());
	}

	//TODO: we should check the returned info to make sure it is correct ... we currently just use it for debug output
    
	// delete the wire response now we have the info
	delete wireResponse;

	// close the temporary TCP connection
	ospTcpConn->stop();
	delete ospTcpConn;

	// now open the named pipes for read/write
	npRetval = this->openFifos();
    
	if (npRetval != OSPNP_SUCCESS) {
		log.error(string("Failed to open named pipes"));
		return -1;
	}

	// success
    return 0;
}


//////



int OSPNamedPipeConnection::makeFifos() {

    if (DEBUG) log.debug(string("Creating named pipe ") + requestPipeFilename);

    // delete first just in case there was an issue before
    unlink(requestPipeFilename.c_str());

    umask(0);
    if (0 != mkfifo(requestPipeFilename.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH)) {
        log.error(string("Failed to create named pipe '") + requestPipeFilename + string("'"));
        return OSPNP_CREATE_REQUEST_PIPE_ERROR;
    }

    if (DEBUG) log.debug(string("Creating named pipe ") + responsePipeFilename);

    // delete first just in case there was an issue before
    unlink(responsePipeFilename.c_str());

    umask(0);
    if (0 != mkfifo(responsePipeFilename.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH)) {
        log.error(string("Failed to create named pipe '") + responsePipeFilename + string("'"));

        // important that we delete the request pipe
        unlink(requestPipeFilename.c_str());

        return OSPNP_CREATE_RESPONSE_PIPE_ERROR;
    }

    this->m_fifosCreated = true;

    return OSPNP_SUCCESS;
}



int OSPNamedPipeConnection::makeNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
        log.error("Failed to set O_NONBLOCK");
        return -1;
    }
    return 0;
}


int OSPNamedPipeConnection::openFifos() {

    if (DEBUG) log.debug(string("Opening request pipe ") + requestPipeFilename);
    requestPipe  = fopen(requestPipeFilename.c_str(), "wb");
    if (!requestPipe) {
        log.error(string("Failed to open request pipe '") + requestPipeFilename + string("' for writing"));
        return OSPNP_OPEN_REQUEST_PIPE_ERROR;
    }
    int requestPipeFD = fileno(requestPipe);
    makeNonBlocking(requestPipeFD);

    if (DEBUG) log.debug(string("Opening response pipe ") + responsePipeFilename);
    responsePipe = fopen(responsePipeFilename.c_str(), "rb");
    if (!responsePipe) {
        log.error(string("Failed to open response pipe '") + responsePipeFilename + string("' for writing"));
        fclose(requestPipe);
        return OSPNP_OPEN_RESPONSE_PIPE_ERROR;
    }
    int responsePipeFD = fileno(responsePipe);
    makeNonBlocking(responsePipeFD);

    if (DEBUG) log.debug("Creating pipe I/O streams");

    // create buffered input stream
    this->is = new OSPFileInputStream(responsePipeFD, 4096);

    // TODO: we should be using a buffer here - not sure why we're not
    this->os = new OSPFileOutputStream(requestPipeFD, 0);

    this->m_fifosOpened = true;

//    if (m_threadedResponseFifo && (m_thread == NULL)) {
//        startResponseThread();
//    }

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

    unsigned int count = 0;
    while (true) {

        if (DEBUG) log.debug("BEFORE read message length from response pipe");

        // no background thread, wait for message directly
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



int OSPNamedPipeConnection::sendOnly(OSPMessage *message, bool flush) {
    return doSendOnly(message, flush);
}

int OSPNamedPipeConnection::doSendOnly(OSPMessage *message, bool flush) {


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

    os->writeByte(1);        // protocol
    os->writeByte(2);        // protocol version
    os->writeInt(requestID); // sequence number
    os->writeByte(1);        // final request message?
    os->writeByte(0);        // reserved
    os->writeShort(1); // message type for OSPWireRequest

    os->writeInt(messageLength);
    os->writeBytes((char *) tempBuffer.getBuffer(), 0, tempBuffer.getOffset());

    // flush the pipe if we are waiting for a response
    if (flush) {
        log.debug("Flushing request pipe");
        os->flush();
    }

    return requestID;
}

/**
 * Reads a single OSPWireResponse response message from the response pipe. Note that this method is not thread-safe
 * and does not need to be. It can be called directly from the application thread if we are using dedicated pipes
 * per database connection. If we are in multi-threaded mode, then a single thread will be calling this method to read
 * responses from the pipe and dispatch them to the waiting application threads.
 */
OSPMessage* OSPNamedPipeConnection::waitForResponse() {

    OSPWireResponse *response = NULL;

    if (DEBUG) log.debug("BEFORE read message length from response pipe");

    unsigned int protocol        = is->readByte();  // ignored
    unsigned int protocolVersion = is->readByte();  // ignored
    unsigned int sequenceNumber  = is->readInt();   // ignored
    unsigned int finalMessage    = is->readByte();  // ignored
    unsigned int reserverd       = is->readByte();  // ignored
    unsigned int messageTypeID   = is->readShort(); // ignored
    unsigned int messageLength   = is->readInt();

    if (DEBUG) log.debug(string("AFTER read message length from response pipe - messageLength is ") + Util::toString((int)messageLength));

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

        // create the wire respsone object
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


void OSPNamedPipeConnection::stop() {

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
