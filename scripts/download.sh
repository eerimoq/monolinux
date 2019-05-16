#!/usr/bin/env bash
#
# Must be run from the repository root.
#

set -e
set -x

# Taken from
# https://www.centennialsoftwaresolutions.com/blog/build-the-linux-kernel-and-busybox-and-run-them-on-qemu.

# 3. Create a workspace:
ROOT=$(readlink -f .)
STAGE=$ROOT/build
TOP=$STAGE/teeny-linux
mkdir -p $STAGE

# 4. Download and extract the Linux kernel and BusyBox
cd $STAGE
curl https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.10.6.tar.xz | tar xJf -
curl https://www.musl-libc.org/releases/musl-1.1.22.tar.gz | tar xzf -
