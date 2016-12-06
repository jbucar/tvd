#!/bin/sh

# Absolute path to this script
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in
SCRIPTPATH=$(dirname "$SCRIPT")

# Default implementations
tac_serial_default() {
	cat /sys/class/net/e*/address | head -n 1
}

tac_shutdown_default() {
	exit 102
}

tac_reboot_default() {
	exit 103
}

tac_recovery_default() {
	echo "Clean data directory: ${TAC_DATA_PATH}"
	rm -rf ${TAC_DATA_PATH}/*
	$fnc_reboot
}

tac_update_default() {
	IMAGE_FILE=$1
	echo $IMAGE_FILE > ${TAC_DATA_PATH}/update.url
	$fnc_reboot
}

get_impl() {
	local var RESULT=$2
	TYPE=`type -t $1`
	if [ -n "$TYPE" ]; then
		TYPE="$1"
	else
		TYPE="$1_default"
	fi
	eval $RESULT=$TYPE
}

# Load implementation
source ${SCRIPTPATH}/impl.sh

# Load defaults functions if necesary
get_impl 'tac_serial' fnc_serial
get_impl 'tac_shutdown' fnc_shutdown
get_impl 'tac_reboot' fnc_reboot
get_impl 'tac_update' fnc_update
get_impl 'tac_recovery' fnc_recovery

# Overwrite variables
: ${RO_INSTALL_PATH=${SCRIPTPATH}}
: ${RW_INSTALL_PATH=$(dirname "$SCRIPTPATH")/data}
: ${RUN_FIRMWARE=1}
: ${URL_UPDATE=/tmp/tac_update_url}
: ${TAC_TRACE=/tmp/tvdmodule.trace}
: ${RUN_ONCE=1}

# Check data direcotry
if [ ! -d "${RW_INSTALL_PATH}" ]; then
	mkdir -p ${RW_INSTALL_PATH}
fi

# Check data permissions
if [ -n "${TAC_USER}" ]; then
	chown ${TAC_USER}:${TAC_USER} ${RW_INSTALL_PATH}
fi

# Load
if [ ${RUN_FIRMWARE} -eq 0 ]; then
	echo "[boot] Stop loading firmware by RUN_FIRMWARE variable! ..."
	exit
fi

# Setup node
NODE_INSTALL_PATH=${RO_INSTALL_PATH}/packages/ar.edu.unlp.info.lifia.tvd.node
export NODE_PATH=${NODE_INSTALL_PATH}/lib/node_modules:${NODE_INSTALL_PATH}/node_modules
NODE_BIN=node

export TAC_INSTALL_PATH=${RW_INSTALL_PATH}
export TAC_DATA_PATH=${RW_INSTALL_PATH}
export TAC_SERIAL=$( $fnc_serial )
echo "[boot] System serial: ${TAC_SERIAL}"

if  [ "$ONLY_NODE" != "1" ]; then

	while [ 1 ]; do
		# Run bootstrap
		NODE_CMD="${NODE_BIN} ${SCRIPTPATH}/bootstrap.js ${RO_INSTALL_PATH} ${RW_INSTALL_PATH}"
		echo "[boot] Launch firmware: ${NODE_CMD}"
		if [ -n "$TAC_USER" ]; then
			su -p -c "HOME=${HOME} $NODE_CMD" "${TAC_USER}"
		else
			$NODE_CMD
		fi
		EXIT_CODE=$?
		echo "[boot] Firmware exited with code ${EXIT_CODE}"

		# Handle exit code

		# Ensure no htmlshell is running
		killall htmlshell

		# recovery
		if [ $EXIT_CODE -eq 100 ]; then
			echo "[boot] Go to menu recovery ..."
			$fnc_recovery
		fi

		# update
		if [ $EXIT_CODE -eq 101 ]; then
			if [ -f ${URL_UPDATE} ]; then
				URL=`cat ${URL_UPDATE}`
				echo "[boot] Firmware update: URL=${URL}"
				$fnc_update ${URL}
			else
				echo "[boot] Update file (${URL_UPDATE}) not found!"
			fi
		fi

		if [ ${RUN_ONCE} -eq 1 ]; then
			exit 0
		fi

		# shutdown
		if [ ${EXIT_CODE} -eq 102 ]; then
			echo "[boot] Shutdown stb ..."
			$fnc_shutdown
		fi

		# reboot
		if [ ${EXIT_CODE} -eq 103 ]; then
			echo "[boot] Reboot stb ..."
			$fnc_reboot
		fi

		# reboot if crash (dvb does not work)
		if [ ${EXIT_CODE} -gt 105 ]; then
			if [ -f ${TAC_TRACE} ]; then
				mv ${TAC_TRACE} ${RW_INSTALL_PATH}/config/ar.edu.unlp.info.lifia.tvd.system/tac.trace
			fi
			echo "[boot] Reboot because crash ..."
			$fnc_reboot
		fi
	done

fi
