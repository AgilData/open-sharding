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
  cp /usr/local/opensharding/lib/libopensharding.so.1.0.0 .
  cp ../scripts/setup.rb .
  tar cvzf $TARBALL lib* setup.rb
)
mv $LIB_DIR/$TARBALL .

