UDP send and receive
====================

Monolinux:

.. code-block:: shell

   $ ifconfig eth0 192.168.0.100 255.255.255.0
   $ ifconfig eth0 up

PC:

.. code-block:: python

   >>> import socket
   >>> s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   >>> s.bind(('192.168.0.4', 9999))
   >>> s.recv(1000)

Monolinux:

.. code-block:: shell

   $ udp send 192.168.0.4 9999 monolinux-to-pc
   $ udp recv 10000

PC:

.. code-block:: python

   >>> s.sendto(b'pc-to-monolinux', 0, ('192.168.0.100', 10000))
