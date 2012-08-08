#!/bin/bash
gcc -I /usr/include/mysql multithreadtest.cpp -L /usr/lib64/myosp/ -lmysqlclient -ldl
