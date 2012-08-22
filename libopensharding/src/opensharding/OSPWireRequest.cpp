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
#include <opensharding/OSPWireRequest.h>

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPInputStream.h>
#include <opensharding/OSPOutputStream.h>
#include <opensharding/OSPByteBuffer.h>

namespace opensharding {

OSPWireRequest::OSPWireRequest(int requestID, int messageType, OSPMessage *request) {
    this->requestID = requestID;
    this->messageType = messageType;
    this->request = request;
    this->ospProtocolVersion = 1; //current protocol version implemented in this driver
}

OSPWireRequest::~OSPWireRequest() {
}

void OSPWireRequest::write(OSPOutputStream *buffer) {

    //TODO: this is functional but very inefficient - see OSPUnixSocketConnection for better example of doing this

    buffer->writeInt(1, requestID);
    buffer->writeInt(2, messageType);
    buffer->writeInt(3, ospProtocolVersion);

    // encode request to temp buffer
    OSPByteBuffer temp(request->getEstimatedEncodingLength());
    request->write(&temp);

    // write encoded message
    buffer->writeBytes(99+messageType, temp.getBuffer(), 0, temp.getOffset());
}

unsigned int OSPWireRequest::getEstimatedEncodingLength() {
    return request->getEstimatedEncodingLength() + 64;
}

}
