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
#include <opensharding/OSPExecuteResponse.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPOutputStream.h>

namespace opensharding {

OSPExecuteResponse::OSPExecuteResponse() {
    resultSetID = 0;
    resultSetColumnCount = 0;
    rowsAffected = 0;
    generatedID = 0;
    sqlState = "00000";
    errorCode = 0;
    errorMessage = "";
}

OSPExecuteResponse::~OSPExecuteResponse() {
}

void OSPExecuteResponse::write(OSPOutputStream *buffer) {
    // no need to implement this in C client
}

void OSPExecuteResponse::setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length) {
    switch (fieldNum) {
        case 11:
            errorMessage = string(buffer+offset, length);
            break;
        case 12:
            sqlState = string(buffer+offset, length);
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }
}

void OSPExecuteResponse::setField(int fieldNum, int value) {
    switch (fieldNum) {
        case 1:
            resultSetID = value;
            break;
        case 2:
            resultSetColumnCount = value;
            break;
        case 3:
            rowsAffected = value;
            break;
        case 5:
            generatedID = value;
            break;
        case 10:
            errorCode = value;
            break;
        default:
            // always ignore fields we don't recognize
            break;
    }
}



}
