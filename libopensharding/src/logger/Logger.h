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



#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <map>

#include <boost/thread/mutex.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <boost/thread.hpp>

#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_TRACE 1
#define LOG_LEVEL_DEBUG 2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_WARN  4
#define LOG_LEVEL_ERROR 5

#define LOG_OUTPUT_STDERR   0
#define LOG_OUTPUT_STDOUT   1
#define LOG_OUTPUT_SYSLOG   2
#define LOG_OUTPUT_FILE     3

using namespace std;

namespace logger {

class Logger {
private:

    // logger name
    string name;

    bool isTrace;
    bool isDebug;
    bool isInfo;

public: // static methods

    static Logger &getLogger(string name);
    static Logger *getLoggerPtr(string name);
    static void configure(string filename);

private: // private instance methods

    void setLevel(int logLevel);
    void log(const char *level, const char *message);

public: // public instance methods

    Logger(string name, int level);
    virtual ~Logger();

    bool isTraceEnabled();
    bool isDebugEnabled();
    bool isInfoEnabled();
    bool isWarnEnabled();

    void trace(string message);
    void debug(string message);
    void info(string message);
    void warn(string message);
    void error(string message);

    void trace(const char *message);
    void debug(const char *message);
    void info(const char *message);
    void warn(const char *message);
    void error(const char *message);
};

// because loggers are staticly created, we can't have static members in Logger, so we have a global variable holding all global state
class LoggerGlobalState {
public:
    int logMode;
    FILE *file;
    map<string, string>  logLevelMap;
    map<string, Logger*> loggerMap;

public:
    LoggerGlobalState() {
        logMode = LOG_OUTPUT_STDERR;
        file = NULL;
    }
};

}

#endif /* LOGGER_H_ */
