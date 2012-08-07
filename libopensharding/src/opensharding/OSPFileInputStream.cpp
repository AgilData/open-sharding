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

OSPFileInputStream::OSPFileInputStream(FILE *file, int buf_size) {
    this->file = file;
    this->buf_size = buf_size;

    // make sure we are non-blocking if we are using a buffer
    if (buf_size>0) {
        fd = fileno(file);
        int flags = fcntl(fd, F_GETFL);
        if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
            log.error("Failed to set O_NONBLOCK - reverting to blocking reads");
            // resort to blocking reads
            this->buf_size = 0;
        }
    }

    buf_pos = 0;
    buf_mark = 0;
    if (buf_size==0) {
        buffer = NULL;
    }
    else {
        buffer = new char[buf_size];
    }

    intBuffer = new char[4];
    stringBufferSize = 1024;
    stringBuffer = new char[stringBufferSize];

}

OSPFileInputStream::~OSPFileInputStream() {
    delete [] intBuffer;
    delete [] stringBuffer;

    if (buffer) {
        delete [] buffer;
        buffer = NULL;
    }
}

int OSPFileInputStream::readInt() {
    //log.trace("readInt()");
    readBytes(intBuffer, 0, 4);
    int byte0 = (int) (0xFF & *(intBuffer+0));
    int byte1 = (int) (0xFF & *(intBuffer+1));
    int byte2 = (int) (0xFF & *(intBuffer+2));
    int byte3 = (int) (0xFF & *(intBuffer+3));
    int ret = byte0<<24 | byte1<<16 | byte2<<8 | byte3;
    //log.trace(string("readInt() returning ") + Util::toString(ret));
    return ret;
}

string OSPFileInputStream::readString() {
    //log.trace("readString()");
    int stringLength = readInt();
    if (stringLength<1) {
        // -1 = NULL
        // 0 = empty string
        return string("");
    }

    //TODO: get rid of this temp string buffer
    if (stringLength>=stringBufferSize) {
        // re-allocate buffer
        delete [] stringBuffer;
        stringBufferSize = stringLength + 512;
        stringBuffer = new char[stringBufferSize];
    }

    readBytes(stringBuffer, 0, stringLength);
    stringBuffer[stringLength-1] = '\0'; // probably not necessary, but good for safety
    //log.trace(string("readString() returning ") + string(temp));


    //TODO: this is a mem cpy
    return string(stringBuffer, stringLength);
}

OSPString *OSPFileInputStream::readOSPString() {
    //log.trace("readString()");
    int stringLength = readInt();
    if (stringLength<1) {
        // -1 = NULL
        // 0 = empty string
        return new OSPString("", 0, 0, false);
    }

    //TODO: this is a mem cpy
    char *stringData = new char[stringLength+1];
    readBytes(stringData, 0, stringLength);
    stringData[stringLength-1] = '\0';

    return new OSPString(stringData, 0, stringLength, true);
}

void OSPFileInputStream::readBytes(char *dest, unsigned int offset, unsigned int length) {

    bool DEBUG = log.isDebugEnabled();

    if (buf_size==0) {
        // if no input buffer is available, then do a simple blocking read
        size_t n = fread(dest+offset, length, 1, file);
        if (n<1) {
            log.error(string("fread() returned no bytes (EOF?)"));
            throw "FAIL";
        }
        else if (n!=1) {
            log.error(string("fread() returned wrong number of items: ") + Util::toString((int)n));
            throw "FAIL";
        }

        if (DEBUG) {
            log.debug(string("blocking fread() returning ") + Util::toString((int)n) + string( "byte(s)"));
        }

        return;
    }

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
                if (DEBUG) {
                    memset(newBuffer, 0, new_buf_size);
                }

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
                    log.debug("Buffer BEFORE shift:");
                    Util::dump(buffer, buf_size, 128);
                }

                // the buffer is large enough, we just need to shift data to the start 
                // to clear some space 
                memcpy(buffer, buffer+buf_pos, unreadDataBytes);
                buf_pos = 0;
                buf_mark = unreadDataBytes;

                // paranoid
                if (DEBUG) {
                    memset(buffer+buf_mark, 0, buf_size-buf_mark);
                    log.debug("Buffer AFTER shift:");
                    Util::dump(buffer, buf_size, 128);
                }
            }

        }

        if (DEBUG) {
            log.debug("starting select() loop");
        }

        // loop until we read something or the file is closed
        while (bytes_read < bytes_to_read && !feof(file)) {

            if (DEBUG) {
                log.debug(string("at top of select() loop; bytes_read=") + Util::toString(bytes_read)
                        + string("; bytes_to_read=") + Util::toString(bytes_to_read));
            }

            // set up selector info
            FD_ZERO (&readFileDescriptorSet);
            FD_SET (fd, &readFileDescriptorSet);

            // selector timeout MUST BE SET EVERY TIME BECAUSE select() MODIFIES THE VALUE!
            timeout.tv_sec = 0; //TODO: make configurable?
            timeout.tv_usec = 0;

            // wait until some data is available to read
            int fdcount = select(fd+1, &readFileDescriptorSet, NULL, NULL, &timeout);

            if (fdcount == -1) {
                // error
                log.error("select() error");
                perror("select()");
            }
            else if (fdcount == 1) {
                // read the available data
                if (DEBUG) log.debug("data is available!");

                while (FD_ISSET(fd, &readFileDescriptorSet)) {
                    // this should always be true
                    if (DEBUG) log.debug("data is available for our FD!");

                    int n = fread(buffer+buf_mark, 1, buf_size-buf_mark, file);
                    if (n==0) {
                        if (DEBUG) log.debug("fread() returned 0 -- means connection closed");
                        // reset flag
                        FD_CLR(fd, &readFileDescriptorSet);
                        break;
                    }
                    else {
                        if (DEBUG) log.debug(string("fread() read ") + Util::toString(n) + string(" byte(s)"));
                        bytes_read += n;
                        // update mark
                        buf_mark += n;

                        if (buf_mark==buf_size) {
                            // DO NOT RESET THE FLAG - THERE MIGHT BE MORE DATA SINCE WE FILLED THE BUFFER
                            if (DEBUG) log.debug("fread() FILLED THE BUFFER SO GROWING IT!");

                            int new_buf_size = buf_size + 1024;
                            char *newBuffer = new char[new_buf_size];

                            // paranoid
                            if (DEBUG) {
                                memset(newBuffer, 0, new_buf_size);
                            }

                            memcpy(newBuffer, buffer, buf_size);
                            delete [] buffer;
                            buffer = newBuffer;
                            buf_size = new_buf_size;

                        }
                        else {
                            FD_CLR(fd, &readFileDescriptorSet);
                        }
                    }
                }
            }
            else {
                // no data
                if (DEBUG) log.debug("no data (timed out and will retry)");
                continue;
            }

            if (DEBUG) log.debug("at end of select() loop");
        }

        if (feof(file)) {
            log.error(string("fread() failed due to EOF"));
            throw "EOF";

        }

        if (DEBUG) {
            log.debug(string("After non-blocking fread(): bytesRead=") + Util::toString((int)bytes_read)
                + string("; buf_pos=") + Util::toString((int)buf_pos)
                + string("; buf_mark=") + Util::toString((int)buf_mark)
                + string("; buf_size=") + Util::toString((int)buf_size)
            );
        }

        if (buf_pos+length>buf_mark) {
            log.error("not enough data to fulfil request");
            throw "FAIL";
        }
    }

    // do we need this memcpy? can't we fread() directly into the user buffer? NO because we usually get more bytes than we ask for

    // copy data from fread buffer to user buffer
    memcpy(dest+offset, buffer+buf_pos, length);
    buf_pos += length;
}

}

