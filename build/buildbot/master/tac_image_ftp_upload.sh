#!/bin/bash

upload() {
	SERVER=sol.lifia.info.unlp.edu.ar
	USERNAME=jbucar
	PASSWORD=jbucar2k15
	FILE=$1
	echo "Uploading ${FILE}"

	ftp -inv ${SERVER} << EOF
user ${USERNAME} ${PASSWORD}
binary
cd public_html
mdelete ${FILE}*
put ${FILE}
chmod 644 ${FILE}
put ${FILE}.md5
chmod 644 ${FILE}.md5
quit
EOF

	rm -f ${FILE}*
}

REPO=$1
PLATFORM=$2
IMAGE_BASE=$3
SRC_PATH=/mnt/repositorio/builds/${REPO}/tac/

cd ${SRC_PATH}
rm -f image-${PLATFORM}.*
for ext in tac sdk; do
	SRC=${IMAGE_BASE}.${ext}
	DST=image-${PLATFORM}.${ext}
	echo "Starting ${DST} upload."
	ln ${SRC} ${DST}
	md5sum ${DST} > ${DST}.md5
	upload ${DST} > /dev/null 2>&1 &
done
