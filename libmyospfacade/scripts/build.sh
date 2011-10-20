#!/bin/bash

if [ -z "$2" ]; then
  echo "USAGE: $0 PLATFORM LIB_DIR"
  exit 2;
fi
PLATFORM=$1
LIB_DIR=$2

make clean; make fifteen;       cp libmyospfacade* $LIB_DIR
make clean; make fifteen_r;     cp libmyospfacade* $LIB_DIR
#make clean; make fifteen_debug; cp libmyospfacade* $LIB_DIR/DEBUG/

make clean; make sixteen;       cp libmyospfacade* $LIB_DIR
make clean; make sixteen_r;     cp libmyospfacade* $LIB_DIR
#make clean; make sixteen_debug; cp libmyospfacade* $LIB_DIR/DEBUG/

