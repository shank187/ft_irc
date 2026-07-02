*This project has been created as part of the 42 curriculum by aelbour, abel-had.*

# ft_irc

## Description

`ft_irc` is a C++98 implementation of an IRC (Internet Relay Chat) server built from
scratch, without relying on any external IRC libraries. The goal of the project is to
understand and reproduce the core mechanics of a real IRC server: handling multiple
simultaneous client connections through non-blocking I/O and a single `poll()` (or
equivalent) call, parsing the IRC protocol, and implementing the standard commands
needed for authentication, messaging, and channel management.

The server accepts connections from standard IRC clients (such as `irssi`) as well as
raw TCP tools (such as `netcat`), and supports multiple clients and channels running
concurrently, each with configurable modes and operator privileges.

As a bonus, the project also includes **TimeBot**, a small IRC bot that connects to the
server like any other client and replies to a `!time` command with the current server
time.

## Instructions

### Compilation

```bash
make
```

This produces the `ircserv` executable (and the `bot` executable for the bonus part).

### Usage

Start the server by providing a port and a connection password:

```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6667 mypassword123
```

### Connecting as a Client

#### 1. Using irssi (recommended)
```bash
irssi -c 127.0.0.1 -p 6667 -w mypassword123 -n eval_nick
```

#### 2. Using netcat (raw commands)
```bash
nc 127.0.0.1 6667
```
Once connected, send the registration commands:
```text
PASS mypassword123
NICK eval_nick
USER eval_user 0 * :Eval User
```

### Implemented Features

- **Authentication:** `PASS`, `NICK`, `USER`
- **Channel Operations:** `JOIN`, `PART`, `KICK`, `INVITE`, `TOPIC`
- **Messaging:** `PRIVMSG` (users and channels)
- **Channel Modes:**
  - `i`: Invite-only channel
  - `t`: Topic restrictions
  - `k`: Channel key (password)
  - `o`: Channel operator privilege
  - `l`: User limit

### Bonus: TimeBot

```bash
./bot 127.0.0.1 6667 mypassword123
```

The bot connects under the nickname **TimeBot**. Send it a private message containing
`!time` to receive the current server time.

**Using irssi:**
```text
/msg TimeBot !time
```

**Using netcat:**
```text
PRIVMSG TimeBot :!time
```

### File Transfer (DCC)

File transfers in IRC are handled via the Direct Client-to-Client (DCC) protocol. The
server's only responsibility is to correctly parse and relay the initial CTCP `PRIVMSG`
handshake between the two clients; once that handshake is relayed, the clients open a
direct peer-to-peer connection to transfer the file themselves.

To test this during evaluation, open two separate `irssi` instances connected to the
server (e.g. `clientA` and `clientB`).

**1. From the sender (clientA):**
```text
/dcc send clientB /path/to/some/test_file.txt
```

**2. From the receiver (clientB):**
```text
/dcc get clientA
```
*(The downloaded file is typically saved to the default `irssi` download directory,
often `~/.irssi/` or the directory from which `irssi` was launched.)*

## Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- `man poll`, `man fcntl`, `man socket` — reference manuals for the system calls used
  in the non-blocking network engine.
- [irssi documentation](https://irssi.org/documentation/) — used as the reference
  client for manual testing.

### How AI was used

AI Usage: Artificial Intelligence (LLMs) was utilized during the development of this project for the following tasks:

- **Concept Explanation:** Breaking down complex network theories (e.g., TCP 3-way handshake, non-blocking sockets, and the difference between IP routing and local DCC transfers).
- **:** helping draft and structure this README.md file.
- **Code Review:** Assisting in debugging specific segmentation faults and explaining Valgrind outputs (e.g., "Conditional jump or move depends on uninitialised value(s)").


## Team

- **aelbour** — Server engine & networking
- **abel-had** — Protocol logic & commands