#!/bin/bash
if [ -z "$1" ]; then
  echo "USAGE: $0 TARBALL"
  exit 2;
fi

TARBALL=$1
rm -f /usr/lib64/opensharding_mysql/libmyosp*.so.1?
tar xzf $TARBALL
cp libmyosp*.so.1? /usr/lib64/opensharding_mysql
rm -f libmyosp*.so.1?
