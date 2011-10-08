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
#include <string.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <map>
#include <syslog.h>

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <logger/Logger.h>

using namespace std;

namespace logger {

static LoggerGlobalState *__loggerGlobalState__ = NULL;

LoggerGlobalState *__logger__getGlobalState() {
    //TODO: use mutex
    if (!__loggerGlobalState__) {
        cerr << "Logger creating LoggerGlobalState object" << endl;
        __loggerGlobalState__ = new LoggerGlobalState();
    }
    return __loggerGlobalState__;
}

// MACRO for readable code
#define LOGGER_GLOBAL_STATE __logger__getGlobalState()

//////////////////////////////
//
// STATIC METHODS
//
//////////////////////////////

/*static*/ Logger &Logger::getLogger(string name) {
    return *getLoggerPtr(name);
}

/*static*/ Logger *Logger::getLoggerPtr(string name) {

    cerr << "getLogger(" << name << ")" << endl;

    Logger *logger = LOGGER_GLOBAL_STATE->loggerMap[name];

    if (!logger) {

        // get log level from map
        string levelName = LOGGER_GLOBAL_STATE->logLevelMap[name];

        // convert level name to int
        int level = LOG_LEVEL_NONE;
        if (levelName == "TRACE") {
            level = LOG_LEVEL_TRACE;
        }
        else if (levelName == "DEBUG") {
            level = LOG_LEVEL_DEBUG;
        }
        else if (levelName == "INFO") {
            level = LOG_LEVEL_INFO;
        }
        else if (levelName == "WARN") {
            level = LOG_LEVEL_WARN;
        }
        else if (levelName == "ERROR") {
            level = LOG_LEVEL_ERROR;
        }
        else {
            // default
            level = LOG_LEVEL_INFO;
        }

        // create new Logger instance
        logger = new Logger(name, level);
        // store logger in map
        LOGGER_GLOBAL_STATE->loggerMap[name] = logger;
    }

    return logger;
}

/**
 * Configure logging from log.properties file, each line in the file is either:
 * - Blank line
 * - Comment (starts with #)
 * - Log level setting in format NAME=LEVEL where LEVEL is one of TRACE,DEBUG,INFO,WARN,ERROR
 */
/*static*/ void Logger::configure(string filename) {

    cerr << "Logger::configure(" << filename << ")" << endl;

    string line;
    ifstream myfile(filename.c_str());
    if (myfile.is_open()) {
        while (! myfile.eof()) {
            getline (myfile,line);

            if (line.size() == 0 || line[0] == '#' || line[0] == ' ' || line[0] == '\t') {
                // ignore - comment or blank line
                continue;
            }

            // look for '='
            int pos = line.find('=');
            if (pos<=0) {
                // ignore, not valid
                continue;
            }
    
            string className = line.substr(0,pos);
            string levelName = line.substr(pos+1);

            // store log level in map
            LOGGER_GLOBAL_STATE->logLevelMap[className] = levelName;

            if (className == "log.output") {
                if (levelName == "STDERR") {
                    LOGGER_GLOBAL_STATE->logMode = LOG_OUTPUT_STDERR;
                }
                else if (levelName == "STDOUT") {
                    LOGGER_GLOBAL_STATE->logMode = LOG_OUTPUT_STDOUT;
                }
                else if (levelName == "SYSLOG") {
                    LOGGER_GLOBAL_STATE->logMode = LOG_OUTPUT_SYSLOG;
                }
                else {
                    LOGGER_GLOBAL_STATE->logMode = LOG_OUTPUT_FILE;
                    if (!LOGGER_GLOBAL_STATE->file) {
                        LOGGER_GLOBAL_STATE->file = fopen(levelName.c_str(), "a");
                    }
                }
            }

            // convert level name to int
            int level = LOG_LEVEL_NONE;
            if (levelName == "TRACE") {
                level = LOG_LEVEL_TRACE;
            }
            else if (levelName == "DEBUG") {
                level = LOG_LEVEL_DEBUG;
            }
            else if (levelName == "INFO") {
                level = LOG_LEVEL_INFO;
            }
            else if (levelName == "WARN") {
                level = LOG_LEVEL_WARN;
            }
            else if (levelName == "ERROR") {
                level = LOG_LEVEL_ERROR;
            }
            else {
                // default
                level = LOG_LEVEL_INFO;
            }

            // configure existing logger, if it exists
            Logger *logger = LOGGER_GLOBAL_STATE->loggerMap[className];
            if (logger) {
                cerr << "configure() reconfiguring logger (" << className << ")" << endl;
                logger->setLevel(level);
            }
            else {
                cerr << "configure() could not find logger (" << className << ")" << endl;
            }

        }
        myfile.close();
    }
    else {
        cerr << "Logger::configure() failed to open file" << endl;
    }
}

//////////////////////////////
//
// NON-STATIC METHODS
//
//////////////////////////////

Logger::Logger(string name, int logLevel) {
    this->name = name;
    cerr << "Logger::Logger(" << name << ")" << endl;
    setLevel(logLevel);
}

void Logger::setLevel(int logLevel) {

    cerr << "Logger::setLevel(" << name << ", " << logLevel << ")" << endl;

    isTrace = logLevel > LOG_LEVEL_NONE && logLevel <= LOG_LEVEL_TRACE;
    isDebug = logLevel > LOG_LEVEL_NONE && logLevel <= LOG_LEVEL_DEBUG;
    isInfo = logLevel <= LOG_LEVEL_INFO;

    //HACK HACK HACK
    /*
    isTrace = true;
    isDebug = true;
    isInfo = true;
    */
}

Logger::~Logger() {
}

bool Logger::isTraceEnabled() {
    return isTrace;
}

bool Logger::isDebugEnabled() {
    return isDebug;
}

bool Logger::isInfoEnabled() {
    return isInfo;
}

bool Logger::isWarnEnabled() {
    return true;
}

void Logger::trace(string message) {
    trace(message.c_str());
}

void Logger::debug(string message) {
    debug(message.c_str());
}

void Logger::info(string message) {
    info(message.c_str());
}

void Logger::warn(string message) {
    warn(message.c_str());
}

void Logger::error(string message) {
    error(message.c_str());
}

// get process-id and thread-id
string getPidTid() {
    char temp[256];
#ifdef __APPLE__
    sprintf(temp, "[PID:N/A]");
#else
    sprintf(temp, "[PID:%d] [TID:%u]", getpid(), (unsigned int)pthread_self());
#endif
    return string(temp);
}

void Logger::log(const char *level, const char *message) {

    if (LOGGER_GLOBAL_STATE->logMode == LOG_OUTPUT_STDERR) {
        cerr << "[" << time(NULL) << "s] "
             << getPidTid()
             << " [opensharding] [" << level << "] [" << name << "] "
             << message << "\n";
    }
    else if (LOGGER_GLOBAL_STATE->logMode == LOG_OUTPUT_STDOUT) {
        cout << "[" << time(NULL) << "s] "
             << getPidTid()
             << " [opensharding] [" << level << "] [" << name << "] "
             << message << "\n";
    }
    else if (LOGGER_GLOBAL_STATE->logMode == LOG_OUTPUT_SYSLOG) {
        int slevel = LOG_ERR;
        if (strcmp("DEBUG",level)==0) {
            slevel = LOG_DEBUG;
        }
        else if (strcmp("INFO",level)==0) {
            slevel = LOG_NOTICE;
        }
        else if (strcmp("WARN",level)==0) {
            slevel = LOG_WARNING;
        }
        else if (strcmp("ERROR",level)==0) {
            slevel = LOG_ERR;
        }

        syslog (
            slevel,
            "[%d] %s [opensharding] [%s] [%s] %s",
            (int) time(NULL),
            getPidTid().c_str(),
            level,
            name.c_str(),
            message
        );
    }
    else {
        fprintf(LOGGER_GLOBAL_STATE->file, "[%ld s] %s [opensharding] [%s] [%s] %s\n", time(NULL), getPidTid().c_str(), level, name.c_str(), message);
        fflush(LOGGER_GLOBAL_STATE->file);
    }
}

void Logger::trace(const char *message) {
    if (!isTrace) return;
    log("TRACE", message);
}

void Logger::debug(const char *message) {
    if (!isDebug) return;
    log("DEBUG", message);
}

void Logger::info(const char *message) {
    if (!isInfo) return;
    log("INFO", message);
}

void Logger::warn(const char *message) {
    log("WARN", message);
}

void Logger::error(const char *message) {
    log("ERROR", message);
}

}
