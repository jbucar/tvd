#!/bin/sh

# Setup chrome-sandbox
OWNER=$( stat -c "%u" chrome-sandbox )
if [ $OWNER -ne 0 ]; then
	sudo chown root:root chrome-sandbox
fi

PERMS=$( stat -c "%a" chrome-sandbox )
if [ $PERMS -ne 4755 ]; then
	sudo chmod 4755 chrome-sandbox
fi

# Run valgrind on htmlshelltest
../tools/valgrind/chrome_tests.sh -t cmdline ./htmlshelltest
