Sample output
=============

.. code-block:: text

   $ help
   Cursor movement

            LEFT   Go left one character.
           RIGHT   Go right one character.
     HOME/Ctrl+A   Go to the beginning of the line.
      END/Ctrl+E   Go to the end of the line.

   Edit

           Alt+D   Delete the word at the cursor.
          Ctrl+D   Delete the chracter at the cursor.
          Ctrl+K   Cut the line from cursor to end.
          Ctrl+T   Swap the last two characters before the cursor (typo).
             TAB   Tab completion for file/directory names.
       BACKSPACE   Delete the character before the cursor.

   History

              UP   Previous command.
            DOWN   Next command.
          Ctrl+R   Recall the last command including the specified character(s)
                   searches the command history as you type.
          Ctrl+G   Escape from history searching mode.

   Commands

             cat   Print a file.
           hello   My command.
            help   Print this help.
         history   List comand history.
        ifconfig   Network interface management.
              ls   List directory contents.
        udp_recv   UDP receive.
        udp_send   UDP send.
   $ ls
   ./
   ../
   proc/
   init
   sys/
   dev/
   OK
   $

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

   $ udp_send 192.168.0.4 9999 monolinux-to-pc
   $ udp_recv 10000

PC:

.. code-block:: python

   >>> s.sendto(b'pc-to-monolinux', 0, ('192.168.0.100', 10000))
