#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e

# 3. Create a workspace:
STAGE=$(readlink -f build)
TOP=$STAGE/monolinux
mkdir -p $STAGE

cd $STAGE/linux-$ML_LINUX_VERSION
make O=$TOP/obj/linux-x86-allnoconfig allnoconfig
cp $ML_ROOT/configs/linux-x86-allnoconfig.config $TOP/obj/linux-x86-allnoconfig/.config
make O=$TOP/obj/linux-x86-allnoconfig
