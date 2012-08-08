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

    if (buf_size < 1) {
        // minimum buffer size
        buf_size = 1024;
    }

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

int OSPFileInputStream::readInt() {
    readBytes(intBuffer, 0, 4);
    int byte0 = (int) (0xFF & *(intBuffer+0));
    int byte1 = (int) (0xFF & *(intBuffer+1));
    int byte2 = (int) (0xFF & *(intBuffer+2));
    int byte3 = (int) (0xFF & *(intBuffer+3));
    return byte0<<24 | byte1<<16 | byte2<<8 | byte3;
}

// simple version to rule out this class as the main issue
void OSPFileInputStream::readBytes(char *dest, unsigned int offset, unsigned int length) {

    bool DEBUG = log.isDebugEnabled();

    unsigned int bytes_to_read = length;
    unsigned int bytes_read = 0;

    buf_pos = 0;
    buf_mark = 0;

    if (bytes_to_read > buf_size) {
        delete [] buffer;
        buffer = new char[bytes_to_read];
        buf_size = bytes_to_read;
    }

    // loop until we read something or the file is closed
    while (bytes_read < bytes_to_read) {

        if (DEBUG) {
            log.debug(string("at top of select() loop:")
                + string("; bytes_read=") + Util::toString(bytes_read)
                + string("; bytes_to_read=") + Util::toString(bytes_to_read)
                + string("; buf_pos=") + Util::toString(buf_pos)
                + string("; buf_mark=") + Util::toString(buf_mark)
                + string("; buf_size=") + Util::toString(buf_size)
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
        }
        else if (fdcount == 1) {

            // paranoid checks
            if (buf_mark < 0 || buf_mark >= buf_size) {
                log.error(string("buffer underflow or overflow; buf_mark=") + Util::toString(buf_mark));
                throw "buffer underflow or overflow";
            }

            if (FD_ISSET(fd, &readFileDescriptorSet)) {
                int n = read(fd, buffer+buf_mark, buf_size-buf_mark);
                if (n==0) {
                    if (DEBUG) log.debug("read() returned 0 -- means connection closed");
                    break;
                }
                else {
                    if (DEBUG) log.debug(string("read() read ") + Util::toString(n) + string(" byte(s)"));
                    bytes_read += n;
                    buf_mark   += n;

                    if (buf_mark > buf_size) {
                        // should never happen
                        log.error("buffer overflow");
                        throw "buffer overflow";
                    }
                }
            }
        }
        else {
            // no data
            if (DEBUG) log.debug("no data (timed out and will retry)");
            continue;
        }
    }

    if (DEBUG) {
        log.debug(string("After non-blocking read(): bytesRead=") + Util::toString((int)bytes_read)
            + string("; buf_pos=") + Util::toString((int)buf_pos)
            + string("; buf_mark=") + Util::toString((int)buf_mark)
            + string("; buf_size=") + Util::toString((int)buf_size)
        );
    }

    // copy to the users buffer
    memcpy(dest+offset, buffer+buf_pos, length);

}

/*

THIS THE REAL VERSION WITH READ AHEAD BUFFER

void OSPFileInputStream::readBytes(char *dest, unsigned int offset, unsigned int length) {

    bool DEBUG = log.isDebugEnabled();

    if (DEBUG) {
        log.debug(string("readBytes(length=") + Util::toString((int)length)
            + string("); buf_pos=") + Util::toString((int)buf_pos)
            + string("; buf_mark=") + Util::toString((int)buf_mark)
            + string("; buf_size=") + Util::toString((int)buf_size)
        );
    }

    // get more data, if needed
    if (buf_pos+length>buf_mark) {

        unsigned int bytes_to_read = buf_pos+length - buf_mark;
        unsigned int bytes_read = 0;

        if (DEBUG) {
            log.debug(string("need more data - need to read ") + Util::toString(bytes_to_read) + " bytes");
        }

        // is there enough space at the end of the buffer
        if (buf_pos+length>buf_size) {

            // how many unread data bytes do we have?
            unsigned int unreadDataBytes = buf_mark-buf_pos;

            // do we need a larger buffer
            if (unreadDataBytes+length>buf_size) {

                if (DEBUG) {
                    log.debug("growing buffer");
                }

                int new_buf_size = unreadDataBytes + length + 1024;
                char *newBuffer = new char[new_buf_size];

                // paranoid
                memset(newBuffer, 0, new_buf_size);

                // copy the unread data from the old buffer
                memcpy(newBuffer, buffer+buf_pos, unreadDataBytes);
                delete [] buffer;
                buffer = newBuffer;
                buf_size = new_buf_size;
                buf_pos = 0;
                buf_mark = unreadDataBytes;

            }
            else {

                if (DEBUG) {
                    log.debug(string("Moving ") + Util::toString(unreadDataBytes) + string(" unread bytes to the start of the buffer"));
                }

                // the buffer is large enough, we just need to shift data to the start to clear some space
                //NOTE: memcpy not safe if ranges overlap so we manually copy the unread bytes
                for (unsigned int i=0; i<unreadDataBytes; i++) {
                    buffer[i] = buffer[buf_pos+i];
                }

                buf_pos = 0;
                buf_mark = unreadDataBytes;

                // paranoid
                memset(buffer+buf_mark, 0, buf_size-buf_mark);
            }

        }

        // loop until we read something or the file is closed
        while (bytes_read < bytes_to_read) {

            if (DEBUG) {
                log.debug(string("at top of select() loop:")
                    + string("; bytes_read=") + Util::toString(bytes_read)
                    + string("; bytes_to_read=") + Util::toString(bytes_to_read)
                    + string("; buf_pos=") + Util::toString(buf_pos)
                    + string("; buf_mark=") + Util::toString(buf_mark)
                    + string("; buf_size=") + Util::toString(buf_size)
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
            }
            else if (fdcount == 1) {

                // paranoid checks
                if (buf_mark < 0 || buf_mark >= buf_size) {
                    log.error(string("buffer underflow or overflow; buf_mark=") + Util::toString(buf_mark));
                    throw "buffer underflow or overflow";
                }

                if (FD_ISSET(fd, &readFileDescriptorSet)) {
                    int n = read(fd, buffer+buf_mark, buf_size-buf_mark);
                    if (n==0) {
                        if (DEBUG) log.debug("read() returned 0 -- means connection closed");
                        break;
                    }
                    else {
                        if (DEBUG) log.debug(string("read() read ") + Util::toString(n) + string(" byte(s)"));
                        bytes_read += n;
                        buf_mark   += n;

                        // paranoid
                        if (buf_mark > buf_size) {
                            // should never happen
                            log.error("buffer overflow");
                            throw "buffer overflow";
                        }
                    }
                }
            }
            else {
                // no data
                if (DEBUG) log.debug("no data (timed out and will retry)");
                continue;
            }
        }

        if (DEBUG) {
            log.debug(string("After non-blocking read(): bytesRead=") + Util::toString((int)bytes_read)
                + string("; buf_pos=") + Util::toString((int)buf_pos)
                + string("; buf_mark=") + Util::toString((int)buf_mark)
                + string("; buf_size=") + Util::toString((int)buf_size)
            );
        }

        if (bytes_read < bytes_to_read) {
            log.error("not enough data to fulfil request");
            throw "FAIL";
        }
    }

    // copy data from read buffer to user buffer
    memcpy(dest+offset, buffer+buf_pos, length);
    buf_pos += length;

}
*/

}

