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
#ifndef __OSPFileInputStream_h__
#define __OSPFileInputStream_h__

#include <opensharding/OSPInputStream.h>
#include <opensharding/OSPString.h>

#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace opensharding {

class OSPFileInputStream : public OSPInputStream {
private:

    // file descriptor
    int fd;

    // selector info
    fd_set readFileDescriptorSet;

    // temp buffers
    char *intBuffer;
    char *stringBuffer;
    int stringBufferSize;

    // read buffer
    char *buffer;
    unsigned int buf_pos;
    unsigned int buf_mark;
    unsigned int buf_size;

    static Logger &log;

public:
    OSPFileInputStream(int fd, int buf_size);
    ~OSPFileInputStream();

    // read raw data
    int readInt();
    string readString();
    OSPString *readOSPString();
    void readBytes(char *buffer, unsigned int offset, unsigned int length);
};

}

#endif // __OSPFileInputStream_h__
