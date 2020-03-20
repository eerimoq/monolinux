#!/usr/bin/env bash

set -e

BUILD=$1
INITRAMFS_FILES=$2

rm -rf $BUILD/initramfs
mkdir -p $BUILD/initramfs
cd $BUILD/initramfs
mkdir root
cp $BUILD/app init
chmod +x init
cd - > /dev/null

for file in $INITRAMFS_FILES ; do
    cp $file $BUILD/initramfs/root
done

cd $BUILD/initramfs
# find . | cpio --quiet -H newc -o | lz4 -9 -l > ../initramfs.cpio.lz4
find . | cpio --quiet -H newc -o > ../initramfs.cpio
