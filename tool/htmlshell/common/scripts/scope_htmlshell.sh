#!/bin/bash

HTMLSHELL=$DEPOT/tool/htmlshell

find $HTMLSHELL/base $HTMLSHELL/media $HTMLSHELL/gpu $HTMLSHELL/content $HTMLSHELL/ui $HTMLSHELL/htmlshell $HTMLSHELL/ozone-aml \
	-path "$HTMLSHELL/base/" -o \
    \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" \) \
	-print > $HTMLSHELL/cscope.files

cscope -e -b -q -k
