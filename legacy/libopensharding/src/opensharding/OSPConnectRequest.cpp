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
#include <opensharding/OSPConnectRequest.h>

#include <opensharding/OSPMessage.h>
#include <opensharding/OSPOutputStream.h>

namespace opensharding {

OSPConnectRequest::OSPConnectRequest(string database, string user, string password, bool createStatement) {
    this->database = database;
    this->user = user;
    this->password = password;
    this->createStatement = createStatement;
}

OSPConnectRequest::~OSPConnectRequest() {
}

void OSPConnectRequest::write(OSPOutputStream *buffer) {
    buffer->writeString(1, database);
    buffer->writeString(2, user);
    buffer->writeString(3, password);
    buffer->writeInt(4, 1); // autocommit
    buffer->writeInt(5, 1); // use named pipes
    buffer->writeString(6, requestPipe);
    buffer->writeString(7, responsePipe);
    buffer->writeInt(8, createStatement ? 1 : 0);
}

unsigned int OSPConnectRequest::getEstimatedEncodingLength() {
    return database.length() + user.length() + password.length() + 16;
}


}
