#!/bin/bash

OSNAME=""
RELEASE=""
if [ -f /etc/issue ]; then
    ISSUE=$(head -n1 /etc/issue)
    LINES=$(echo "${ISSUE}" | grep -c " ") # check for space in first line (i.e. valid)
    if [ $LINES -eq 0 ]; then
        OSNAME="arch"
    else
        OSNAME=$(echo ${ISSUE} | cut -f 1 -d " " | tr [:upper:] [:lower:])
        RELEASE=$(echo $ISSUE | sed 's/[^0-9]*\([0-9\.]*\) .*/\1/' | tr \. _)
    fi
else
    unamestr=`uname`
    if [[ "$unamestr" == 'Darwin' ]]; then
        OSNAME="macosx"
        SHAREDLIBEXT="dylib"
    else
        OSNAME="arch"
    fi
fi
if [ -z "$RELEASE" ]; then RELEASE="X"; fi
if [ "$OSNAME" == "ubuntu" -o "$OSNAME" == "arch" ]; then
  RELEASE="X"
fi


X64=$(uname -a | grep x86_64)
BITS=""; if [ -n "$X64" ]; then BITS="x64"; else BITS="x32"; fi
OSNAME_FULL="${OSNAME}-${RELEASE}-${BITS}"
echo "osname=${OSNAME}"           >> build.properties
echo "osname-full=${OSNAME_FULL}" >> build.properties
