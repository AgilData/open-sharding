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
#ifndef __OSPNamedPipeConnection_h__
#define __OSPNamedPipeConnection_h__

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//#include <string>
//#include <iostream>
#include <map>
#include <queue>
#include <stdio.h>

#include <opensharding/OSPConnectionInfo.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPMessage.h>
#include <opensharding/OSPMessageConsumer.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPFileInputStream.h>
#include <opensharding/OSPFileOutputStream.h>
#include <opensharding/OSPByteBuffer.h>
#include <logger/Logger.h>



#define OSPNP_SUCCESS                       0
#define OSPNP_ERROR                         -1
#define OSPNP_CREATE_REQUEST_PIPE_ERROR     -2
#define OSPNP_CREATE_RESPONSE_PIPE_ERROR    -3
#define OSPNP_OPEN_REQUEST_PIPE_ERROR       -4
#define OSPNP_OPEN_RESPONSE_PIPE_ERROR      -5

#define OSPNP_MAX_REQUEST_ID                2000000000    // just to make sure we never overflow



namespace opensharding {


// TODO: Should probably break queue out into it's own class

typedef std::queue<OSPWireResponse*> fifo_id_msg_queue;



class OSPNamedPipeConnection : public OSPConnection {
private:
    FILE *requestPipe;
    FILE *responsePipe;

    OSPFileInputStream *is;
    OSPFileOutputStream *os;

    string requestPipeFilename;
    string responsePipeFilename;

    bool m_threadedResponseFifo;

    boost::thread* m_thread;
    boost::condition_variable m_cond;
    boost::mutex m_mutex;
    boost::mutex m_send_mutex;
 
    bool m_fifosCreated;
    bool m_fifosOpened;

    int nextRequestID;

    char *buffer;
    unsigned int bufferSize;

    map<int, fifo_id_msg_queue> m_responses;

    static logger::Logger &log;

public:
    OSPNamedPipeConnection(OSPConnectionInfo *info, bool threadedResponseFifo);
    ~OSPNamedPipeConnection();
    
    int init(OSPConnectionInfo *info, bool threadedResponseFifo);

    string getRequestPipeFilename() { return requestPipeFilename; }
    string getResponsePipeFilename() { return responsePipeFilename; }

    bool fifosCreated() { return m_fifosCreated; }
    bool fifosOpened() { return m_fifosOpened; }

    int makeFifos();
    int openFifos();

    void startResponseThread();
 
    OSPMessage* sendMessage(OSPMessage*);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK);
    OSPMessage* sendMessage(OSPMessage*,  bool expectACK, OSPMessageConsumer *);

    int sendOnly(OSPMessage *message, bool flush);
    OSPMessage* waitForResponse();

    // response background thread handler
    void operator () ();

    void stop();
};

} // namespace

#endif // __OSPNamedPipeConnection_h__
