#!/bin/bash
# startup.sh
# This script mounts the disk image and sets up the environment

IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="./<device-file>"

# Create a loop device for the image file
LOOP_DEVICE=$(sudo losetup -f --show $IMAGE)

# Create a symbolic link to the loop device
ln -s $LOOP_DEVICE $DEVICE_FILE

# Create the mount directory if it does not exist
mkdir -p $MOUNT_POINT

# Mount the loop device
mount $IMAGE $MOUNT_POINT



