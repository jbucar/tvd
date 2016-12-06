#!/bin/sh

CHROME="./htmlshell"
LOG="--enable-logging=stderr --log-level=0 --v=0 --vmodule=*=0"
TRACE=""
# TRACE="--trace-to-console=ozone,drm"
OZONE="--ozone-platform=gbm"
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
export EGL_PLATFORM=surfaceless

# Run htmlshell (user need to be in input group for keyboard support)
$CHROME $LOG $TRACE $OZONE $CRASH "$@"
