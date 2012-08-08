#!/bin/bash
gcc -I /usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP multithreadtest.cpp
gcc -o multithreadtest multithreadtest.o -lc -lstdc++ -ldl -lmysqlclient -llibboost_thread-gcc44-mt-1_38 -L/usr/lib64/myosp -L/usr/local/lib


