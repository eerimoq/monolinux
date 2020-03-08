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
    echo "Copying $file to /root in the initramfs."
    cp $file $BUILD/initramfs/root
done

cd $BUILD/initramfs
find . | cpio -H newc -o > ../initramfs.cpio
