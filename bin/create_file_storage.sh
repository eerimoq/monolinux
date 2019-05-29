#!/usr/bin/env bash

set -e

DISK=mldisk.img
LOOP_DEV=/dev/loop20
MOUNT_POINT=foo

# Create a partition.
qemu-img create -f raw $DISK 1G
fdisk $DISK <<EOF
n




w
EOF

# Create a file system. 2048 * 512 = 1048576.
sudo losetup -o 1048576 $LOOP_DEV $DISK
sudo mkfs.ext4 $LOOP_DEV

# Write an empty file to the file system.
mkdir $MOUNT_POINT
sudo mount $LOOP_DEV $MOUNT_POINT
sudo touch $MOUNT_POINT/hello.txt
sudo umount $MOUNT_POINT

sudo losetup -d $LOOP_DEV
