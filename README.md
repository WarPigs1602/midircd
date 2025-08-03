# midircd – Modern IRC Server (based on ircu, fork of snircd)

**midircd** is a powerful, highly configurable IRC daemon offering modern features and extensive control for channels and users. Its goal is to provide a secure, flexible, and feature-rich IRC server.

---

## Main Features

- **Configurable Feature System:**  
  All server features can be enabled or disabled at runtime or via configuration (F-lines). See `include/ircd_features.h`.

- **Advanced Channel Modes:**  
  - `+c`: Block mIRC/ANSI color codes  
  - `+C`: Block CTCPs  
  - `+N`: Block channel notices  
  - `+M`: Only authenticated users may speak  
  - `+T`: Block multi-target messages  
  - `+D/+d`: Delayed join (invisible until activity)  
  - `+u`: Hide quit/part messages  
  - `+j`: Anti-join-flood  
  - `+L`: Channel linking  
  - `+e`: Ban exception  
  - `+Z`: TLS-only channel

- **Extended User and Oper Modes:**  
  - `+X`: Oper override for channel restrictions  
  - `+k`: Network service protection (no kicks/deops/kills)  
  - `+n`: Hide channels in /whois  
  - `+I`: Hide idle time  
  - `+R`: Only accept messages from authenticated users  
  - Many more!

- **IRCv3 Support:**  
  - Capabilities: SASL, account-notify, away-notify, chghost, echomessage, extjoin, invite-notify, message-tags, standard-replies, channel-rename

- **SASL Authentication:**  
  Full SASL support with forwarding to configurable services.

- **Sethost:**  
  Opers and authorized users can change their hostname (spoofing), with fine-grained control via S: and F: lines.

- **Automatic Channel Modes:**  
  New channels receive default modes (e.g., `ntCN`), configurable via `AUTOCHANMODES` and `AUTOCHANMODES_LIST`.

- **Enhanced /check and /stats Commands:**  
  Detailed information about channels, users, and servers.

- **MOTD Management:**  
  Flexible management for MOTD and remote-MOTD, banner support.

- **Anti-Abuse:**  
  Clone and flood limits, IP checks, TLS-only channels, improved host-hiding/cloaking.

---

## Channel Privileges & Hierarchy

midircd supports a fine-grained privilege system for channel users:

| Mode | Prefix | Name         | Description                                                             |
|------|--------|--------------|-------------------------------------------------------------------------|
| +S   | !      | ChanService  | Channel service bot, highest privilege, can override all restrictions   |
| +q   | ~      | Owner        | Channel owner, can set/remove all lower modes and manage the channel    |
| +a   | &      | Admin        | Channel admin, can set most lower modes and manage the channel          |
| +o   | @      | Operator     | Channel operator, can manage users and moderate the channel             |
| +h   | %      | Half-Op      | Limited moderation rights (cannot set higher modes)                     |
| +v   | +      | Voice        | Voiced user, can speak in moderated (+m) channels                      |

**Hierarchy:**  
ChanService > Owner > Admin > Operator > Half-Op > Voice > normal user

- Only users with higher privilege can set or remove modes for users with lower privileges.
- Half-ops cannot set or remove operator/admin/owner modes.
- Channel service bots (+S) can override all restrictions.
- The privilege system is enforced for all mode changes, kicks, and other moderation actions.

---

## Important IRC Commands

All commands use the `/COMMAND` syntax. Common commands (see `/HELP` in IRC for details):

- `/ADMIN [server]` — Show admin info
- `/AWAY [message]` — Set away status
- `/BYE`, `/EXIT`, `/QUIT` — End session
- `/CHANNEL [channel]` — Join a channel
- `/CLEAR` — Clear the screen
- `/HELP [command]` — Show help
- `/CHECK` — Show status of channels, users, servers
- `/STATS [type]` — Show stats (a/c/d/e/f/g/i/j/J etc.)
- `/MODE` — Set channel/user modes
- `/SETHOST` — Change hostname (Opers)
- `/OPER` — Become an operator
- `/WHOIS`, `/WHOWAS`, `/LIST`, `/NAMES`, `/TOPIC`, `/KICK`, `/BAN`, /RENAME, etc.

---

## Key Settings / Limits

- Maximum channels per user: 10
- Maximum nickname length: 15
- Maximum bans per channel: 45
- Maximum ban length: 40 characters
- Clone limit per IP: 4
- Channel length: 200 characters
- Default port: 4400
- TLS: TLSv1.1, TLSv1.2, TLSv1.3 supported

---

## Additional Features

- Host cloaking and hiding (configurable)
- WebIRC cloaking
- Advanced logging and debugging options
- Automatic operator level management
- Local channels, Zannels, topic burst
- Service and feature URLs preconfigured

---

## Documentation & Help

See these files for detailed information on features, configuration, and commands:  
- `doc/irc.1` — User commands  
- `doc/ircd.8` — Server options  
- `ircd/ircd_features.c` & `include/ircd_features.h` — Feature definitions

---

## License

See LICENSE file in the repository.

---

**Contact/Support:**  
Questions and contributions via GitHub issue or directly to the project maintainers.
