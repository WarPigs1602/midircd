# midircd - The midiandmore IRC Daemon

**midircd** is a modern, feature-rich IRC (Internet Relay Chat) daemon based on QuakeNet's snircd and inspired by Undernet's ircu. It provides a robust platform for running IRC networks, supporting classic and contemporary IRC protocols and features.

---

## Features

- **Comprehensive IRC Protocol Support**
  - Implements standard IRC client/server and server/server communication.
  - Supports Undernet P10 and QuakeNet protocol extensions.
  - Fully compatible with modern IRC clients.

- **Advanced User and Channel Management**
  - Registration, nickname handling, channel creation, and operator privileges.
  - Supports "string channels" (channels starting with '+') with ownership and advanced /mode command support.

- **Network Linking**
  - Link multiple servers to form resilient IRC networks.
  - Handles server registration, authentication, and synchronization with timestamp-based conflict resolution.
  - Automatic protocol version detection for seamless upgrades and backwards compatibility.

- **Extensible and Configurable**
  - Modular configuration via `config.h` and runtime options.
  - Feature toggling and custom behaviors through source-level and config changes.

- **Performance and Efficiency**
  - Optimized memory usage and speed.
  - Includes a custom database primitives library (`libs/dbprim`) for efficient in-memory management using linked lists, hash tables, and sparse matrices.

- **Protocol Extensions**
  - WHOX: Extended WHO information.
  - USERIP: IP-based banning and management.
  - RPL_ISUPPORT: Announces server features to clients for advanced script compatibility.

- **Test Suite**
  - Perl-based automated tests for protocol and feature verification.

- **Rich Documentation**
  - Protocol and server documentation in `doc/` (notably `doc/p10.html` and `doc/features.txt`).
  - Historical notes, changelogs, and feature overviews.

---

## Getting Started

### Building

1. **Edit Configuration:**
   - Adjust `include/config.h` to match your requirements.
   - Be cautious not to override protected lines unless you know what you are doing.

2. **Compile:**
   - Use the provided Makefiles (typically `make` from the project root).

3. **Install and Run:**
   - Start the server:  
     ```
     ./ircd/ircd -f <path-to-config>
     ```
   - Link to other IRC servers as needed.

### Testing

- Use the test framework under `tests/`:
  - Requires Perl modules: `POE`, `POE::Component::IRC` (installable from CPAN).
  - See `tests/readme.txt` for details.

---

## Architecture

- **Core Daemon (`ircd/`)**: Handles all protocol logic, client/server networking, and IRC state management.
- **Database Primitives (`libs/dbprim/`)**: Custom data structures for performance and scalability.
- **Documentation (`doc/`)**: Protocol specs, feature lists, manuals, and historical background.

---

## Licensing

- **Main Project**: GNU General Public License (GPL).
- **Library Components**: GNU Library General Public License (LGPL).
- See COPYING files for full terms.

---

## Contributors

See `doc/AUTHORS` for a full list of contributors and acknowledgements.

---

## Further Reading

- Protocol specification: `doc/p10.html`
- Features: `doc/features.txt`
- History and changelog: `doc/history/README-2.6`, `doc/history/overview.u2.9`

---

## Contact

Bug reports and suggestions are welcome! Please see the AUTHORS file or open an issue on GitHub.

---

**midircd** continues the development tradition of QuakeNet and Undernet, bringing modern enhancements and reliability to IRC networking.
