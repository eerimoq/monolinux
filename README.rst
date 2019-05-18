|buildstatus|_

Monolinux
=========

An embedded Linux distro with a single statically linked executable,
because smaller is better!

Build and run
=============

First of all, install a few packages:

.. code-block:: text

   sudo apt-get install curl qemu-system-x86 flex bison

Then download the sources into ``$ML_SOURCES``. This is normally only
done once.

.. code-block:: text

   cd ..
   wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.1.3.tar.xz
   wget https://www.musl-libc.org/releases/musl-1.1.22.tar.gz
   cd -

Now, run the commands below to create a file system, build the Linux
kernel and run everything in QEMU.

.. code-block:: text

   $ source setup.sh
   $ make -C examples/hello_world

Configuration
=============

Environment variables exported in setup.sh. Change at will.

.. code-block:: text

   $ env | grep ML_
   ML_LINUX_VERSION=5.1.3
   ML_SOURCES=/home/erik/workspace/monolinux/..
   ML_ROOT=/home/erik/workspace/monolinux
   ML_MUSL_VERSION=1.1.22

ToDo: Add some information about Linux kernel configuration.

Ideas
=====

- Cross compilation.

- Board support. How?

- Run on some embedded system.

- Add unit tests of the monolinux library.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux
