# design

## user interface

The user shall have a `tcp_socket` module to interact with.
This module manages a `tcp_socket` struct, an object that under the hood uses a raw socket that lets the library control the TCP header's contents.
This module will have the following modules:

```
tcp_socket_init
tcp_socket_destroy
tcp_socket_bind
tcp_socket_connect
tcp_socket_listen
tcp_socket_accept
tcp_socket_send
tcp_socket_recv
```

This API should resemble the glibc socket interface.


## TCP state machine

A `tcp_socket` struct will contain a `state` field, which represents the socket's state.
I will not list all the possible states (the reason being a lack of knowledge), but imagine something like this:

```
CLOSED
LISTEN
SYN_SENT
SYN_RECEIVED
ESTABLISHED
```

taken from this image: <https://en.wikipedia.org/wiki/File:Tcp_state_diagram.png>

This diagram isn't entirely clear to me, but that's ok for now. I want to prepare the infrastructure first, create the connection part, and only then the disconnection part.


## data flow

### receiving packets

I will have an IP raw socket that will receive all packets on a given interface. From now on this socket will be named "the reader", or "the global reader".
The IP socket, upon receive, will parse the TCP header and pass the packet to the relevant socket, if it exists. (TODO: what to do if it doesn't exist?)
Then, when `tcp_socket_recv` or `tcp_socket_accept` is called with the relevant socket, the packet is handled by the socket's state machine and/or user.

### transmitting packets

Transmitting packets can be done by calling `tcp_socket_send` or `tcp_socket_connect`. It uses the underlying raw socket contained in the TCP socket structure to read the packet.


## concurrency

The design written above, as well as the aspiration of handling multiple sockets at once, force me to implement it in a concurrent way.
I can use a thread pool: each socket will have a thread, and I will add a global read thread (to handle the reader) and a configuration thread.
The configuration thread will have a configuration socket: a UNIX socket that can configure the library: for example add new sockets, bind a socket to an address, or close a socket.
Alternatively, I could use `select(2)`: the user's sockets will be writeable, and I will have 2 readable sockets: the global reader socket and a configuration socket.
If I use `select(2)`, the `select` loop will have to run in a different thread.

No matter which concurrency model I choose, the system will roughly look like this:
```
+-------------------+                                    +----------------------+
|                   |                                    |                      |
|                   |   bind req.                        |                      |
|     sender 1      +-------------------+                |      reader          |
|                   |                   |                |                      |
|                   |                   |                |                      |
+-------------------+                   |                +----------------------+
                                        |                                        
+-------------------+                   |                                        
|                   |                   |                +----------------------+
|                   |                   |                |                      |
|     sender 2      |                   |                |                      |
|                   |  connect req.     +----------------+      config socket   |
|                   +------------------------------------+                      |
+-------------------+                                    |                      |
                                                         +----------------------+
           .                                                                     
           .                                                                     
           .                                                                     
+--------------------+                                                           
|                    |                                                           
|                    |                                                           
|     sender n       |                                                           
|                    |                                                           
|                    |                                                           
+--------------------+                                                           
```

After rethinking this problem I have realized that the user's `tcp_socket` don't need to be writeable (we can send without waiting for an event...).
So `select(2)` with only the reader and config socket is the simplest way to go in my opinion.
