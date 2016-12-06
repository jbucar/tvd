#!/bin/bash
# Absolute path to this script
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in
SCRIPT_PATH=$(dirname "$SCRIPT")
SCRIPT_BASE=$(basename "$SCRIPT")

: ${TAC_IMAGE_FILE="$1"}
: ${TAC_IMAGE_DIR="$( mktemp -du )"}

: ${TARGET_IMG="disk.img"}
: ${ROOTFS="${TAC_IMAGE_DIR}/rootfs.tar.gz"}
: ${MIDDLEWARE="${TAC_IMAGE_DIR}/middleware.tgz"}
: ${KERNEL="${TAC_IMAGE_DIR}/bzImage"}
: ${INITRAM="${TAC_IMAGE_DIR}/rootfs.cpio.gz"}
: ${SYSLINUX_CFG="${TAC_IMAGE_DIR}/syslinux.cfg"}

: ${BOOT_PART_SIZE=20}
: ${ROOTFS_PART_SIZE=500}
: ${SPACER=1}
: ${DEBUG=0}

if [ -z ${TAC_IMAGE_FILE} ]; then
    echo "usage: ${SCRIPT_BASE} TAC_IMAGE"
    exit 1
fi

cmd() {
	echo "    $1"
	if [ ${DEBUG} -lt 1 ]; then
		$1
	fi
}

cleanup() {
    echo "image: cleanup..."
	cmd "umount ${OE_TMP}"
	cmd "umount ${LOOP}"
    cmd "losetup -d ${LOOP}"
    cmd "rm -rf ${OE_TMP}"
}

error() {
    echo "Error: $1"
	cleanup
    exit 99
}

MiB() {
    echo $(( $1 * 1024 * 1024 ))
}

Sector() {
    echo $(( $1 / 512 ))
}

create_part() {
	START=$(( $2 +1 ))
	END=$(( $START + $3 ))
	FS_TYPE=$4
	local OFF=$5
	echo "Creating partition $1: ($START,$END) ..."
	cmd "parted -s ${LOOP} -a min unit s mkpart primary $FS_TYPE ${START} ${END}"
	cmd "sync"
	eval $OFF=$END
}

extract_image() {
	if [ ! -f ${TAC_IMAGE_FILE} ]; then
		error "Could not find ${TAC_IMAGE_FILE}"
	fi

	cmd "mkdir -p ${TAC_IMAGE_DIR}"
	cmd "tar xf ${TAC_IMAGE_FILE} -C ${TAC_IMAGE_DIR}"
}

# extract image
extract_image

# ensure loop
modprobe loop

# setup env
OE_TMP=$(mktemp -d)
LOOP=$(losetup -f)
DISK_SIZE="$(( $BOOT_PART_SIZE + $SPACER + $ROOTFS_PART_SIZE + $SPACER ))"

# create an image
echo "Creating image file: ${TARGET_IMG}..."
cmd "dd if=/dev/zero of=${TARGET_IMG} bs=512 count=$(Sector $(MiB $DISK_SIZE)) status=none"
sync

# ---------------------------------------------------------------------------------
# create partition table
# ---------------------------------------------------------------------------------

echo "Creating partition table ..."
cmd "losetup ${LOOP} ${TARGET_IMG}"
cmd "parted -s ${LOOP} mklabel msdos"
sync

OFFSET=0

# ---------------------------------------------------------------------------------
# create partition boot
# ---------------------------------------------------------------------------------

# create partition
create_part "boot" 2047 $(Sector $(MiB $BOOT_PART_SIZE)) fat16 OFFSET
cmd "parted -s ${LOOP} set 1 boot on"

# create filesystem
echo "Creating filesystem on partition boot ..."
cmd "mkfs.vfat -F 16 ${LOOP}p1"
cmd "sync"

# mount
echo "Mounting boot partition on $OE_TMP..."
cmd "mount ${LOOP}p1 ${OE_TMP}"

# copy files
echo "Copying kernel/dtb to boot partition ..."
cmd "cp ${KERNEL} ${OE_TMP}"
cmd "cp ${INITRAM} ${OE_TMP}"
cmd "cp ${SYSLINUX_CFG} ${OE_TMP}"

# unmount
echo "Unmounting boot partition ..."
cmd "sync"
cmd "umount ${OE_TMP}"


# ---------------------------------------------------------------------------------
# create partition rootfs
# ---------------------------------------------------------------------------------

# create partition
create_part "rootfs" ${OFFSET} $(Sector $(MiB $ROOTFS_PART_SIZE)) ext4 OFFSET

# create filesystem
echo "Creating rootfs filesystem ..."
cmd "mkfs.ext4 -q ${LOOP}p2"
cmd "sync"

# mount
echo "Mounting rootfs partition on $OE_TMP..."
cmd "mount ${LOOP}p2 ${OE_TMP}"

# Extract rootfs files
echo "Extracting rootfs ..."
cmd "tar xf ${ROOTFS} -C ${OE_TMP}"

# Extract middleware files
echo "Extracting middleware ..."
cmd "mkdir -p ${OE_TMP}/opt/tac/"
cmd "tar xf ${MIDDLEWARE} -C ${OE_TMP}/opt/tac"

# unmount
echo "Unmounting rootfs ..."
cmd "sync"
cmd "umount ${OE_TMP}"


# ---------------------------------------------------------------------------------
# Prepare syslinux boot
# ---------------------------------------------------------------------------------
# Wtire MBR
echo "Writing MBR"
cmd "dd if=/usr/lib/syslinux/bios/mbr.bin of=${TARGET_IMG} bs=440 count=1 status=none conv=notrunc"
sync

# Write booloader
echo "Writing syslinuc bootloader to ${LOOP}p1"
cmd "syslinux ${LOOP}p1"
cmd "sync"

# cleanup
cleanup

