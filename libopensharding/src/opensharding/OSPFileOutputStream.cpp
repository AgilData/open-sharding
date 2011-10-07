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
#include <stdio.h>
#include <string>

#include <opensharding/OSPFileOutputStream.h>
#include <opensharding/OSPByteBuffer.h>

#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace opensharding {

logger::Logger OSPFileOutputStream::log = Logger::getLogger("OSPFileOutputStream");

OSPFileOutputStream::OSPFileOutputStream(FILE *file, unsigned int writeBufferSize) {
    this->file = file;
    this->writeBufferSize = writeBufferSize;

    intBuffer = new char[4];
    varIntBuffer = new char[8];

    if (writeBufferSize==0) {
        writeBuffer = NULL;
    }
    else {
        //TODO: this is untested
        writeBuffer = new OSPByteBuffer(writeBufferSize);
    }
}

OSPFileOutputStream::~OSPFileOutputStream() {
    delete [] intBuffer;
    delete [] varIntBuffer;
    if (writeBuffer) {
        delete writeBuffer;
    }
}

void OSPFileOutputStream::writeVarInt(int n) {
    int offset = 0;
    while (n>0x7F) {
        if (offset>=7) {
            throw "ERROR";
        }
        varIntBuffer[offset++] = (n & 0x7F) | 0x80;
        n >>= 7;
    }
    varIntBuffer[offset++] = (n & 0x7F);
    writeBytes(varIntBuffer, 0, offset);
}

void OSPFileOutputStream::writeInt(int n) {
    intBuffer[3] = (char) (n); n = n >> 8;
    intBuffer[2] = (char) (n); n = n >> 8;
    intBuffer[1] = (char) (n); n = n >> 8;
    intBuffer[0] = (char) (n);
    writeBytes(intBuffer, 0, 4);
}

void OSPFileOutputStream::writeInt(int fieldNum, int n) {
    writeFieldHeader(fieldNum, 0);
    //log.trace("writeInt()");
    intBuffer[3] = (char) (n); n = n >> 8;
    intBuffer[2] = (char) (n); n = n >> 8;
    intBuffer[1] = (char) (n); n = n >> 8;
    intBuffer[0] = (char) (n);
    writeBytes(intBuffer, 0, 4);
}

void OSPFileOutputStream::writeString(int fieldNum, string s) {
    writeBytes(fieldNum, s.c_str(), 0, s.length());
}

void OSPFileOutputStream::writeBytes(int fieldNum, const char *buffer, unsigned int offset, unsigned int length) {
    writeFieldHeader(fieldNum, 2);
    writeVarInt(length);
    writeBytes(buffer, offset, length);
}

void OSPFileOutputStream::writeBytes(const char *buffer, unsigned int offset, unsigned int length) {

    if (!writeBuffer) {
        // write direct to file as we don't have a write buffer
        writeBytesToFile(buffer, offset, length);
        return;
    }

    // is the buffer full?
    //TODO: should be getOffset or getLength ?
    if  (writeBuffer->getOffset() + length > writeBufferSize) {

        // there isn't enough space in the buffer, so flush the current buffer to file, and reset it
        flush();

        if (length > writeBufferSize) {
            // the data is larger than the buffer, so write direct to file
            writeBytesToFile(buffer, offset, length);
        }
        else {
            // write to the buffer
            writeBuffer->writeBytes(buffer, offset, length);
        }
    }
    else {
        // write to the buffer
        writeBuffer->writeBytes(buffer, offset, length);
    }

}

void OSPFileOutputStream::writeBytesToFile(const char *buffer, unsigned int offset, unsigned int length) {
    //log.trace("writeBytes()");
    size_t n = fwrite(buffer+offset, sizeof(char), length, file);
    if (n!=length) {
        log.error("writeBytes() FAILED");
        throw "FAIL";
    }
}

void OSPFileOutputStream::writeFieldHeader(int fieldNum, int wireType) {
    //TODO: this is wrong - should write var int
    int n = fieldNum << 3 | wireType;
    intBuffer[3] = (char) (n); n = n >> 8;
    intBuffer[2] = (char) (n); n = n >> 8;
    intBuffer[1] = (char) (n); n = n >> 8;
    intBuffer[0] = (char) (n);
    writeBytes(intBuffer, 0, 4);
}

void OSPFileOutputStream::flush() {

    //TODO: should be getOffset or getLength ?

    if (writeBuffer && writeBuffer->getLength()>0) {
        writeBytesToFile(writeBuffer->getBuffer(), 0, writeBuffer->getLength());
        writeBuffer->reset();
    }

    //log.trace("flush()");
    fflush(file);
}

}
