#!/bin/bash

function createSdtt(){
	mv $DESTDIR/lifia $DESTDIR/firmware$VERSION_ID
	cd $DESTDIR
	tar -cP . | gzip > ~/upgrades/$IMAGEDATE/firm_${MAKER_ID}_${MODEL_ID}_${VERSION_ID}.tar.gz
	cp ~/upgrades/$IMAGEDATE/firm_${MAKER_ID}_${MODEL_ID}_${VERSION_ID}.tar.gz  ~/ts_ota/ts_ota_builder2/upgrade.tar.gz
        cd ~/ts_ota/ts_ota_builder2
        make clean
        make
        mv tc_sdtt_celestial.ts ~/upgrades/$IMAGEDATE
}

function createOTA(){
	echo "dir ${WORK_DIR}/opt/CS/tools/ts_maker"
	cd ${WORK_DIR}/opt/CS/tools/ts_maker
	sed -i "s/0x11/0x${OTA_MAKER_ID}/g;s/0x12/0x${OTA_MODEL_ID}/g" "ts_maker.cfg"
	rm ota.ts -rf
	rm rom.img -rf
	rm rootfs -rf
        ln -s ~/buildbot/builds/usb/LATEST/rootfs rootfs
	./ts_maker_nor
	if [ $? != "0" ]; then
		exit 1
	fi
  	cp ota.ts ${WORK_DIR}/opt/CS/ts_ota/ts_ota_builder1
	cd ${WORK_DIR}/opt/CS/ts_ota/ts_ota_builder1
	make clean
	make
	if [ $? != "0" ]; then
		exit 1
	fi
	OTADIR=~/buildbot/builds/ota/${IMAGEDATE}/
	mkdir ${OTADIR} -p
	mv tc_ota_celestial.ts ${OTADIR}/.
	echo "[OTA] maker_id=${MAKER_ID}, model_id=${MODEL_ID}"
	echo "[OTA] output=${OTADIR}"
}


#----------------------------------------------------------------------------------------
BUILD_TYPE=$1
MAKER_ID=18
MODEL_ID=32

if [ $BUILD_TYPE == "ota" ]; then
	#	OTA
	OTA_MAKER_ID=$2
	OTA_MODEL_ID=$3
	VERSION_ID=$4
else if [ $BUILD_TYPE == "sdtt" ]; then
	#	SDTT
	VERSION_ID=$2
	else
		echo "The build type must be ota or sdtt"; exit 1
	fi
fi

#----------------------------------------------------------------------------------------

IMAGEDATE=`date +%Y%m%d-%H%M`
# Use workdir from environment
# WORK_DIR=work
WORK_DIR="`( cd "$WORK_DIR" && pwd  && cd .. )`" 

# use DESTDIR from environment 
#export DESTDIR=$WORK_DIR/install

if [ $BUILD_TYPE == "ota" ]; then createOTA; fi
if [ $BUILD_TYPE == "sdtt" ]; then createSdtt; fi
echo "Finished."
