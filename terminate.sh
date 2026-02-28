#!/bin/bash
# terminate.sh
MOUNT_POINT="./mount"
DEVICE_FILE="./<device-file>"

# Unmount the disk image
umount $MOUNT_POINT

# Detach the loop device
LOOP_DEVICE=$(readlink $DEVICE_FILE)
losetup -d $LOOP_DEVICE

# Remove the symbolic link and mount point
rm -f $DEVICE_FILE

