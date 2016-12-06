#!/bin/sh

: ${LOADER_SET_VALUE=/usr/sbin/fw_setenv}
: ${TAC_USER=tac}
: ${KERNEL_LEVEL=4}
: ${RUN_ONCE=0}

# Get system flush delay
FLUSH_DELAY=$( expr $( cat /proc/sys/vm/dirty_expire_centisecs ) / 100 )
FLUSH_DELAY=$( expr $FLUSH_DELAY + 2 )

# Ingore kernel messages
echo ${KERNEL_LEVEL} > /proc/sys/kernel/printk

# Set locale variables
export LANG=C
export LC_ALL=C

# Create home directory
export HOME=$(mktemp -d /tmp/tac.XXXXXX)
chown tac:tac $HOME

sync_sleep() {
	echo "[sync_sleep] Syncinc filesystem ..."
	sync
	sleep $FLUSH_DELAY
}

tac_serial() {
	ifconfig eth0 | awk '/HWaddr/ {print $NF}'
}

tac_shutdown() {
	sync_sleep

	echo "mem" > /sys/power/state
	# Wait forever
	sleep 100000000
}

tac_reboot() {
	sync_sleep

	reboot
	# Wait forever
	sleep 100000000
}

tac_recovery() {
	$LOADER_SET_VALUE FORCE_RECOVERY 1
	$LOADER_SET_VALUE FACTORY_RESET 1
	tac_reboot
}

tac_update() {
	$LOADER_SET_VALUE FORCE_RECOVERY 1
	$LOADER_SET_VALUE FACTORY_RESET 1
	$LOADER_SET_VALUE URL_IMG_TAC $1
	tac_reboot
}

