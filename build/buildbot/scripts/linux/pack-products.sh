#!/bin/bash
TMP_WORK=$1
SOURCE_ROOT=$2
GINGA_PATH=$3
ZAPPER_PATH=$4
REP_TARGET=$5

TMP_ROOT=${TMP_WORK}/rootfs
TMP_IMG=${TMP_WORK}/images
IMAGEDATE=`date +%Y%m%d-%H%M`
REP_USB=${REP_TARGET}/${IMAGEDATE}/

# clean temp dir
rm -rf ${TMP_WORK}
mkdir -p ${TMP_ROOT}
mkdir -p ${TMP_IMG}
mkdir -p ${REP_USB}

# copy root to temp dir 
cp -adPr ${SOURCE_ROOT}/CS/platform/root ${TMP_ROOT}
sync 

STB_PATH=${TMP_ROOT}/root/newtronic/original
mkdir -p ${STB_PATH}
cp -r ${GINGA_PATH}/* ${STB_PATH}
cp -r ${ZAPPER_PATH}/* ${STB_PATH}
rm -rf ${STB_PATH}/lib/lifia/*.a ${STB_PATH}/include

# create image
cd ${SOURCE_ROOT}/CS/platform/
echo "[image] using dir: ${SOURCE_ROOT}/platform/"
./image.sh -r ${TMP_ROOT}/root -i ${TMP_IMG}

# deploy image to repository and create symlink to latest
cp ${TMP_IMG}/* ${REP_USB}/
ln -s ${REP_USB} ${REP_TARGET}/LATEST
#cp /builds/dsmcc-upg/${UPG_DIR}/* /home/builder/gw/src/testcases/postbuild/upgrade/resources/

