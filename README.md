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

The project includes a simple IRC bot that connects to the server and responds to commands.[cite: 3]
```bash
./bot 127.0.0.1 6667 mypassword123
```
*Note: Send `!time` to the bot in a private message to get the current server time.*[cite: 3]

### Testing the Bot

The bot connects using the nickname **TimeBot**. You can request the current server time by sending it a private message containing the `!time` command.

**Using irssi:**
```text
/msg TimeBot !time
```

**Using netcat:**
```text
PRIVMSG TimeBot :!time
```

## File Transfer (DCC)

File transfers in IRC are handled via Direct Client-to-Client (DCC) protocol. The server's responsibility is to properly parse and relay the initial CTCP `PRIVMSG` handshake between the two clients. Once the handshake is relayed, the clients open a direct peer-to-peer connection to transfer the file.

### Testing File Transfer

To test this during evaluation, open two separate `irssi` instances connected to your server (e.g., `clientA` and `clientB`).

**1. From the sender (clientA):**
Initiate the file transfer using the `/dcc send` command, specifying the target nickname and the path to the file you want to send.
```text
/dcc send clientB /path/to/some/test_file.txt
```

**2. From the receiver (clientB):**
The receiving client will get a notification about the incoming file. To accept and download it, use the `/dcc get` command.
```text
/dcc get clientA
```
*(Note: The downloaded file will typically be saved in the default `irssi` download directory, often `~/.irssi/` or the directory from which `irssi` was launched).*