MYSQL_INCLUDE_PATH=/usr/local/include/mysql
BOOST_INCLUDE_PATH=/usr/local/include/boost-1_38
BOOST_LIB_PATH=/usr/local/lib/

TEST_LIB=uuid

# automatically detect gcc version so that the correct boost library version is referenced when linking the wrapper
GCC_VERSION := $(shell sh -c 'gcc -v 2>&1|tail -n 1 | cut -f 3 -d " "  | cut -f -2 -d \. |tr -d \. || echo UNKNOWN')

# manual override
#GCC_VERSION=44

BOOST_THREAD_LIB=boost_thread-gcc$(GCC_VERSION)-mt
BOOST_SYSTEM_LIB=boost_system-gcc$(GCC_VERSION)-mt

CPPFLAGS=-I./src -I/usr/local/opensharding/include -I$(MYSQL_INCLUDE_PATH) -I$(BOOST_INCLUDE_PATH) $(CPP_OPTIONS) $(EXTRA_CFLAGS)

RELEASE_CPPOPTIONS="-g -O2 -Wall -c -fmessage-length=0 -fPIC -MMD -MP"
DEBUG_CPPOPTIONS="-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP"

OBJ=src/mysql/MySQLClient.o src/mysql/MySQLAbstractConnection.o src/mysql/MySQLAbstractResultSet.o src/mysql/MySQLConnMap.o src/mysql/MySQLDriver.o src/mysql/MySQLErrorState.o src/mysql/MySQLOSPConnection.o src/mysql/MySQLNativeConnection.o src/mysql/MySQLNativeResultSet.o src/mysql/MySQLOSPResultSet.o

TEST_OBJ=test/src/MySQLTest.o test/src/Test.o

all: $(SO_FILE) $(TEST_FILE)

fifteen:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.0.45 -DMYSQL_5_0" SO_FILE="libmyospfacade.so.15" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient.so"
fifteen_r:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.0.45 -DMYSQL_5_0" SO_FILE="libmyospfacade_r.so.15" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient_r.so"
fifteen_debug:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.0.45 -DMYSQL_5_0" SO_FILE="libmyospfacade.so.15.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient.so"
fifteen_r_debug:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.0.45 -DMYSQL_5_0_R" SO_FILE="libmyospfacade_r.so.15.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient_r.so"

sixteen:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.1.36 -DMYSQL_5_1" SO_FILE="libmyospfacade.so.16" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient.so"
sixteen_r:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.1.36 -DMYSQL_5_1" SO_FILE="libmyospfacade_r.so.16" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient_r.so"
sixteen_debug:
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.1.36 -DMYSQL_5_1" SO_FILE="libmyospfacade.so.16.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient.so"

$(SO_FILE): $(OBJ)
	@echo "Creating Shared Library"
	#gcc -shared -Wl,-soname,$(DEST_SO) -o $(SO_FILE) src/mysql/*.o -lc -lstdc++ -luuid -lopensharding -lmysys -lmystrings -l$(BOOST_THREAD_LIB) -L$(BOOST_LIB_PATH) -L/usr/local/opensharding/lib -L/usr/lib64/mysql
	gcc -shared -Wl,-soname,$(DEST_SO) -o $(SO_FILE) src/mysql/*.o -lc -lstdc++ -luuid -lopensharding -l$(BOOST_THREAD_LIB) -L$(BOOST_LIB_PATH) -L/usr/local/opensharding/lib -L/usr/lib64/mysql

testsuite_fifteen: fifteen
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.0.45 -DMYSQL_5_0" TEST_FILE="testsuite" CPP_OPTIONS=$(RELEASE_CPPOPTIONS)
testsuite_sixteen: sixteen
	make EXTRA_CFLAGS="-Imysql-headers/mysql-5.1.36 -DMYSQL_5_1" TEST_FILE="testsuite" CPP_OPTIONS=$(RELEASE_CPPOPTIONS)
$(TEST_FILE): $(TEST_OBJ)
	@echo "Compiling tests..."
	gcc -o $(TEST_FILE) test/src/*.o -lc -llogger -lutil -lopensharding -l$(BOOST_THREAD_LIB) -l$(TEST_LIB) -L/usr/local/lib -lcpptest -L. -L/usr/local/lib 

clean:
	rm -f *.a lib*.so* testsuite
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*.a" -exec rm -f {} \;
	find . -name "*.d" -exec rm -f {} \;
