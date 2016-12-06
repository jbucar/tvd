#!/bin/bash

REPO=$1
CODENAME=$2
VERSION=$3
BUILD=$4
IMAGE_BASE=$5

DST_DIR=/mnt/repositorio/entregas/tac/images/${CODENAME}/${BUILD}/${VERSION}

cd /mnt/repositorio/builds/${REPO}/tac/
mkdir -p ${DST_DIR}
for ext in sdk tac tac.sym; do
	SRC=${IMAGE_BASE}.${ext}
	cp ${SRC} ${DST_DIR}
done
cd ${DST_DIR}
md5sum ${IMAGE_BASE}.tac > ${IMAGE_BASE}.md5
