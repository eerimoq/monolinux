|buildstatus|_
|codecov|_

Monolinux
=========

An embedded Linux distro with a single statically linked executable,
because smaller is better!

Monolinux is primarily targeting the `C` programming language.

Features:

- Everything the `Linux kernel`_ provides.

  - File systems.

  - Networking.

  - Device drivers.

  - ...

- Libraries to statically link with your application.

  - `ml`_ (the Monolinux library)

  - `curl`_ (HTTP, FTP, ...)

  - `OpenSSL`_ (TLS, SSL, crypto, ...)

  - `zlib`_

  - `xz`_ (LZMA)

  - `detools`_ (delta encoding)

Build and run
=============

Install all prerequisites:

.. code-block:: shell

   $ sudo apt install curl qemu-system-x86 flex bison

   # Only required for unit testing:
   $ git clone https://github.com/eerimoq/unicorn
   $ (cd unicorn && make && sudo make install)
   $ sudo pip3 install pyinotify
   $ cd ..
   $ wget https://musl.cc/x86_64-linux-musl-cross.tgz
   $ tar xf x86_64-linux-musl-cross.tgz
   $ cd -

Source the development environment setup script.

.. code-block:: shell

   $ source setup.sh

Download the Linux kernel sources to the ``$ML_SOURCES``
directory. This is normally only done once.

.. code-block:: shell

   $ mkdir $ML_SOURCES
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

Automatically build and run a test suite when one of its files has
been modified.

.. code-block:: text

   $ cd ml/tst/shell
   $ ml test
   ...
   ================================================================================
   CC main.c
   LD /home/erik/workspace/monolinux/ml/tst/shell/build/suite

   Running tests...

        1 - 6 |  ......

   Test results:

       PASS various_commands (50.85ms)
       PASS ls (50.77ms)
       PASS cat (50.95ms)
       PASS command_editing (50.90ms)
       PASS quotes (50.73ms)
       PASS history (50.62ms)

   Tests: 6 passed, 6 total
   Time:  352.57ms
   ================================================================================
   CC main.c
   LD /home/erik/workspace/monolinux/ml/tst/shell/build/suite

   Running tests...

   <more output>

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

User project file tree before build. These files are normally under
version control.

See `Monolinux Jiffy`_ for a real example project.

.. code-block:: text

   my-project/                  - my Monolinux project
   ├── app/
   │   ├── main.c               - application entry point
   │   └── Makefile             - project makefile
   ├── Makefile                 - top level makefile
   ├── monolinux/               - Monolinux
   ├── README.rst               - readme
   └── setup.sh                 - environment setup script

User project file tree after build.

.. code-block:: text

   my-project/
   ├── app/
   │   ├── build/                   - all build output
   │   │   ├── app                  - the one and only executable
   │   │   ├── initramfs/           - unpacked ramfs
   │   │   ├── initramfs.cpio       - packed ramfs
   │   │   ├── linux-5.1.3/         - Linux source and build output
   │   │   ├── packages/            - packages source and objects
   │   │   │   └── curl-7.65.0/
   │   │   └── root/                - headers and libraries container
   │   │       ├── bin/
   │   │       ├── include/         - include files
   │   │       │    └── curl/
   │   │       │        └── curl.h
   │   │       ├── lib/             - static libraries
   │   │       │    └── libcurl.a
   │   │       └── share/
   │   ├── main.c
   │   └── Makefile
   ├── Makefile
   ├── monolinux/
   ├── README.rst
   └── setup.sh

Configuration
=============

Environment variables exported in setup.sh. Change at will.

.. code-block:: shell

   $ env | grep ML_
   ML_LINUX_VERSION=5.1.3
   ML_LINUX_CONFIG=/home/erik/workspace/monolinux/configs/x86_64-default.config
   ML_SOURCES=/home/erik/workspace/monolinux/../sources
   ML_ROOT=/home/erik/workspace/monolinux
   ML_AUTOTOOLS_HOST=

``ARCH`` and ``CROSS_COMPILE`` are also used when cross-compiling.

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

Cross compilation
=================

See `Monolinux Jiffy`_ for a real example project.

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

Useful
======

Configure eth0 in Monolinux in QEMU and send a UDP packet.

.. code-block:: shell

   $ ifconfig eth0 10.0.2.15 255.255.255.0
   $ ifconfig eth0 up
   $ udp_send 10.0.2.2 9999 hej
   $ http_get http://10.0.2.2:9999/
   $ http_get https://10.0.2.2/

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux

.. |codecov| image:: https://codecov.io/gh/eerimoq/monolinux/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/monolinux

.. _Monolinux Jiffy: https://github.com/eerimoq/monolinux-jiffy

.. _Linux kernel: https://www.kernel.org/

.. _ml: https://github.com/eerimoq/monolinux/tree/master/ml

.. _curl: https://curl.haxx.se/

.. _OpenSSL: https://www.openssl.org/

.. _zlib: https://zlib.net/

.. _xz: https://tukaani.org/xz/

.. _detools: https://github.com/eerimoq/detools
