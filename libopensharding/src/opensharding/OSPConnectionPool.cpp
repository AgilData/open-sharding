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
#include <opensharding/OSPConnectionPool.h>
#include <util/MutexLock.h>

//TODO: convert this global mutex into a class member
static pthread_mutex_t OSPConnectionPool_mutex = PTHREAD_MUTEX_INITIALIZER;

using namespace util;

namespace opensharding {

OSPConnectionPool::OSPConnectionPool() {
    poolSize = 200;
    pool = new OSPConnection*[poolSize];
    for (int i=0; i<poolSize; i++) {
        pool[i] = NULL;
    }
}

OSPConnectionPool::~OSPConnectionPool() {
    MutexLock lock("OSPConnectionPool_mutex", &OSPConnectionPool_mutex);
    // delete all the connections
    for (int i=0; i<poolSize; i++) {
        if (pool[i]) {
            delete pool[i];
            pool[i] = NULL;
        }
    }
}

OSPConnection *OSPConnectionPool::borrowConnection() {
    MutexLock lock("OSPConnectionPool_mutex", &OSPConnectionPool_mutex);
    OSPConnection *ret = NULL;
    for (int i=0; i<poolSize; i++) {
        if (pool[i]) {
            ret = pool[i];
            pool[i] = NULL;
            break;
        }
    }
    return ret;
}

void OSPConnectionPool::returnConnection(OSPConnection *conn) {
    MutexLock lock("OSPConnectionPool_mutex", &OSPConnectionPool_mutex);
    // look for the first empty slot
    bool returned = false;
    for (int i=0; i<poolSize; i++) {
        if (!pool[i]) {
            // return to pool
            pool[i] = conn;
            returned = true;
            break;
        }
    }

    if (!returned) {
        // no room in pool
        delete conn;
    }
}


}
