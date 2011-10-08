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


#ifndef __Util_h__
#define __Util_h__

#include <string>
#include <sys/time.h>

#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace util {

class Util {

private:
    static Logger &xlog;

public:

    static void init();
    static string generateUUID();
    static void generateUniqueID(char *);
    static void dump(const char *buf, unsigned int len, int lineLength);
    static string createRandomWord(unsigned int length);
    static char *createString(const char *source);
    static const char *notImplemented(const char *klass, const char *method);

    static string toLower(string str);

    static bool equalsIgnoreCase(string str1, string str2);
    static bool equalsIgnoreCase(const char *str1, const char *str2);
    static bool _strnicmp(const char *str1, const char *str2, int l);

    static string toString(long int i);
    static string toString(int i);
    static string toString(void *p);
    static string toString(const void *p);

    static string trim(string);
    static string removeQuotes(string);
    static string formatIP(char *);
    static void parseIP(string hostAddr, char *hostBytes);
    static const char * createException(const char *message);

    static string getElapsedTimeString(struct timeval *start, struct timeval *end);

};

}

#endif
