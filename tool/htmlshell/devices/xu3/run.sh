#!/bin/sh

CHROME="./htmlshell"
LOG="--enable-logging=stderr --log-level=0 --v=0 --vmodule=*=0"
TRACE="--trace-to-console=ozone"
OZONE="--ozone-platform=fbdev"
GPU_FLAGS="--gpu-sandbox-failures-fatal=yes --gpu-sandbox-start-early=yes"
CRASH="--enable-crash-reporter"

OWNER=$( stat -c "%u" chrome-sandbox )
if [ $OWNER -ne 0 ]; then
	chown root:root chrome-sandbox
fi

PERMS=$( stat -c "%a" chrome-sandbox )
if [ $PERMS -ne 4755 ]; then
	chmod 4755 chrome-sandbox
fi

# Set locale variables
export LANG=C
export LC_ALL=C

# Run htmlshell
$CHROME $LOG $TRACE $OZONE $CRASH $GPU_FLAGS "$@"

