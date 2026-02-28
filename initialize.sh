#!/bin/bash
# initialize.sh
# This script initializes the disk image file



IMAGE="storage_vgc.img"
SIZE="100M"

# Create a new disk image file
dd if=/dev/zero of=$IMAGE bs=1M count=${SIZE%M}

# Format the disk image as ext4
sudo mkfs.ext4 $IMAGE

