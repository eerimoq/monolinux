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
make O=$TOP/obj/monolinux allnoconfig
cp $ML_LINUX_CONFIG $TOP/obj/monolinux/.config
make O=$TOP/obj/monolinux
