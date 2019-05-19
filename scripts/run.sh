#!/usr/bin/env bash

set -e

TOP=$(readlink -f build)/monolinux

qemu-system-x86_64 \
    -kernel $TOP/obj/monolinux/arch/x86/boot/bzImage \
    -initrd $TOP/initramfs.cpio \
    -nographic -append "console=ttyS0"
