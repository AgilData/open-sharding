LIBXML_INCLUDE_PATH=/usr/include/libxml2

# release settings
#CPPFLAGS=-Isrc -I$(LIBXML_INCLUDE_PATH) -O2 -Wall -c -fmessage-length=0 -fPIC -MMD -MP

# debug settings
CPPFLAGS=-Isrc -I$(LIBXML_INCLUDE_PATH) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP

all: libopensharding.so.1.0.0

libopensharding.so.1.0.0: src/opensharding/*.cpp src/opensharding/*.h
	@echo "Compiling libopensharding..."
	@rm -f *.o
	@rm -f *.d
	@g++ $(CPPFLAGS) src/opensharding/OSPByteBuffer.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPConnection.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPConnectRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPConnectResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPCreateStatementRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPCreateStatementResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPDisconnectRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPDisconnectResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPException.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPExecuteRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPExecuteResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPErrorResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPFileInputStream.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPFileOutputStream.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPInputStream.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPMessage.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPMessageConsumer.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPMessageDecoder.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPNamedPipeConnection.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPOutputStream.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPResultSetRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPResultSetResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPResultSetMetaResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPResultSetRowResponse.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPString.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPTCPConnection.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPWireRequest.cpp
	@g++ $(CPPFLAGS) src/opensharding/OSPWireResponse.cpp
	@g++ $(CPPFLAGS) src/logger/Logger.cpp
	@g++ $(CPPFLAGS) src/util/Util.cpp
	@g++ $(CPPFLAGS) src/util/DynLib.cpp
	@g++ $(CPPFLAGS) src/util/OSPConfig.cpp
	@echo "Linking libopensharding..."
	@rm -f libopensharding.so*
	@gcc -shared -Wl,-soname,libopensharding.so -o libopensharding.so.1.0.0 *.o -lc -lstdc++ -lpthread
	@ln -s libopensharding.so.1.0.0 libopensharding.so
	@rm -f *.o
	@rm -f *.d

clean:
	@rm -rf *.o
	@rm -rf *.d
	@rm -rf *.a
	@rm -rf *.so
	@rm -f libopensharding*

install:
	@echo "NO NEED TO INSTALL THIS NOW"
