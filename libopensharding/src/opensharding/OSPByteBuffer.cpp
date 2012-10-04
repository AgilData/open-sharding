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
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>

#include <opensharding/OSPByteBuffer.h>

#include <logger/Logger.h>
#include <util/Util.h>

using namespace std;
using namespace util;

namespace opensharding {

logger::Logger &OSPByteBuffer::log = Logger::getLogger("OSPByteBuffer");

OSPByteBuffer::OSPByteBuffer(unsigned int _length) {

    length = _length;

    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "Allocating %u bytes\n", length);
        log.debug(msg);
    }

    buffer = new char[length];
    memset(buffer, 0, length);
    offset = 0;

    deleteBuffer = true;
}

OSPByteBuffer::OSPByteBuffer(char *_buffer, unsigned int _length) {
    buffer = _buffer;
    length = _length;
    deleteBuffer = false;

    if (log.isDebugEnabled()) {
        log.debug("Created OSPByteBuffer from existing buffer");
    }
}

OSPByteBuffer::~OSPByteBuffer() {
    if (deleteBuffer) {
        delete [] buffer;
    }
}

void OSPByteBuffer::flip() { // same as reset()
    offset = 0;
}

void OSPByteBuffer::skip(unsigned int n) {
    // validation checks for offset, length
    if (offset+n > length) {
        log.error("attempt to read past end of buffer in skip()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        log.debug("skip()");
    }

    offset += n;
}

char OSPByteBuffer::readByte() {
    // validation checks for offset, length
    if (offset+1 > length) {
        log.error("attempt to read past end of buffer in readByte()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        log.debug("readByte()");
    }

    return buffer[offset++];
}

void OSPByteBuffer::readBytes(char *dest, unsigned int index, unsigned int numBytes) {
    // validation checks for offset, length
    if (offset+numBytes > length) {
        log.error("attempt to read past end of buffer in readBytes()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        char msg[128];
        sprintf(msg, "readBytes(%i); offset=%u, length=%u", numBytes, offset, length);
        log.debug(msg);
    }

    memcpy(dest+index, buffer+offset, numBytes);

    if (log.isDebugEnabled()) {
        printf("readBytes() returned {");
        for (unsigned int i=0; i<numBytes; i++) {
            printf("%i ", (int) (0xFF & dest[index+i]));
        }
        printf("}\n");
    }

    offset+=numBytes;
}

int OSPByteBuffer::readVarInt() {

    //log.trace("readVarInt()");

    // validation checks for offset, length
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }

    int b;
    int result;

    b = (buffer[offset++]); result  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }
    b = (buffer[offset++]); result |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }
    b = (buffer[offset++]); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }
    b = (buffer[offset++]); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }
    b = (buffer[offset++]); result |=  b         << 28; if (!(b & 0x80)) goto done;
    if (offset+1 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readVarInt()");
    }

    //TODO: handle this
    // If the input is larger than 32 bits, we still need to read it all
    // and discard the high-order bits.
//    for (int i = 0; i < kMaxVarintBytes - kMaxVarint32Bytes; i++) {
//    b = *(ptr++); if (!(b & 0x80)) goto done;
//    }

    // We have overrun the maximum size of a varint (10 bytes).  Assume
    // the data is corrupt.
    throw Util::createException("FAILED_TO_DECODE_VARINT");

done:
    //log.info(string("readVarInt() returning ") + Util::toString(result));
    return result;
}

int OSPByteBuffer::readInt() {
    // validation checks for offset, length
    if (offset+4 > length) {
        throw Util::createException("attempt to read past end of buffer in OSPByteBuffer::readInt()");
    }

    if (log.isDebugEnabled()) {
        log.debug("readInt()");
    }
    int ret = OSPByteBuffer::readInt(buffer, offset);
    offset += 4;

    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "readInt() returned %i", ret);
        log.debug(msg);
    }

    return ret;
}

short OSPByteBuffer::readShort() {
    // validation checks for offset, length
    if (offset+2 > length) {
        log.error("attempt to read past end of buffer in readShort()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        log.debug("readShort()");
    }

    short ret = OSPByteBuffer::readShort(buffer, offset);
    offset += 2;

    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "readShort() returned %i", ret);
        log.debug(msg);
    }

    return ret;
}

long OSPByteBuffer::readLong() {
    // validation checks for offset, length
    if (offset+8 > length) {
        log.error("attempt to read past end of buffer in readLong()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        log.debug("readLong()");
    }

    long ret = OSPByteBuffer::readLong(buffer, offset);
    offset += 8;

    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "readLong() returned %li", ret);
        log.debug(msg);
    }

    return ret;
}

string OSPByteBuffer::readString() {
    return readString(4);
}

string OSPByteBuffer::readString(int bytesForLength) {

    // validation checks for offset, length
    if (offset+bytesForLength > length) {
        log.error("attempt to read past end of buffer in readCString()");
        throw Util::createException("attempt to read past end of buffer");
    }

    if (log.isDebugEnabled()) {
        log.debug("readString()");
    }

    int strlen = -1;
    if (bytesForLength==2) {
        strlen = OSPByteBuffer::readShort(buffer, offset);
        offset += 2;
    }
    else if (bytesForLength==4) {
        strlen = OSPByteBuffer::readInt(buffer, offset);
        offset += 4;
    }
    else {
        log.error("invalid bytesForLength (only 2 or 4 supported)");
        throw Util::createException("invalid bytesForLength (only 2 or 4 supported)");
    }

    // validation checks for offset, length
    if (offset+strlen > length) {
        ////logger.info << "readCString() offset=" << offset << "; strlen=" << strlen << "; length=" << length << endl;
        log.error(
            string("attempt to read past end of buffer in readCString due to string length ")
            + Util::toString((int)strlen)
            + string(" at offset ")
            + Util::toString((int)offset)
            + string(" exceeding buffer size of ")
            + Util::toString((int)length)
        );
        throw Util::createException("attempt to read past end of buffer");
    }

    if (strlen == -1) {
        // NULL string from OSP
        return string("");
    }

    if (strlen == 0) {
        // EMPTY string from OSP
        return string("");
    }

    // create a std::string that will take its own copy of this part of the buffer
    string str(buffer+offset, strlen);

    offset += strlen;

    return str;
}

OSPString *OSPByteBuffer::readOSPString() {
    return readOSPString(4);
}

/*
 * This method does not take a copy of the string but merely returns a reference to it
 */
OSPString *OSPByteBuffer::readOSPString(int bytesForLength) {

    // validation checks for offset, length
    if (offset+bytesForLength > length) {
        log.error("attempt to read past end of buffer in readCString()");
        throw Util::createException("attempt to read past end of buffer");
    }

    int strlen = -1;
    if (bytesForLength==2) {
        strlen = OSPByteBuffer::readShort(buffer, offset);
        offset += 2;
    }
    else if (bytesForLength==4) {
        strlen = OSPByteBuffer::readInt(buffer, offset);
        offset += 4;
    }
    else {
        log.error("invalid bytesForLength (only 2 or 4 supported)");
        throw Util::createException("invalid bytesForLength (only 2 or 4 supported)");
    }

    // validation checks for offset, length
    if (offset+strlen > length) {
        ////logger.info << "readCString() offset=" << offset << "; strlen=" << strlen << "; length=" << length << endl;
        log.error(
            string("attempt to read past end of buffer in readCString due to string length ")
            + Util::toString((int)strlen)
            + string(" at offset ")
            + Util::toString((int)offset)
            + string(" exceeding buffer size of ")
            + Util::toString((int)length)
        );
        throw Util::createException("attempt to read past end of buffer");
    }

    if (strlen == -1) {
        // NULL string from OSP
        return new OSPString("", 0, 0, false);
    }

    if (strlen == 0) {
        // EMPTY string from OSP
        return new OSPString("", 0, 0, false);
    }

    OSPString *ret = new OSPString(buffer, offset, strlen, false);
    offset += strlen;
    return ret;
}

void OSPByteBuffer::ensureCapacity(int n) {

    if (this->offset+n > this->length) {
        if (deleteBuffer) {
            // this means we allocated the buffer, so it is safe to regrow it
            char *oldBuffer = buffer;
            int newLength = this->length + n + 256;
            char *newBuffer = new char[newLength];
            memcpy(newBuffer, oldBuffer, this->offset);
            buffer = newBuffer;
            this->length = newLength;

            // delete old buffer
            delete [] oldBuffer;
        }
        else {
            // user-allocated buffer, so we fail
            log.error("buffer overflow");
            throw "buffer overflow";
        }
    }
}

void OSPByteBuffer::writeByte(char byte) {
    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "writeByte(%i) offset=%i, length=%i", byte, offset, length);
        log.debug(msg);
    }

    ensureCapacity(1);
    buffer[offset] = byte;
    offset++;
}

void OSPByteBuffer::writeBytes(const char *data, unsigned int dataOffset, unsigned int numBytes) {

    if (log.isDebugEnabled()) {
        char msg[256];
        sprintf(msg, "writeBytes(numBytes=%u) offset=%u, length=%u", numBytes, offset, length);
        log.debug(msg);
    }

    ensureCapacity(numBytes);
    if (data == NULL) {
        log.error("null pointer");
        throw Util::createException("null pointer");
    }
    memcpy(buffer+offset, data+dataOffset, numBytes);
    offset += numBytes;
}

void OSPByteBuffer::writeString(string str) {
    writeString(str.c_str(), 4);
}

void OSPByteBuffer::writeString(const char *data, int bytesForLength) {

    unsigned int stringLength = strlen(data);

    if (log.isDebugEnabled()) {
        char msg[128];
        sprintf(msg, "writeString(\"%s\" (%u chars)) offset=%u, length=%u", data, stringLength, offset, length);
        log.debug(msg);
    }


    unsigned int numBytes = bytesForLength + stringLength; // 4 bytes for string length
    ensureCapacity(numBytes);
    // write length of string (number of bytes excluding null terminator)
    if (bytesForLength==2) {
        OSPByteBuffer::writeShort(buffer, offset, (unsigned short) stringLength);
    }
    else if (bytesForLength==4) {
        OSPByteBuffer::writeInt(buffer, offset, stringLength);
    }
    else {
        log.error("invalid value for bytesForLength");
        throw Util::createException("invalid value for bytesForLength");
    }
    offset += bytesForLength;
    // write string (excluding null terminator)
    strncpy(buffer+offset, data, stringLength);
    offset += stringLength;
}

void OSPByteBuffer::writeShort(short n) {

    if (log.isDebugEnabled()) {
        char msg[128];
        sprintf(msg, "writeShort(%i) offset=%u, length=%u", n, offset, length);
        log.debug(msg);
    }

    ensureCapacity(2);
    OSPByteBuffer::writeShort(buffer, offset, n);
    offset += 2;
}

void OSPByteBuffer::writeVarInt(int n) {

    //log.trace(string("writeVarInt(") + Util::toString(n) + string(") BEGIN"));

    while (true) {
        if ((n & ~0x7F) == 0) {
            ensureCapacity(1);
            buffer[offset++] = n & 0xFF;
            //log.trace(string("writeVarInt() WRITE BYTE ") + Util::toString((int)buffer[offset-1]));
            //log.trace(string("writeVarInt() END"));
            return;
        } else {
            ensureCapacity(1);
            buffer[offset++] = ((n & 0x7F) | 0x80);
            //log.trace(string("writeVarInt() WRITE BYTE ") + Util::toString((int)buffer[offset-1]));
            n >>= 7;
        }
    }
}

void OSPByteBuffer::writeInt(int fieldNum, int i) {
    writeVarInt(fieldNum << 3); // wire type 0
    writeVarInt(i);
}

void OSPByteBuffer::writeString(int fieldNum, string s) {
    writeBytes(fieldNum, s.c_str(), 0, s.length());
}

void OSPByteBuffer::writeBytes(int fieldNum, const char *buffer, unsigned int offset, unsigned int length) {
    writeVarInt((fieldNum << 3) | 0x02); // wire type 2
    writeVarInt(length);
    writeBytes(buffer, offset, length);
}

void OSPByteBuffer::writeInt(int n) {

    if (log.isDebugEnabled()) {
        char msg[128];
        sprintf(msg, "writeInt(%i) offset=%u, length=%u", n, offset, length);
        log.debug(msg);
    }

    ensureCapacity(4);
    OSPByteBuffer::writeInt(buffer, offset, n);
    offset += 4;
}

void OSPByteBuffer::writeLong(long n) {

    if (log.isDebugEnabled()) {
        char msg[128];
        sprintf(msg, "writeLong(%li) offset=%u, length=%u", n, offset, length);
        log.debug(msg);
    }

    ensureCapacity(8);
    OSPByteBuffer::writeLong(buffer, offset, n);
    offset += 8;
}

char *OSPByteBuffer::getBuffer() {
    return buffer;
}

unsigned int OSPByteBuffer::getLength() {
    return length;
}


short OSPByteBuffer::readShort(char *data, int offset) {
    short byte0 = (short) (0xFF & *(data+offset+0));
    short byte1 = (short) (0xFF & *(data+offset+1));
    int ret = byte0<<8 | byte1;
    return ret;
}

int OSPByteBuffer::readInt(char *data, int offset) {
    int byte0 = (int) (0xFF & *(data+offset+0));
    int byte1 = (int) (0xFF & *(data+offset+1));
    int byte2 = (int) (0xFF & *(data+offset+2));
    int byte3 = (int) (0xFF & *(data+offset+3));
    int ret = byte0<<24 | byte1<<16 | byte2<<8 | byte3;
    return ret;
}

long OSPByteBuffer::readLong(char *data, int offset) {
    long byte0 = (long) (0xFF & *(data+offset+0));
    long byte1 = (long) (0xFF & *(data+offset+1));
    long byte2 = (long) (0xFF & *(data+offset+2));
    long byte3 = (long) (0xFF & *(data+offset+3));
    long byte4 = (long) (0xFF & *(data+offset+4));
    long byte5 = (long) (0xFF & *(data+offset+5));
    long byte6 = (long) (0xFF & *(data+offset+6));
    long byte7 = (long) (0xFF & *(data+offset+7));
    long ret = byte0<<56 | byte1<<48 | byte2<<40 | byte3<<32 | byte4<<24 | byte5<<16 | byte6<<8 | byte7;
    return ret;
}

void OSPByteBuffer::writeShort(char *b, int offset, short n) {
    b[offset+1] = (char) (n);
    n = n >> 8;
    b[offset+0] = (char) (n);
}

void OSPByteBuffer::writeInt(char *b, int offset, int n) {
    b[offset+3] = (char) (n);
    n = n >> 8;
    b[offset+2] = (char) (n);
    n = n >> 8;
    b[offset+1] = (char) (n);
    n = n >> 8;
    b[offset+0] = (char) (n);
}

void OSPByteBuffer::writeLong(char *b, int offset, long n) {
    b[offset+7] = (char) (n);
    n = n >> 8;
    b[offset+6] = (char) (n);
    n = n >> 8;
    b[offset+5] = (char) (n);
    n = n >> 8;
    b[offset+4] = (char) (n);
    n = n >> 8;
    b[offset+3] = (char) (n);
    n = n >> 8;
    b[offset+2] = (char) (n);
    n = n >> 8;
    b[offset+1] = (char) (n);
    n = n >> 8;
    b[offset+0] = (char) (n);
}

string OSPByteBuffer::toString() {
    string ret = "";
    for (unsigned int i=0; i<length; i++) {
        ret << (int) (0xFF & buffer[i]) << " ";
    }
    return ret;
}

void OSPByteBuffer::flush() {
    // do nothing
}


}
