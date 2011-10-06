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
#ifndef __OSPMessage_h__
#define __OSPMessage_h__

#include <string.h>

#include <opensharding/OSPInputStream.h>
#include <opensharding/OSPOutputStream.h>

using namespace std;

namespace opensharding {

class OSPMessage {
public:
    OSPMessage();
    virtual ~OSPMessage();
    virtual unsigned char getMessageType()=0;

    // set field (called from OSPMessageDecoder)
    virtual void setField(int fieldNum, char *buffer, unsigned int offset, unsigned int length)=0;
    virtual void setField(int fieldNum, int value)=0;

    // encode message
    virtual void write(OSPOutputStream*)=0;

    // estimate the length of the encoded message (this is used so that an appropriately-sized temporary
    // encoding buffer can be created, avoiding expensive buffer resize operations)
    virtual unsigned int getEstimatedEncodingLength()=0;
};

}

#endif // __OSPMessage_h__
