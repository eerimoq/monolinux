|buildstatus|_
|codecov|_

Monolinux
=========

An embedded Linux distro with a single statically linked executable,
because smaller is better!

Build and run
=============

Install all prerequisites:

.. code-block:: shell

   $ sudo apt install curl qemu-system-x86 flex bison

   # Only required for unit testing:
   $ git clone https://github.com/eerimoq/unicorn
   $ (cd unicorn && make && sudo make install)
   $ sudo pip3 install pyinotify

Source the development environment setup script.

.. code-block:: shell

   $ source setup.sh

Download the Linux kernel sources to the ``$ML_SOURCES``
directory. This is normally only done once.

.. code-block:: shell

   $ cd $ML_SOURCES
   $ wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.1.3.tar.xz
   $ cd -

Run the commands below to create a file system, build the Linux kernel
and run everything in QEMU.

.. code-block:: shell

   $ make -C examples/hello_world run
   ...
   Hello world!
   Hello world!
   Hello world!
   ...

Exit QEMU with Ctrl-A C and then q <Enter>.

Unit testing
============

Execute all unit tests.

.. code-block:: shell

   $ make -s -j4
   ...

Configuration
=============

Environment variables exported in setup.sh. Change at will.

.. code-block:: shell

   $ env | grep ML_
   ML_LINUX_VERSION=5.1.3
   ML_LINUX_CONFIG=/home/erik/workspace/monolinux/configs/x86_64-default.config
   ML_SOURCES=/home/erik/workspace/monolinux/../sources
   ML_ROOT=/home/erik/workspace/monolinux

Linux kernel
------------

Unpack the Linux kernel archive.

.. code-block:: shell

   $ cd examples/hello_world
   $ make unpack
   $ cd build/linux-$ML_LINUX_VERSION

Create a new Linux kernel configuration based on the all-no
configuration.

.. code-block:: shell

   $ make allnoconfig
   $ make nconfig

Start using the default configuraiton.

.. code-block:: shell

   $ cp .config $ML_LINUX_CONFIG

or

.. code-block:: shell

   $ export ML_LINUX_CONFIG=$(readlink -f .config)

Cross compilation
=================

ARM 32 bits with MUSL.

.. code-block:: shell

   $ wget https://musl.cc/arm-linux-musleabi-cross.tgz
   $ tar xf https://musl.cc/arm-linux-musleabi-cross.tgz
   $ export PATH=$PATH:$(readlink -f arm-linux-musleabi-cross/bin)
   $ make ARCH=arm CROSS_COMPILE=arm-linux-musleabi- ML_LINUX_CONFIG=$ML_ROOT/configs/arm-all-no.config

ARM 64 bits.

.. code-block:: shell

   $ sudo apt install gcc-aarch64-linux-gnu
   $ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- ML_LINUX_CONFIG=$ML_ROOT/configs/arm64-all-no.config

File tree
=========

This is the file tree of the Monolinux repository.

.. code-block:: text

   monolinux/                   - this repository
   ├── bin/                     - executables
   ├── configs/                 - a few Linux kernel configs
   ├── examples/                - example applications
   ├── LICENSE                  - license
   ├── make/                    - build system
   │   └── packages/            - packages build specifications
   ├── ml/                      - the Monolinux C library
   └── setup.sh                 - development environment setup script

User project file tree before build.

.. code-block:: text

   my-project/                  - my Monolinux project
   ├── app/
   │   ├── main.c               - application entry point
   │   └── Makefile             - project makefile
   ├── monolinux/               - Monolinux
   └── setup.sh                 - environment setup script

User project file tree after build.

.. code-block:: text

   my-project/
   ├── app/
   │   ├── main.c
   │   └── Makefile
   ├── build/                   - all build output
   │   ├── app                  - the only executable
   │   ├── initramfs/           - unpacked ramfs
   │   ├── initramfs.cpio       - packed ramfs
   │   ├── linux-5.1.3/         - Linux source and build output
   │   ├── packages/            - packages source and objects
   │   │   └── curl-7.65.0/
   │   └── root/                - headers and libraries container
   │       ├── bin/
   │       ├── include/         - include files
   │       │    └── curl/
   │       │        └── curl.h
   │       ├── lib/             - static libraries
   │       │    └── libcurl.a
   │       └── share/
   ├── monolinux/
   └── setup.sh

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux

.. |codecov| image:: https://codecov.io/gh/eerimoq/monolinux/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/monolinux
