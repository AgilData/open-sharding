#!/bin/bash
if [ -z "$1" ]; then
  echo "USAGE: $0 TARBALL LIB_DIR"
  exit 2;
fi
if [ -z "$2" ]; then
  echo "USAGE: $0 TARBALL LIB_DIR"
  exit 2;
fi


TARBALL=$1
LIB_DIR=$2
rm -f $LIB_DIR/$TARBALL 2>/dev/null
(cd $LIB_DIR;
  cp /usr/local/lib/libboost_thread-gcc4?-mt-1_38.so.1.38.0 .
  cp /usr/local/opensharding/lib/libopensharding.so.1.0.0 .
  cp ../scripts/setup.rb .
  cp ../src/myosp.conf .
  cp ../src/README.txt .
  tar cvzf $TARBALL lib* setup.rb myosp.conf README.txt
)
mv $LIB_DIR/$TARBALL .

