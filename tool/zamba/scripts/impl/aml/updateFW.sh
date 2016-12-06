#!/bin/sh
BINPATH="`dirname \"$0\"`"
BINPATH="`( cd \"$BINPATH\" && pwd )`"  # absolutized and normalized
if [ -z "$BINPATH" ] ; then
  # error; for some reason, the path is not accessible
  # to the script (e.g. permissions re-evaled after suid)
  echo "ERROR: cannot determine the location of the platfom scripts path."
  exit 1  # fail
fi

export BINPATH

# Load variables
. $BINPATH/vars.sh

# Move all updates to $UPDATE_PATH
mkdir -p $UPDATE_PATH
mv $RAMDISK_PATH/update/* $UPDATE_PATH

# Get first update
NEW_UPDATE_PACK=$( ls $UPDATE_PATH | head -n 1 )
echo Processing new update package: $NEW_UPDATE_PACK

fw_setenv force_recovery 1
fw_setenv url_image_tac file://$NEW_UPDATE_PACK
sync
reboot
