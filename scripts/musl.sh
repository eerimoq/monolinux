#!/usr/bin/env bash

set -e

BUILD=$(readlink -f build)
INSTALL_DIR=$BUILD/musl
MUSL_ROOT=$BUILD/musl-$ML_MUSL_VERSION

mkdir -p $INSTALL_DIR

cd $MUSL_ROOT
./configure --disable-shared --prefix=$INSTALL_DIR
make
make install
