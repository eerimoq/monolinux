#!/usr/bin/env bash

set -e

# Taken from
# https://www.centennialsoftwaresolutions.com/blog/build-the-linux-kernel-and-busybox-and-run-them-on-qemu.

# 3. Create a workspace:
BUILD=$(readlink -f build)
TOP=$BUILD/monolinux
mkdir -p $BUILD

# 8. Build the directory structure of the initramfs
mkdir -pv $TOP/initramfs/x86-busybox
cd $TOP/initramfs/x86-busybox
mkdir -pv {bin,dev,sbin,etc,proc,sys/kernel/debug,usr/{bin,sbin},lib,lib64,mnt/root,root}
cp -av /dev/{null,console,tty,sda1} $TOP/initramfs/x86-busybox/dev/

# 9. Create init and make it executable
cp $BUILD/app $TOP/initramfs/x86-busybox/init

# 10. Make init executable:
chmod +x $TOP/initramfs/x86-busybox/init

# 11. Create the initramfs:
cd $TOP/initramfs/x86-busybox
find . | cpio -H newc -o > ../../initramfs.cpio
