#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e
set -x

# Taken from
# https://www.centennialsoftwaresolutions.com/blog/build-the-linux-kernel-and-busybox-and-run-them-on-qemu.

# 3. Create a workspace:
ROOT=$(readlink -f .)
STAGE=$ROOT/build
TOP=$STAGE/teeny-linux
mkdir -p $STAGE

# 8. Build the directory structure of the initramfs
mkdir -pv $TOP/initramfs/x86-busybox
cd $TOP/initramfs/x86-busybox
mkdir -pv {bin,dev,sbin,etc,proc,sys/kernel/debug,usr/{bin,sbin},lib,lib64,mnt/root,root}
cp -av /dev/{null,console,tty,sda1} $TOP/initramfs/x86-busybox/dev/

# 9. Create init and make it executable
cp $STAGE/app $TOP/initramfs/x86-busybox/init

# 10. Make init executable:
chmod +x $TOP/initramfs/x86-busybox/init

# 11. Create the initramfs:
cd $TOP/initramfs/x86-busybox
find . | cpio -H newc -o > ../initramfs.cpio
cd ..
cat initramfs.cpio | gzip > $TOP/initramfs.igz
