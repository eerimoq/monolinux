#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e

STAGE=$(readlink -f build)
TOP=$STAGE/monolinux

qemu-system-x86_64 \
    -kernel $TOP/obj/monolinux/arch/x86/boot/bzImage \
    -initrd $TOP/initramfs.cpio \
    -nographic -append "console=ttyS0"
