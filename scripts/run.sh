#!/usr/bin/env bash
#
# Must be run from the repository root.
#

STAGE=$(readlink -f build)
TOP=$STAGE/teeny-linux

qemu-system-x86_64 \
    -kernel $TOP/obj/linux-x86-allnoconfig/arch/x86/boot/bzImage \
    -initrd $TOP/initramfs.igz \
    -nographic -append "earlyprintk=serial,ttyS0 console=ttyS0"
