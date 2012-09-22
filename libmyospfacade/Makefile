CPPFLAGS=-I./src -I../libopensharding/src $(CPP_OPTIONS) $(EXTRA_CFLAGS)

RELEASE_CPPOPTIONS="-g -O2 -Wall -c -fmessage-length=0 -fPIC -MMD -MP"
DEBUG_CPPOPTIONS="-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP"

include mysql_compat.mk

OBJ=src/mysql/MySQLClient.o src/mysql/MySQLAbstractConnection.o src/mysql/MySQLAbstractResultSet.o src/mysql/MySQLConnectionInfo.o src/mysql/MySQLConnMap.o src/mysql/MySQLDriver.o src/mysql/MySQLErrorState.o src/mysql/MySQLOSPConnection.o src/mysql/MySQLNativeConnection.o src/mysql/MySQLNativeResultSet.o src/mysql/MySQLOSPResultSet.o src/myutil/MyOSPConfig.o src/myutil/MyOSPLogger.o 

TEST_OBJ=test/src/MySQLTest.o test/src/Test.o

all: $(SO_FILE) $(TEST_FILE)

	
fifteen:
	make EXTRA_CFLAGS="-I../mysql-install-5.0/mysql-5.0.96/include -DMYSQL_5_0" SO_FILE="libmyosp.so.15" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient.so.15" VERSION_SCRIPT="src/mysql/version/mysqlclient_15.ver" MYSQL_OBJ="$(MYSQL50_OBJ)"
fifteen_r:
	make EXTRA_CFLAGS="-I../mysql-install-5.0/mysql-5.0.96/include -DMYSQL_5_0_R" SO_FILE="libmyosp_r.so.15" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient_r.so.15" VERSION_SCRIPT="src/mysql/version/mysqlclient_15.ver" MYSQL_OBJ="$(MYSQL50_OBJ)"
fifteen_debug:
	make EXTRA_CFLAGS="-I../mysql-install-5.0/mysql-5.0.96/include -DMYSQL_5_0" SO_FILE="libmyosp.so.15.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient.so.15" VERSION_SCRIPT="src/mysql/version/mysqlclient_15.ver" MYSQL_OBJ="$(MYSQL50_OBJ)"
fifteen_r_debug:
	make EXTRA_CFLAGS="-I../mysql-install-5.0/mysql-5.0.96/include -DMYSQL_5_0_R" SO_FILE="libmyosp_r.so.15.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient_r.so.15" VERSION_SCRIPT="src/mysql/version/mysqlclient_15.ver" MYSQL_OBJ="$(MYSQL50_OBJ)"

sixteen:
	make EXTRA_CFLAGS="-I../mysql-install-5.1/mysql-5.1.65/include -DMYSQL_5_1" SO_FILE="libmyosp.so.16" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient.so.16" VERSION_SCRIPT="src/mysql/version/mysqlclient_16.ver" MYSQL_OBJ="$(MYSQL51_OBJ)"
sixteen_r:
	make EXTRA_CFLAGS="-I../mysql-install-5.1/mysql-5.1.65/include -DMYSQL_5_1_R" SO_FILE="libmyosp_r.so.16" CPP_OPTIONS=$(RELEASE_CPPOPTIONS) DEST_SO="libmysqlclient_r.so.16" VERSION_SCRIPT="src/mysql/version/mysqlclient_16.ver" MYSQL_OBJ="$(MYSQL51_OBJ)"
sixteen_debug:
	make EXTRA_CFLAGS="-I../mysql-install-5.1/mysql-5.1.65/include -DMYSQL_5_1" SO_FILE="libmyosp.so.16.debug" CPP_OPTIONS=$(DEBUG_CPPOPTIONS) DEST_SO="libmysqlclient.so.16" VERSION_SCRIPT="src/mysql/version/mysqlclient_16.ver" MYSQL_OBJ="$(MYSQL51_OBJ)"

$(SO_FILE): $(OBJ)
	@echo "Creating Shared Library"
	#gcc -shared -Wl,--version-script=$(VERSION_SCRIPT),-soname,$(DEST_SO) -o $(SO_FILE) src/mysql/*.o src/myutil/*.o -lz -lc -lstdc++ -lpthread -lopensharding -lmysys -lmystrings -L../libopensharding
	gcc -shared -Wl,--version-script=$(VERSION_SCRIPT),-soname,$(DEST_SO) -o $(SO_FILE) src/mysql/*.o src/myutil/*.o $(MYSQL_OBJ) -lz -lc -lstdc++ -lpthread -lopensharding -L../libopensharding

testsuite_fifteen: fifteen
	make EXTRA_CFLAGS="-I../mysql-install-5.0/mysql-5.0.96/include -DMYSQL_5_0" TEST_FILE="testsuite" CPP_OPTIONS=$(RELEASE_CPPOPTIONS)
testsuite_sixteen: sixteen
	make EXTRA_CFLAGS="-I../mysql-install-5.1/mysql-5.1.65/include -DMYSQL_5_1" TEST_FILE="testsuite" CPP_OPTIONS=$(RELEASE_CPPOPTIONS)
$(TEST_FILE): $(TEST_OBJ)
	@echo "Compiling tests..."
	gcc -o $(TEST_FILE) test/src/*.o -lz -lc -llogger -lutil -lopensharding  -lcpptest -L../libopensharding -L.
clean:
	rm -f *.a lib*.so* testsuite
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*.a" -exec rm -f {} \;
	find . -name "*.d" -exec rm -f {} \;
