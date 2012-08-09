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
#include <map>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include <util/DynLib.h>
#include <util/MutexLock.h>
#include <logger/Logger.h>

using namespace logger;

// library handle -- TODO: add support for multiple libraries
void* lib_handle = NULL;

// logger for global methods
static Logger &LOG = Logger::getLogger("DynLib");

// global mutex
static pthread_mutex_t dynlib_mutex = PTHREAD_MUTEX_INITIALIZER;

namespace util {

DynLib::DynLib(const char *libraryName) {

    MutexLock lock("DynLib", &dynlib_mutex);

    if (NULL == lib_handle) {

        if (LOG.isDebugEnabled()) {
            LOG.debug("Running one-off init in DynLib");
        }

        if (libraryName==NULL || strlen(libraryName)==0) {
            LOG.error("NULL or empty library name passed to DynLib constructor");
            return;
        }

        if (LOG.isDebugEnabled()) {
            LOG.debug("Loading dynamic library");
        }
        lib_handle = dlopen(libraryName, RTLD_LAZY | RTLD_LOCAL);
        if (NULL == lib_handle) {
            //logger.info << "Failed to load dynamic library " << libraryName << endl;
            LOG.error(string("Failed to load dynamic library ") + libraryName + string(" - try running ldd on this file to check for missing dependencies!"));
        } else {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Loaded OK");
            }
        }
    }
}

DynLib::~DynLib() {
    //TODO: unload library ?
}

void *DynLib::get_function(const char *functionName) {

    //TODO:
    //Mutex mutex;

    if (LOG.isDebugEnabled()) {
        LOG.debug(string("get_function(") + functionName + ")");
    }

    if (NULL == lib_handle) {
        LOG.error("Library not loaded");
        return NULL;
    }

    void *func = functionMap[functionName];
    if (func==NULL) {
        func = dlsym(lib_handle, functionName);
        if (!func) {
            const char *error = dlerror();
            LOG.error(string("Failed to resolve function '") + string(functionName) + string("' - dlerror() returns ") + string(error==NULL?"NULL":error));
            return NULL;
        }
        functionMap[functionName] = func;
    }

    return func;
}

} // namespace util


