#!/usr/bin/env bash

set -e

BUILD=$1

mkdir -p $BUILD/initramfs
cd $BUILD/initramfs
mkdir -p {bin,dev,sbin,etc,proc,sys/kernel/debug,usr/{bin,sbin},lib,lib64,mnt/root,root}
cp -a /dev/{null,console,tty,sda1} dev
cp $BUILD/app init
chmod +x init
find . | cpio -H newc -o > ../initramfs.cpio
