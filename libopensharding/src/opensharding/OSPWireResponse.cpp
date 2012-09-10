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
#include <opensharding/OSPWireResponse.h>

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPInputStream.h>
#include <opensharding/OSPOutputStream.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPCreateStatementResponse.h>
#include <opensharding/OSPExecuteResponse.h>
#include <opensharding/OSPResultSetMetaResponse.h>
#include <opensharding/OSPResultSetRowResponse.h>
#include <opensharding/OSPDisconnectResponse.h>
#include <opensharding/OSPErrorResponse.h>
#include <opensharding/OSPMessageDecoder.h>

namespace opensharding {

OSPWireResponse::OSPWireResponse() {
    requestID = 0;
    messageType = 0;
    response = NULL;
    errorResponse = false;
    finalResponse = true;
}

OSPWireResponse::~OSPWireResponse() {
    if (response) {
        delete response;
    }
}

void OSPWireResponse::write(OSPOutputStream *buffer) {
    // implementation not required - C never encodes responses
}

void OSPWireResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {
    switch (fieldNum) {
        case 100:
            response = new OSPConnectResponse();
            break;
        case 101:
            response = new OSPCreateStatementResponse();
            break;
        case 102:
            response = new OSPExecuteResponse();
            break;
        case 103:
            response = new OSPResultSetMetaResponse();
            break;
        case 104:
            response = new OSPDisconnectResponse();
            break;
        case 109:
            response = new OSPResultSetRowResponse();
            break;
        case 200:
            response = new OSPErrorResponse();
            errorResponse = true;
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }

    // decode the response message
    if (response) {
        OSPByteBuffer temp(buffer+offset, length);
        OSPMessageDecoder decoder;
        decoder.decode(response, &temp);
    }

}

void OSPWireResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 1:
            requestID = value;
            break;
        case 2:
            messageType = value;
            break;
        case 3:
            finalResponse = value!=0;
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }
}



}
