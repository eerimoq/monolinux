#!/usr/bin/env bash

set -e

BUILD=$(readlink -f build)
TOP=$BUILD/monolinux
mkdir -p $BUILD

cd $BUILD/linux-$ML_LINUX_VERSION
make O=$TOP/obj/monolinux allnoconfig
cp $ML_LINUX_CONFIG $TOP/obj/monolinux/.config
make O=$TOP/obj/monolinux
