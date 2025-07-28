# midircd – Modern IRC Server (based on ircu, forked from snircd)

**midircd** is a modern IRC server based on Undernet's ircu and directly forked from snircd. It extends snircd with additional IRCv3 features, improved channel/user modes, and many enhancements for advanced IRC networks.

> **Note:** midircd is a continuation and further development of the snircd project, inheriting all its features and improvements.

---

## Key Features

- **Configurable Features System:**  
  All server features can be enabled/disabled at runtime or via configuration using the feature system (see `include/ircd_features.h` and F-lines).

- **Advanced Channel Modes:**  
  - +c: Block mIRC/ANSI color codes  
  - +C: Block CTCPs  
  - +N: Block channel notices  
  - +M: Only authenticated users may speak  
  - +T: Block multi-target messages  
  - +D/+d: Delayed join (invisible until activity)  
  - +u: Hide quit/part messages  
  - +j: Anti-join-flood  
  - +L: Channel link  
  - +e: Ban exception  
  - +Z: TLS-only channels

- **Extended User and Oper Modes:**  
  - +X: Oper override for channel restrictions  
  - +k: Network service protection (no kicks/deops/kills)  
  - +n: Hide channels in /whois  
  - +I: Hide idle time  
  - +R: Only accept messages from authenticated users

- **IRCv3 Support:**  
  - Capabilities: SASL, account-notify, away-notify, chghost, echomessage, extjoin, invite-notify, message-tags, standard-replies, channel-rename  
  - Full SASL authentication with service forwarding (see `ircd/m_sasl.c`)

- **Sethost:**  
  Opers and authorized users can change their hostname (spoofing), with fine-grained control via S: and F: lines.

- **Automatic Channel Modes:**  
  New channels receive default modes (e.g., "ntCN"), configurable via `AUTOCHANMODES` and `AUTOCHANMODES_LIST`.

- **Enhanced /check and /stats Commands:**  
  Detailed information about channels, users, and servers.

- **MOTD Management:**  
  Multiple MOTDs supported, selected by hostmask, IP, or class. Caching and dynamic reload (`motd.c`).

- **Anti-Abuse Mechanisms:**  
  - Nick/host/IP abuse protection  
  - G-lines (global bans) also on nick basis  
  - Automatic +i (invisible) for new users  
  - Channel join limits per user/IP

- **HEAD_IN_SAND Features:**  
  Granular control over visibility and accessibility of server information and statistics.

---

## Channel Privileges and Hierarchy

midircd supports a fine-grained privilege system for channel users. The following channel user modes and their hierarchy are available:

| Mode | Prefix | Name         | Description                                                                 |
|------|--------|--------------|-----------------------------------------------------------------------------|
| +S   | !      | ChanService  | Channel service bot, highest privilege, can override all restrictions        |
| +q   | ~      | Owner        | Channel owner, can set all lower modes and manage the channel                |
| +a   | &      | Admin        | Channel admin, can set most lower modes and manage the channel               |
| +o   | @      | Operator     | Channel operator, can manage users and moderate the channel                  |
| +h   | %      | Half-Op      | Half-operator, limited moderation rights (cannot set higher modes)           |
| +v   | +      | Voice        | Voiced user, can speak in moderated (+m) channels                            |

**Hierarchy:**  
`ChanService` > `Owner` > `Admin` > `Operator` > `Half-Op` > `Voice` > normal user

- Only users with a higher privilege can set or remove modes for users with lower privileges.
- Half-ops cannot set or remove operator/admin/owner modes.
- Channel service bots (+S) can override all restrictions.
- The privilege system is enforced for all mode changes, kicks, and other moderation actions.

---

## Channel Types

midircd supports several channel types, each with specific characteristics and use-cases:

| Prefix | Example         | Description                                                                 |
|--------|----------------|-----------------------------------------------------------------------------|
| #      | #channel       | **Global Channel:** Standard, network-wide channels visible to all users.   |
| &      | &local         | **Local Channel:** Only exists on the local server, not visible network-wide. |
| !      | !IDchannel     | **Network Channel:** Channels with a unique network ID for collision prevention and advanced features (e.g., channel renaming, linking). |
| +      | +channel       | **Modeless Channel:** Channels without channel modes or ops, open for all.  |

- **Global Channels (`#`)**: The most common type, used for public or private group chats across the network.
- **Local Channels (`&`)**: Useful for server-specific discussions or admin channels.
- **Network Channels (`!`)**: Used for advanced features like channel renaming, linking, and preventing channel takeovers.
- **Modeless Channels (`+`)**: No channel operators or modes; everyone can join and speak freely.

For more details on channel types and their behavior, see the documentation and `include/channel.h`.

---

## Major Enhancements

- **IRCv3 Capabilities:**  
  SASL, account-notify, message-tags, channel-rename, standard-replies, and more.

- **SASL Authentication:**  
  SASL commands are forwarded to a configurable service (`FEAT_SASL_SERVICE_NAMES`), with fallback to iauthd if unavailable.

- **Extended Channel and User Modes:**  
  +M, +T, +u, +D/+d, +j, +L, +e, +Z, +R, +X, +I, +n, +k

- **/check Command:**  
  Shows status and details for channels, users, and servers.

- **Sethost Improvements:**  
  Automatic application, password protection, freeform hostnames for opers.

- **Automatic Channel Modes:**  
  New channels get default modes automatically.

- **Enhanced Statistics and Debugging:**  
  /stats commands show more details, including memory and feature reporting.

- **Anti-Abuse:**  
  Improved G-line and USERIP mechanisms, protection against channel and nick abuse.

- **MOTD Improvements:**  
  Multiple MOTDs, selection by hostmask/IP/class, caching.

---

## Configuration

Most options are set via F-lines in the configuration or at runtime using the feature system.  
See `doc/readme.features` and `doc/readme.snircd` (applies to midircd) for examples and details.

---

## Further Documentation

- `doc/readme.snircd` – Detailed feature descriptions and changelog (applies to midircd)
- `doc/readme.features` – Overview of all configurable features
- `doc/readme.iauth` – iauth protocol documentation

---

## Contributors

See the "contributors" section in `doc/readme.snircd`.

---

## License

GNU General Public License v2 or later.

---

**Note:**  
This README summarizes the most important features and enhancements. For details on individual features and configuration, please consult the referenced documentation.