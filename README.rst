Monolinux
=========

Create embedded Linux systems with a single statically linked
executable, because smaller is better!

Monolinux is primarily targeting the `C` programming language.

Features:

- Everything the `Linux kernel`_ provides.

  - File systems.

  - Networking.

  - Device drivers.

  - ...

- Libraries to statically link with your application.

  - `async`_ (asynchronous framework)

  - `bitstream`_ (bit packing and unpacking)

  - `bunga`_ (OAM over TCP/IP)

  - `curl`_ (HTTP, FTP, ...)

  - `dbg-macro`_ (printf debugging)

  - `detools`_ (delta encoding)

  - `heatshrink`_ (a compression algorithm)

  - `humanfriendly`_ (various utilities)

  - `mbedTLS`_ (TLS, SSL, crypto, ...)

  - `messi`_ (message passing in distributed systems)

  - `monolinux-c-library`_ (the Monolinux C library)

  - `pbtools`_ (Google Protocol Buffers)

  - `xz`_ (LZMA)

  - `zlib`_

Creating a project
==================

A minimal Monolinux project's file tree looks like below. Add needed
software packages to the `3pp` folder.

.. code-block:: text

   my-project/
   ├── 3pp/
   │   ├── arm-linux-musleabi-cross.tgz  - a toolchain (if needed)
   │   ├── linux/                        - the linux kernel
   │   └── monolinux/                    - this repository as a submodule
   ├── app/                              - your application
   │   ├── main.c
   │   └── Makefile
   ├── LICENSE
   ├── Makefile
   └── setup.sh                          - development environment setup script

Here are more example projects:

- `An Monolinux example project`_

- `Monolinux Raspberry Pi 3`_

- `Monolinux Jiffy`_

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
   $ cat /proc/mounts

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

.. _Linux kernel: https://www.kernel.org/

.. _async: https://github.com/eerimoq/async

.. _bitstream: https://github.com/eerimoq/bitstream

.. _bunga: https://github.com/eerimoq/bunga

.. _curl: https://curl.haxx.se/

.. _dbg-macro: https://github.com/eerimoq/dbg-macro

.. _detools: https://github.com/eerimoq/detools

.. _heatshrink: https://github.com/atomicobject/heatshrink

.. _humanfriendly: https://github.com/eerimoq/humanfriendly

.. _mbedTLS: https://tls.mbed.org/

.. _messi: https://github.com/eerimoq/messi

.. _monolinux-c-library: https://github.com/eerimoq/monolinux-c-library

.. _pbtools: https://github.com/eerimoq/pbtools

.. _xz: https://tukaani.org/xz/

.. _zlib: https://zlib.net/

.. _An Monolinux example project: https://github.com/eerimoq/monolinux-example-project

.. _Monolinux Raspberry Pi 3: https://github.com/eerimoq/monolinux-raspberry-pi-3

.. _Monolinux Jiffy: https://github.com/eerimoq/monolinux-jiffy
