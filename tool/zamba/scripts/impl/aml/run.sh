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

# Show current IP on console
ifconfig

if [ ${RUN_ZAPPER} -eq 0 ]; then
 	echo "Stop loading zapper by RUN_ZAPPER variable! ..."
	exit
fi

echo "Waiting for HDMI connection"
while true
do
	HDP_STATE=$(cat $HDMI_FILE)
	[ $HDP_STATE -ne 0 ] && break
	echo "HDMI not detected, waiting for connection..."
	sleep 2
done

echo "Loading firmware from ${ZAMBA_ROOTFS_PATH}"

while [ 1 ]; do                                                                 
	echo Removing USB notifier pipe!
	rm -f /tmp/usb_notifier

	echo "Starting zapper: ./bin/${ZAPPER_TOOL} ${ZAPPER_PARAMS}"
	cd ${ZAMBA_ROOTFS_PATH}/bin/
	./${ZAPPER_TOOL} ${ZAPPER_PARAMS} ${ZAPPER_LOG} &
	PID=$!

	echo "Setting to pid ${PID} OOM score adjust to -1000"
	echo -1000 > /proc/${PID}/oom_score_adj

	echo "Wait for zapper exit"
	wait $PID
	EXIT_CODE=$?
	echo "Zapper(${PID}) exited with code ${EXIT_CODE}"

	# Check update firmware
	if [ $EXIT_CODE -eq $UPDATE_CODE ]
	then
		echo "Performing firmware update"
		${BINPATH}/updateFW.sh
	fi

	# Check goto browser
	if [ $EXIT_CODE -eq 163 ]
	then
		echo "Launching Web Browser"
		if [ -f ${ZAMBA_ROOTFS_PATH}/bin/browser.sh ]
		then
			${ZAMBA_ROOTFS_PATH}/bin/browser.sh
		fi
	fi

	# Factory Reset
	if [ $EXIT_CODE -eq 164 ]
	then
		echo "Performing factory reset"
		${BINPATH}/factoryReset.sh
	fi

	if [ ${NEED_REBOOT} -eq 1 ]; then
 		echo "Reboot stb ..."
		sync
		reboot
	fi
done

