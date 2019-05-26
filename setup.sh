# Monolinux root folder.
export ML_ROOT=$(readlink -f .)

# Path to source code archives.
export ML_SOURCES=$ML_ROOT/../sources

# Linux kernel version to use.
export ML_LINUX_VERSION=5.1.3

# Linux kernel configuration.
export ML_LINUX_CONFIG=$ML_ROOT/configs/x86_64-default.config

export PATH=$PATH:$ML_ROOT/bin

# Cross compilation.

export CROSS_COMPILE=
# autotools: The system where built programs and libraries will run.
export ML_AUTOTOOLS_HOST=

# For ARM.
# export ARCH=arm
# export CROSS_COMPILE=arm-linux-musleabi-
# export ML_AUTOTOOLS_HOST=--host=arm-linux-musleabi
