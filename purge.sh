#!/bin/bash
# purge.sh
MOUNT_POINT="./mount"
DEVICE_FILE="./<device-file>"
IMAGE="storage_vgc.img"

# Unmount the disk image if mounted
umount $MOUNT_POINT 2>/dev/null

# Detach the loop device
LOOP_DEVICE=$(readlink $DEVICE_FILE)
losetup -d $LOOP_DEVICE 2>/dev/null

# Remove the symbolic link, mount point, and disk image file
rm -f $DEVICE_FILE
rm -rf $MOUNT_POINT
rm -f $IMAGE

