#!/bin/sh

# Variables
: ${ROOTFS=""}
: ${ZAMBA_ROOTFS_PATH=$ROOTFS/firmware}
: ${RAMDISK_PATH=$ROOTFS/tmp/zamba}
: ${UPDATE_PATH=$ZAMBA_ROOTFS_PATH/var/}
: ${NEED_REBOOT=1}
: ${ZAPPER_TOOL="zamba"}
: ${ZAPPER_PARAMS="--set=zapper.ramDisk=${RAMDISK_PATH}"}
: ${ZAPPER_LOG="--debug=all.all.debug"}
: ${RUN_ZAPPER=1}
: ${UPDATE_CODE=162}
: ${HDMI_FILE=/sys/class/amhdmitx/amhdmitx0/hpd_state}

# Get system flush delay
FLUSH_DELAY=$( expr $( cat /proc/sys/vm/dirty_expire_centisecs ) / 100 )
FLUSH_DELAY=$( expr $FLUSH_DELAY + 2 )
if [ -z  "$GINGA_VERSION" ]
then
	export GINGA_VERSION=$($ZAMBA_ROOTFS_PATH/bin/ginga -v | head -n 1 | sed -e "s#Ginga.ar \(.*\) by LIFIA#\1##")
fi
