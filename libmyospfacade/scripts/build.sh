#!/bin/bash

if [ -z "$2" ]; then
  echo "USAGE: $0 PLATFORM LIB_DIR"
  exit 2;
fi
PLATFORM=$1
LIB_DIR=$2

#make clean; make fifteen || exit 2;       cp libmyosp* $LIB_DIR
#make clean; make fifteen_r || exit 2;     cp libmyosp* $LIB_DIR
#make clean; make fifteen_debug || exit 2; cp libmyosp* $LIB_DIR/DEBUG/

#make clean; make sixteen || exit 2;       cp libmyosp* $LIB_DIR
make clean; make sixteen_r || exit 2;     cp libmyosp* $LIB_DIR
#make clean; make sixteen_debug || exit 2; cp libmyosp* $LIB_DIR/DEBUG/

