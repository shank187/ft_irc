# ft_irc

A C++98 IRC server built for the 42 curriculum. This project implements a fully functional IRC server using non-blocking I/O and `poll()`, capable of handling multiple concurrent clients and standard IRC channels.

## Team
- **[Aelbour]** (Server Engine & Networking)
- **[Abel-had]** (Protocol Logic & Commands)

## Compilation

To compile the server, simply run:
```bash
make
```

## Usage

Start the server by providing a port and a connection password:
```bash
./ircserv <port> <password>
```
**Example:**
```bash
./ircserv 6667 mypassword123
```

## Connecting as a Client

Evaluators can connect using standard IRC clients like `irssi` or raw TCP tools like `netcat`.

### 1. Using irssi (Recommended)
```bash
irssi -c 127.0.0.1 -p 6667 -w mypassword123 -n eval_nick
```

### 2. Using netcat (Raw Commands)
```bash
nc -c 127.0.0.1 6667
```
Once connected, send the registration commands:
```text
PASS mypassword123
NICK eval_nick
USER eval_user 0 * :Eval User
```

## Implemented Features

- **Authentication:** `PASS`, `NICK`, `USER`
- **Channel Operations:** `JOIN`, `PART`, `KICK`, `INVITE`, `TOPIC`
- **Messaging:** `PRIVMSG` (Users and Channels)
- **Channel Modes:**
  - `i`: Invite-only channel
  - `t`: Topic restrictions
  - `k`: Channel key (password)
  - `o`: Channel operator privilege
  - `l`: User limit

## Bonus (TimeBot)

The project includes a simple IRC bot that connects to the server and responds to commands.
```bash
./bot 127.0.0.1 6667 mypassword123
```
*Note: Send `!time` to the bot in a private message to get the current server time.*