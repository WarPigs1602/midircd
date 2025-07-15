# Extended IRC Channel User Modes

This implementation adds extended channel user modes to the IRC server, providing a comprehensive hierarchy of user privileges within channels.

## New Channel User Modes

| Mode | Symbol | Name | Description |
|------|--------|------|-------------|
| +S | ~ | Service | Highest level, typically for services/bots |
| +q | & | Owner | Channel owner, can set most modes |
| +a | % | Admin | Channel administrator |
| +o | @ | Operator | Channel operator (existing) |
| +h | % | Half-operator | Limited operator privileges |
| +v | + | Voice | Can speak in moderated channels (existing) |

## Hierarchy and Permissions

The hierarchy from highest to lowest privilege is:
1. Service (~) - Can set any mode
2. Owner (&) - Can set any mode except Service
3. Admin (%) - Can set Operator, Half-op, Voice modes
4. Operator (@) - Can set Half-op, Voice modes
5. Half-operator (%) - Can set Voice mode
6. Voice (+) - No mode-setting privileges
7. Regular user - No special privileges

## Usage Examples

### Setting Modes
```
MODE #channel +q nick     # Give owner status to nick
MODE #channel +a nick     # Give admin status to nick  
MODE #channel +h nick     # Give half-op status to nick
MODE #channel +S nick     # Give service status to nick (requires high privileges)
```

### Removing Modes
```
MODE #channel -q nick     # Remove owner status from nick
MODE #channel -a nick     # Remove admin status from nick
MODE #channel -h nick     # Remove half-op status from nick
```

## Display in IRC Commands

### NAMES Command
The NAMES command will show users with their highest privilege prefix:
```
= #testchan :~Service &Owner %Admin @Operator %HalfOp +Voice RegularUser
```

### WHO Command
The WHO command includes the prefix in the flags field based on the user's highest privilege.

### WHOIS Command  
The WHOIS command displays channels with the user's privilege prefix:
```
:server 319 nick :~#servicechan &@#ownerchan %#adminchan @#opchan +#voicechan #regular
```

## Channel Creation
When a user creates a new channel, they automatically receive Owner (+q) status along with the traditional Operator (+o) status, providing them with full control over the channel.

## Backwards Compatibility
This implementation maintains full backwards compatibility with existing IRC clients and servers. Clients that don't understand the new modes will simply see them as regular channel prefixes, and the traditional +o and +v modes continue to work as before.

## Protocol Support
The server advertises support for the new modes through the IRC 005 (RPL_ISUPPORT) numeric:
- PREFIX=(Sqahov)~&%@+
- STATUSMSG=~&%@+
- CHANMODES includes the new modes in the appropriate categories

## Implementation Notes
- Multiple modes can be stacked (e.g., a user can have both +q and +o)
- The highest privilege is displayed in most contexts
- Permission checks ensure users can only modify modes at or below their privilege level
- Services and server operators may have special override permissions