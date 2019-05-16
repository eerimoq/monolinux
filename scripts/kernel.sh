#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e
set -x

# 3. Create a workspace:
ROOT=$(readlink -f .)
STAGE=$ROOT/build
TOP=$STAGE/teeny-linux
mkdir -p $STAGE

# 12. Config the Linux kernel with the minimal config
cd $STAGE/linux-4.10.6
make O=$TOP/obj/linux-x86-allnoconfig allnoconfig

# 13. Turn options on for QEMU:
cd $STAGE/linux-4.10.6
# make O=$TOP/obj/linux-x86-allnoconfig nconfig
cp $ROOT/configs/linux-x86-allnoconfig.config $TOP/obj/linux-x86-allnoconfig/.config

# 15. Make the kernel:
cd $STAGE/linux-4.10.6
make O=$TOP/obj/linux-x86-allnoconfig
