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
#include <opensharding/OSPMessageDecoder.h>
#include <logger/Logger.h>
#include <util/Util.h>

#define TRACE log.isTraceEnabled()

using namespace std;
using namespace logger;
using namespace util;

namespace opensharding {

logger::Logger &OSPMessageDecoder::log = Logger::getLogger("OSPMessageDecoder");

OSPMessageDecoder::OSPMessageDecoder() {
}

OSPMessageDecoder::~OSPMessageDecoder() {
}

void OSPMessageDecoder::decode(OSPMessage *message, OSPByteBuffer *bb) {

    //TODO: HACK: should really be getOffset but when OSPByteBuffer is created from existing buffer, offset is set to zero
    unsigned int size = bb->getLength();


    bb->flip();

    if (TRACE) log.trace(string("decode() BEGIN: buffer offset = ") + Util::toString((int)bb->getOffset()) + string("; size = ") + Util::toString((int)size));

    while (bb->getOffset()<size) {
        int fieldNumberAndWireType = bb->readVarInt();
        int fieldNumber = (fieldNumberAndWireType & 0xFFFFF8) >> 3; //TODO: review this - does it handle full range of base 128 varints?
        int wireType = fieldNumberAndWireType & 0x07;

        if (TRACE) log.trace(string("Field #") + Util::toString(fieldNumber) + string(" has wire type ") + Util::toString(wireType));

        try {
            if (wireType == 0) {
                // varint
                int fieldValue = bb->readVarInt();

                if (TRACE) log.trace(string("fieldValue = ") + Util::toString(fieldValue));

                message->setField(fieldNumber, fieldValue);
            } else if (wireType == 2) {
                // length delimited
                int fieldLength = bb->readVarInt();
                //System.out.println("field length = " + fieldLength);
                char *fieldValue = new char[fieldLength+1];
                bb->readBytes(fieldValue, 0, fieldLength);
                fieldValue[fieldLength] = '0';

                if (TRACE) log.trace(string("fieldValue = BINARY DATA; length=") + Util::toString((int)fieldLength));

                message->setField(fieldNumber, fieldValue, 0, fieldLength);
            } else if (wireType == 5) {
                // fixed 32 bit integer
                int fieldValue = bb->readInt();

                if (TRACE) log.trace(string("fieldValue = ") + Util::toString(fieldValue));

                message->setField(fieldNumber, fieldValue);
            } else {
                throw "UNSUPPORTED WIRE TYPE";
            }
        } catch (const char *ex) {
            log.error(string("Failed to decode message: ") + string(ex));
        } catch (...) {
            log.error("Failed to decode message");
        }
    }

    if (TRACE) log.trace("decode() END");
}

} // namespace
