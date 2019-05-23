#!/usr/bin/env bash

set -e

BUILD=$1

mkdir -p $BUILD
mkdir -pv $BUILD/initramfs
cd $BUILD/initramfs
mkdir -pv {bin,dev,sbin,etc,proc,sys/kernel/debug,usr/{bin,sbin},lib,lib64,mnt/root,root}
cp -av /dev/{null,console,tty,sda1} dev
cp $BUILD/app init
chmod +x init
find . | cpio -H newc -o > ../initramfs.cpio
