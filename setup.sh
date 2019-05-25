# Monolinux root folder.
export ML_ROOT=$(readlink -f .)

# Path to source code archives.
export ML_SOURCES=$ML_ROOT/..

# Linux kernel version to use.
export ML_LINUX_VERSION=5.1.3

# MUSL version to use.
export ML_MUSL_VERSION=1.1.22

# Linux kernel configuration.
export ML_LINUX_CONFIG=$ML_ROOT/configs/x86_64-default.config

export PATH=$PATH:$ML_ROOT/bin
