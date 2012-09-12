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
#include <cstring>

#include <opensharding/OSPFileInputStream.h>

#include <logger/Logger.h>
#include <util/Util.h>

using namespace std;
using namespace logger;
using namespace util;

namespace opensharding {

logger::Logger &OSPFileInputStream::log = Logger::getLogger("OSPFileInputStream");

OSPFileInputStream::OSPFileInputStream(int fd, int _buf_size) {
    this->fd = fd;
    this->buf_size = _buf_size;

    buf_pos = 0;
    buf_mark = 0;
    buffer = new char[buf_size];

    intBuffer = new char[4];
}

OSPFileInputStream::~OSPFileInputStream() {
    delete [] intBuffer;

    if (buffer) {
        delete [] buffer;
        buffer = NULL;
    }
}

short OSPFileInputStream::readShort() {
    readBytes(intBuffer, 0, 2);
    int byte0 = (int) (0xFF & *(intBuffer+0));
    int byte1 = (int) (0xFF & *(intBuffer+1));
    return byte0<<8 | byte1;
}

int OSPFileInputStream::readInt() {
    readBytes(intBuffer, 0, 4);
    int byte0 = (int) (0xFF & *(intBuffer+0));
    int byte1 = (int) (0xFF & *(intBuffer+1));
    int byte2 = (int) (0xFF & *(intBuffer+2));
    int byte3 = (int) (0xFF & *(intBuffer+3));
    return byte0<<24 | byte1<<16 | byte2<<8 | byte3;
}

void OSPFileInputStream::readBytes(char *dest, unsigned int offset, unsigned int length) {

    bool DEBUG = log.isDebugEnabled();

    if (DEBUG) {
        log.debug(string("readBytes()")
            + string(": dest=") + Util::toString((void*)dest)
            + string("; offset=") + Util::toString((int)offset)
            + string("; length=") + Util::toString((int)length)
        );
    }
    
//    bool USE_BUFFER = true;
//
//    // temporary HACK so we just read directly into the user's buffer
//    if (!USE_BUFFER) {
//        fill_buffer(dest, offset, length, length);
//        return;
//    }
    
    // loop until we write enough data into the caller's buffer
    while (length>0) {
        // how much data do we already have?
        unsigned int available_bytes = buf_mark - buf_pos;

        // if we have enough data to fulfil the request then copy it into the caller's buffer and return
        if (available_bytes >= length) {
            // yes we have all the data the user needs
            memcpy(dest+offset, buffer+buf_pos, length);
            buf_pos += length;
            // return since we already wrote all the data requested
            return;
        }
        
        // do we have partial data available?
        if (available_bytes>0) {
            // we have partial data so copy that for now
            memcpy(dest+offset, buffer+buf_pos, available_bytes);
            // reduce length so we know how much more we need to read
            offset += available_bytes;
            length -= available_bytes;
            // buffer is now empty because we used all available bytes
        }
        
        //NOTE: our buffer is definitely empty if we get this far!
        buf_pos = 0;
        buf_mark = 0;
        available_bytes = 0;

        // is the caller asking for more data than we can buffer?
        if (length > buf_size) {
            // request is for more data than we can buffer, so read directly into the caller's buffer now
            fill_buffer(dest, offset, length, length);
            return;
        }
        
        // read at least the remaining 'length' bytes into our empty buffer
        buf_mark = fill_buffer(buffer, 0, buf_size, length);
    }
}

/**
 * Read directly from the file into a buffer. Read at least min_bytes_to_read but up to length.
 */
unsigned int OSPFileInputStream::fill_buffer(char *buffer, unsigned int buffer_offset, unsigned int buffer_length, unsigned int min_bytes_to_read) {

    bool DEBUG = log.isDebugEnabled();

    if (DEBUG) {
        log.debug(
            string("fill_buffer(buffer=")    + Util::toString(buffer)
            + string("; buffer_offset=")     + Util::toString(buffer_offset)
            + string("; buffer_length=")     + Util::toString(buffer_length)
            + string("; min_bytes_to_read=") + Util::toString(min_bytes_to_read)
            + string(")")
        );
    }

    unsigned int bytes_read = 0;

    // loop until we read enough data
    while (bytes_read < min_bytes_to_read) {

        if (DEBUG) {
            log.debug(string("at top of select() loop:")
                + string("; bytes_read=") + Util::toString(bytes_read)
                + string("; min_bytes_to_read=") + Util::toString(min_bytes_to_read)
            );
        }

        // set up selector info
        FD_ZERO (&readFileDescriptorSet);
        FD_SET (fd, &readFileDescriptorSet);

        // wait until some data is available to read
        int fdcount = select(fd+1, &readFileDescriptorSet, NULL, NULL, NULL);

        if (fdcount == -1) {
            // error
            log.error("select() error");
            perror("select()");
            break;
        }
        else if (fdcount == 1) {
            if (FD_ISSET(fd, &readFileDescriptorSet)) {
                // ask for enough data to fill the buffer - we may not get that much though
                int n = read(fd, buffer+buffer_offset, buffer_length-buffer_offset);
                if (DEBUG) log.debug(string("read() read ") + Util::toString(n) + string(" byte(s)"));
                if (n==0) {
                    // connection was closed, return error
                    break;
                }
                else {
                    buffer_offset += n;
                    bytes_read += n;
                }
            }
        }
        else {
            // no data
            if (DEBUG) log.debug("no data (timed out and will retry)");
            continue;
        }
    }
    
    return bytes_read;
}

}



