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
#ifndef __OSPFileOutputStream_h__
#define __OSPFileOutputStream_h__

#include <opensharding/OSPOutputStream.h>
#include <opensharding/OSPByteBuffer.h>

#include <stdio.h>
#include <string>

#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace opensharding {


class OSPFileOutputStream : public OSPOutputStream {
private:
    int fd;

    // temp encoding buffers
    char *varIntBuffer;
    char *intBuffer;

    // main buffer
    unsigned int writeBufferSize;
    OSPByteBuffer *writeBuffer;

    static Logger &log;

    void writeFieldHeader(int fieldNum, int wireType);
    void writeBytesToFile(const char *buffer, unsigned int offset, unsigned int length);

public:
    OSPFileOutputStream(int fd, unsigned int writeBufferSize);
    ~OSPFileOutputStream();

    // write raw data
    void writeByte(char b);
    void writeShort(short i);
    void writeInt(int i);
    void writeVarInt(int i);
    void writeBytes(const char *buffer, unsigned int offset, unsigned int length);

    // write fields
    void writeShort(int fieldNum, short i);
    void writeInt(int fieldNum, int i);
    void writeString(int fieldNum, string s);
    void writeBytes(int fieldNum, const char *buffer, unsigned int offset, unsigned int length);

    void flush();
};

}

#endif // __OSPFileOutputStream_h__
