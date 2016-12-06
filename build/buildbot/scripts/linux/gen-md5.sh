#!/bin/bash

DESTDIR=$1
SUBDIR=$2
TOOL=$3

####
TARGET=${DESTDIR}/${SUBDIR}/${TOOL}.md5

cd ${DESTDIR}
find . -type f -exec md5sum {} \; > ${TARGET}
exit $?
