#!/bin/bash

CHROME="./htmlshelltest"
LOG="--enable-logging=stderr --log-level=0 --v=0"
TRACE=""
# TRACE="--trace-to-console=ozone"
OZONE="--ozone-platform=headless --ozone-dump-file=/dev/null"
CRASH="--enable-crash-reporter"

OWNER=$( stat -c "%u" chrome-sandbox )
if [ $OWNER -ne 0 ]; then
	sudo chown root:root chrome-sandbox
fi

PERMS=$( stat -c "%a" chrome-sandbox )
if [ $PERMS -ne 4755 ]; then
	sudo chmod 4755 chrome-sandbox
fi

# Run htmlshelltest
$CHROME $LOG $TRACE $OZONE $CRASH $@
