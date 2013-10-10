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
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPOutputStream.h>

namespace opensharding {

OSPConnectResponse::OSPConnectResponse() {
    connID = "";
    requestPipeFilename = "";
    responsePipeFilename = "";
    stmtID = 0;
}

OSPConnectResponse::~OSPConnectResponse() {
}

void OSPConnectResponse::write(OSPOutputStream *buffer) {
    // implementation not required - C does not encode responses
}

void OSPConnectResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {
    switch (fieldNum) {
        case 1:
            connID = string(buffer+offset, length);
            break;
        case 2:
            requestPipeFilename = string(buffer+offset, length);
            break;
        case 3:
            responsePipeFilename = string(buffer+offset, length);
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }
}

void OSPConnectResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 4:
            stmtID = value;
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }
}



}
