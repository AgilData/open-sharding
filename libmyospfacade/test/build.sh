#!/bin/bash
gcc -I /usr/include/mysql test.c -L /usr/lib64/mysql/ -lmysqlclient -ldl
