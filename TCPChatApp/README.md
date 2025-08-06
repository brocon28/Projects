# Simple TCP Client/Server Chat Application

A basic multi-client chat system implemented in C++ using TCP sockets and multithreading.

---

## Features

- Supports multiple clients connecting simultaneously to the server.
- Broadcasts messages from any client to all other connected clients.
- Clients send a username upon connection; messages are tagged with usernames.
- Real-time message receiving and sending with threaded client handling.
- User prompt displays `[You]:` in red to differentiate user input.
- Graceful handling of client disconnects and server shutdown.

---

## Technologies Used

- C++11
- POSIX sockets (`<sys/socket.h>`, `<arpa/inet.h>`, `<unistd.h>`)
- C++11 threads (`<thread>`)
- Mutexes and atomic flags for thread safety

---

## How to Build

1. Make sure you have `g++` installed (supporting C++11).
2. Navigate to the project directory.
3. Run:
   ```bash
   make
