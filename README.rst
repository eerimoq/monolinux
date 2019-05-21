|buildstatus|_

Monolinux
=========

An embedded Linux distro with a single statically linked executable,
because smaller is better!

Build and run
=============

First of all, install a few packages:

.. code-block:: shell

   $ sudo apt-get install curl qemu-system-x86 flex bison

Then download the sources to the ``$ML_SOURCES`` directory. This is
normally only done once.

.. code-block:: shell

   $ cd ..
   $ wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.1.3.tar.xz
   $ wget https://www.musl-libc.org/releases/musl-1.1.22.tar.gz
   $ cd -

Now, run the commands below to create a file system, build the Linux
kernel and run everything in QEMU.

.. code-block:: shell

   $ source setup.sh
   $ make -C examples/hello_world run
   ...
   Hello world!
   Hello world!
   Hello world!
   ...

Exit QEMU with Ctrl-A C and then q <Enter>.

Configuration
=============

Environment variables exported in setup.sh. Change at will.

.. code-block:: shell

   $ env | grep ML_
   ML_LINUX_VERSION=5.1.3
   ML_LINUX_CONFIG=/home/erik/workspace/monolinux/configs/x86_64-default.config
   ML_SOURCES=/home/erik/workspace/monolinux/..
   ML_ROOT=/home/erik/workspace/monolinux
   ML_MUSL_VERSION=1.1.22

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

Ideas
=====

- Cross compilation. Something like this?

  .. code-block:: shell

     # Use configs/jiffy.config and cross compiler.
     $ make BOARD=jiffy

     # 64 bits.
     $ sudo apt install gcc-aarch64-linux-gnu

     $ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- allnoconfig
     $ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- nconfig
     $ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j4

     $ ARCH=aarch64 CC=aarch64-linux-gnu-gcc ./configure --disable-shared --prefix=../musl
     $ make

     # 32 bits.
     $ sudo apt install gcc-arm-linux-gnueabi

     $ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- allnoconfig
     $ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- nconfig
     $ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -j4

     $ ARCH=arm CC=arm-linux-gnueabi-gcc ./configure --disable-shared --prefix=../musl
     $ make

- Run on some embedded system.

- Add unit tests of the Monolinux library.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux
