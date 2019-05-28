#!/usr/bin/env bash

set -e

BUILD=$1

mkdir -p $BUILD/initramfs
cd $BUILD/initramfs
mkdir -p {dev,proc,sys/kernel/debug}
cp -a /dev/{null,console,tty,sda1,urandom} dev
cp $BUILD/app init
chmod +x init
find . | cpio -H newc -o > ../initramfs.cpio
