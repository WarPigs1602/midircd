# Channel User Modes Implementation

This document describes the implementation of extended channel user modes in midircd.

## New Modes

The following channel user modes have been added with hierarchy support:

- `+S` (Service) - Channel service bot (highest level)
- `+q` (Owner) - Channel owner  
- `+a` (Admin) - Channel administrator
- `+o` (Chanop) - Channel operator (existing)
- `+h` (Halfop) - Half operator
- `+v` (Voice) - Voice (existing, lowest level)

## Hierarchy

The mode hierarchy is: **S > q > a > o > h > v**

Users with higher modes can set/unset lower modes, but cannot affect users at or above their own level.

## Special Rules

1. **Service Auto-Assignment**: Users identified as channel services (via the FLAG_SERVICE client flag or IsChannelService) automatically receive the +S mode when joining channels.

2. **Self-Devoicing**: Users can always remove modes from themselves that are at or below their current level.

3. **Server Override**: Servers and authenticated service bots can set any mode on any user.

## Implementation Details

### Data Structures

- Added `CHFL_SERVICE` (0x0200), `CHFL_OWNER` (0x0080), `CHFL_ADMIN` (0x0040), `CHFL_HALFOP` (0x0020) flags
- Updated `CHFL_VOICED_OR_OPPED` and `CHFL_OVERLAP` macros to include new modes
- Added corresponding `MODE_*` constants for parsing

### Functions Added

- `get_mode_level()` - Returns hierarchy level (0-6) for a status bitmask
- `check_mode_hierarchy()` - Validates if a mode change is allowed by hierarchy rules

### Protocol Support

- Updated BURST protocol to handle new mode characters (S, q, a, h)
- Enhanced mode parsing in `mode_parse()` to recognize new modes
- Modified `send_channel_modes()` to transmit new modes correctly

### Files Modified

- `include/channel.h` - Mode definitions and macros
- `ircd/channel.c` - Mode parsing and hierarchy logic
- `ircd/m_burst.c` - BURST protocol support

## Testing

The implementation includes:
- Hierarchy level validation
- Mode bit value verification (no overlaps)
- Integration with existing mode system
- Backward compatibility with old protocol

## Usage Examples

```
/mode #channel +q nick      # Give owner to nick
/mode #channel +a nick      # Give admin to nick  
/mode #channel +h nick      # Give halfop to nick
/mode #channel -o nick      # Remove op from nick (if you have higher level)
```

Service bots joining channels automatically receive +S mode.