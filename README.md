*This project has been created as part of the 42 curriculum by Aelbour and Abel-had.*

# ft_irc

## Description

The **ft_irc** project aims to recreate a fully functional Internet Relay Chat (IRC) server from scratch in C++98. The goal of this project is to understand network programming, TCP/IP protocols, and client-server architecture. The server is capable of handling multiple concurrent clients without crashing or hanging, utilizing non-blocking I/O multiplexing with the `poll()` system call.

**Team Responsibilities:**
- **Aelbour:** Server Engine & Networking (`socket`, `bind`, `listen`, `poll`, memory management)
- **Abel-had:** Protocol Logic & IRC Commands parsing

**Implemented Features:**
- Authentication (`PASS`, `NICK`, `USER`)
- Channel Operations (`JOIN`, `PART`, `KICK`, `INVITE`, `TOPIC`)
- Messaging (`PRIVMSG` for users and channels)
- Channel Modes (`i`, `t`, `k`, `o`, `l`)
- File Transfer (DCC via CTCP)
- Bonus: IRC Bot (TimeBot)

---

## Instructions

### Compilation
To compile the server, run the following command at the root of the repository:
```bash
make
```

### Execution
Start the server by providing a port and a connection password:
```bash
./ircserv <port> <password>
```
**Example:**
```bash
./ircserv 6667 mypassword123
```

### Connecting as a Client
Evaluators can connect using standard IRC clients like `irssi` or raw TCP tools like `netcat` (`nc`).

**Using irssi (Recommended):**
```bash
irssi -c 127.0.0.1 -p 6667 -w mypassword123 -n eval_nick
```

### Testing the Bot
The project includes a bot that connects to the server and responds to commands. Run it in a separate terminal:
```bash
./bot 127.0.0.1 6667 mypassword123
```
*In `irssi`, send a private message to request the current server time:* `/msg TimeBot !time`

### Testing File Transfer (DCC)
To safely test localhost file transfers without overwriting files:
1. **User A (Terminal 1):** `echo "Secret file!" > secret.txt`, then start irssi and connect.
2. **User B (Terminal 2):** `mkdir receiver_dir && cd receiver_dir`, then start irssi and connect.
3. **User A (Sender):** Send the file using `/dcc send UserB secret.txt`
4. **User B (Receiver):** Accept the file using `/dcc get UserA`

---

## Resources

**Classic References:**
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/): Used heavily for understanding socket creation, `bind()`, `listen()`, `accept()`, and I/O multiplexing with `poll()`.
- [RFC 1459 (Internet Relay Chat Protocol)](https://datatracker.ietf.org/doc/html/rfc1459): Referenced for core protocol logic and message formatting.
- [RFC 2812 (Internet Relay Chat: Client Protocol)](https://datatracker.ietf.org/doc/html/rfc2812): Referenced for specific command behaviors and numeric replies.
- standard C++98 documentation for utilizing containers (`std::vector`, `std::map`).

**AI Usage:**
Artificial Intelligence (LLMs) was utilized during the development of this project for the following tasks:
- **Concept Explanation:** Breaking down complex network theories (e.g., TCP 3-way handshake, non-blocking sockets, and the difference between IP routing and local DCC transfers).
- **Code Review:** Assisting in debugging specific segmentation faults and explaining Valgrind outputs (e.g., "Conditional jump or move depends on uninitialised value(s)").
