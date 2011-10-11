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
#ifndef __OSPByteBuffer_h__
#define __OSPByteBuffer_h__

#include <string>

#include <opensharding/OSPInputStream.h>
#include <opensharding/OSPOutputStream.h>
#include <opensharding/OSPString.h>
#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace opensharding {

class OSPByteBuffer : public OSPInputStream, public OSPOutputStream {
private:

    static Logger &log;

    bool deleteBuffer;
    char *buffer;
    unsigned int offset;
    unsigned int length;

    void ensureCapacity(int n);

public:

    OSPByteBuffer(unsigned int length);
    OSPByteBuffer(char *buffer, unsigned int length);
    ~OSPByteBuffer();

    void reset() { offset=0; }

    unsigned int getOffset() { return offset; }

    void skip(unsigned int n);

    char readByte();
    void readBytes(char *dest, unsigned int offset, unsigned int length);
    int readVarInt();
    int readInt();
    short readShort();
    long readLong();
    OSPString *readOSPString();
    string readString();
    string readString(int bytesForLength);
    OSPString *readOSPString(int bytesForLength);

    // OSPOutputStream
    void writeVarInt(int i);
    void writeInt(int fieldNum, int i);
    void writeString(int fieldNum, string s);
    void writeBytes(int fieldNum, const char *buffer, unsigned int offset, unsigned int length);

    void flush();

    void writeByte(char byte);
    void writeBytes(const char *data, unsigned int offset, unsigned int length);
    void writeShort(short n);
    void writeInt(int n);
    void writeLong(long n);
    void writeString(const char *data, int bytesForLength);
    void writeString(string);

    char *getBuffer();
    unsigned int getLength();

    void flip();

    void display();

    // STATIC METHODS
    static short readShort(char *data, int offset);
    static int readInt(char *data, int offset);
    static long readLong(char *data, int offset);

    static void writeShort(char *data, int offset, short n);
    static void writeInt(char *data, int offset, int n);
    static void writeLong(char *data, int offset, long n);



};

}

#endif
