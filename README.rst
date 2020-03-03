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

  - `mbedTLS`_ (TLS, SSL, crypto, ...)

  - `zlib`_

  - `xz`_ (LZMA)

  - `detools`_ (delta encoding)

ToDo:

- ...

Creating a project
==================

A minimal Monolinux project's file tree looks like below. Add needed
libraries to the `3pp`.

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

Here are two example projects:

- `An Monolinux example project`_

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

.. _ml: https://github.com/eerimoq/monolinux-c-library

.. _curl: https://curl.haxx.se/

.. _mbedTLS: https://tls.mbed.org/

.. _zlib: https://zlib.net/

.. _xz: https://tukaani.org/xz/

.. _detools: https://github.com/eerimoq/detools

.. _An Monolinux example project: https://github.com/eerimoq/monolinux-example-project

.. _Monolinux Jiffy: https://github.com/eerimoq/monolinux-jiffy
