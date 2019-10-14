|buildstatus|_

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

  - `ml`_ (the Monolinux C library)

  - `curl`_ (HTTP, FTP, ...)

  - `OpenSSL`_ (TLS, SSL, crypto, ...)

  - `zlib`_

  - `xz`_ (LZMA)

  - `detools`_ (delta encoding)

ToDo:

- ...

Build and run
=============

Install all prerequisites:

.. code-block:: shell

   $ sudo apt install curl qemu-system-x86 flex bison libelf-dev
   $ wget https://musl.cc/x86_64-linux-musl-cross.tgz
   $ tar xf x86_64-linux-musl-cross.tgz

Source the development environment setup script.

.. code-block:: shell

   $ source setup.sh

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
   │   │       │   └── curl/
   │   │       │       └── curl.h
   │   │       ├── lib/             - static libraries
   │   │       │   └── libcurl.a
   │   │       └── share/
   │   ├── main.c
   │   └── Makefile
   ├── Makefile
   ├── monolinux/
   ├── README.rst
   └── setup.sh

Configuration
=============

In general, just modify any files in this repository to match your
project's needs.

There are a few environment variables exported in ``setup.sh``.

Cross compilation
=================

See `Monolinux Jiffy`_ for an example project.

Tips and tricks
===============

See ``man procfs`` for details of the following commands.

.. code-block:: shell

   $ cat /proc/devices
   $ cat /proc/meminfo
   $ cat /proc/modules
   $ cat /proc/net/arp
   $ cat /proc/net/dev
   $ cat /proc/net/protocols
   $ cat /proc/net/route
   $ cat /proc/uptime
   $ cat /proc/version

Disk commands.

.. code-block:: shell

   $ ls /sys/block
   $ mount /dev/sda1 /mnt/disk ext4
   $ ls /mnt/disk
   $ cat /mnt/disk/README

Configuration.

.. code-block:: shell

   # DNS.
   $ cat /etc/resolv.conf

Device tree.

.. code-block:: shell

   $ cat /sys/firmware/devicetree/base/...

Sources
=======

Sources can be found here: https://github.com/eerimoq/monolinux-sources

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux

.. _Monolinux Jiffy: https://github.com/eerimoq/monolinux-jiffy

.. _Linux kernel: https://www.kernel.org/

.. _ml: https://github.com/eerimoq/monolinux-c-library

.. _curl: https://curl.haxx.se/

.. _OpenSSL: https://www.openssl.org/

.. _zlib: https://zlib.net/

.. _xz: https://tukaani.org/xz/

.. _detools: https://github.com/eerimoq/detools
