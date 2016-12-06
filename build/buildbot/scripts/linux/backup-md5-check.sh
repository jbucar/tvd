#!/bin/bash

ORIGINAL=$1
COPY=$2

function runCmd(){
        $1
        if [ $? -ne 0 ]; then
		echo "Backups failed"
                exit 1
        fi
}

RESTORE=`echo ${COPY} | grep bacula-restore | wc -l`

if [ ${RESTORE} -ne 1 ]; then
	echo "Error: second argument must be the temp dir for copy, not the original. It will be deleted after comparing md5 checksum."
	exit 1 
fi

rm /tmp/backup.opt.* -rf

cd ${ORIGINAL}
find . -type f -exec md5sum {} \; > /tmp/backup.opt.orig.md5

runCmd "cd ${COPY}"
find . -type f -exec md5sum {} \; > /tmp/backup.opt.copy.md5

BDATE=`date +%Y%m%d-%H%M`
sort -k 2 /tmp/backup.opt.orig.md5 > /tmp/s.backup.opt.orig-${BDATE}.md5
sort -k 2 /tmp/backup.opt.copy.md5 > /tmp/s.backup.opt.copy-${BDATE}.md5

runCmd "diff /tmp/s.backup.opt.orig-${BDATE}.md5 /tmp/s.backup.opt.copy-${BDATE}.md5"

echo "Backups integrity check successful"
echo "Deleting restored files."                                                                      
rm ${COPY}/* -rf       

exit 0

