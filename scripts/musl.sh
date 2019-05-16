#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e
set -x

STAGE=$(readlink -f .)/build
INSTALL_DIR=$STAGE/musl
MUSL_ROOT=$STAGE/musl-1.1.22

mkdir -p $INSTALL_DIR

cd $MUSL_ROOT
./configure --disable-shared --prefix=$INSTALL_DIR
make
make install
