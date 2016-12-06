#!/bin/bash

CHROME="./htmlshell"
LOG="--enable-logging=stderr --log-level=0 --v=0 --vmodule=*=0"
TRACE=""
# TRACE="--trace-to-console=ozone"
OZONE="--ozone-platform=egltest"
CRASH="--enable-crash-reporter"

OWNER=$( stat -c "%u" chrome-sandbox )
if [ $OWNER -ne 0 ]; then
	sudo chown root:root chrome-sandbox
fi

PERMS=$( stat -c "%a" chrome-sandbox )
if [ $PERMS -ne 4755 ]; then
	sudo chmod 4755 chrome-sandbox
fi

# Run htmlshell (user need to be in input group for keyboard support)
$CHROME $LOG $TRACE $OZONE $CRASH "$@"
