|buildstatus|_

Monolinux
=========

Single process Linux.

Plan
====

- Networking.

Build and run
=============

First of all, install a few packages:

.. code-block:: text

   sudo apt-get install curl qemu-system-x86

Now, run the command below to create a file system, build the Linux
kernel and run everything in QEMU.

.. code-block:: text

   $ make

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux
