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

  - `mbedTLS`_ (TLS, SSL, crypto, ...)

  - `zlib`_

  - `xz`_ (LZMA)

  - `detools`_ (delta encoding)

ToDo:

- ...

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

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux

.. _Monolinux Jiffy: https://github.com/eerimoq/monolinux-jiffy

.. _Linux kernel: https://www.kernel.org/

.. _ml: https://github.com/eerimoq/monolinux-c-library

.. _curl: https://curl.haxx.se/

.. _OpenSSL: https://www.openssl.org/

.. _mbedTLS: https://tls.mbed.org/

.. _zlib: https://zlib.net/

.. _xz: https://tukaani.org/xz/

.. _detools: https://github.com/eerimoq/detools
