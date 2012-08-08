#!/bin/bash
gcc -I /usr/include/mysql multithreadtest.c -L /usr/lib64/myosp/ -lmysqlclient -ldl
