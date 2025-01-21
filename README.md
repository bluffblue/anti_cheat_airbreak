# Advanced Anti-Airbreak System

A highly accurate anti-airbreak system for SA-MP/open.mp servers that minimizes false positives while maintaining strong cheat detection.

## Features

- Smart Detection System
  - Accurate airbreak detection for both on-foot and in-vehicle
  - Intelligent fall detection to prevent false positives
  - Advanced velocity tracking
  - Interior and virtual world awareness

- Optimized Performance
  - Minimal server resource usage
  - Efficient position and velocity checks
  - Smart update intervals

- Robust Player Handling
  - Automatic monitoring initialization
  - Clean player disconnection handling
  - Safe player state management

## Installation

1. Ensure you have the required dependencies:
```pawn
#include <a_samp>
#include <PawnRakNet>
```

2. Copy the files to your server:
   - `src/anti-airbreak.inc` to your includes directory
   - `test.pwn` (optional, for testing)

3. Include the system in your gamemode:
```pawn
#include "src/anti-airbreak"
```

## Usage

### Basic Implementation
```pawn
public OnPlayerConnect(playerid)
{
    AirBreak_Init(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    AirBreak_Stop(playerid);
    return 1;
}

public OnPlayerUpdate(playerid)
{
    new detection = AirBreak_Check(playerid);
    if(detection > 0)
    {
        // Handle airbreak detection
        // detection == 1: On foot airbreak
        // detection == 2: Vehicle airbreak
    }
    return 1;
}
```

### Functions

- `AirBreak_Init(playerid)`: Start monitoring a player
- `AirBreak_Stop(playerid)`: Stop monitoring a player
- `AirBreak_Check(playerid)`: Check for airbreak (returns 0: no detection, 1: on foot, 2: in vehicle)

## Configuration

You can modify these defines in the include file:
```pawn
#define MAX_AIRBREAK_DISTANCE 15.0    // Maximum allowed teleport distance
#define MAX_AIRBREAK_HEIGHT 5.0       // Maximum allowed height change
#define AIRBREAK_CHECK_INTERVAL 500   // Check interval in milliseconds
#define MAX_SPEED_MULTIPLIER 1.5      // Speed multiplier for vehicles
#define FALL_SPEED_THRESHOLD -0.1     // Vertical speed threshold for fall detection
#define MAX_FALL_DISTANCE 100.0       // Maximum allowed fall distance
```

## Dependencies

- SA-MP/open.mp Server
- [PawnRakNet](https://github.com/urShadow/Pawn.RakNet)

## Building

Using sampctl:
```bash
sampctl package build
```

## Testing

1. Compile and run test.pwn
2. Connect to your server
3. The system will automatically monitor all players
4. Detected cheaters will be automatically kicked

## note
There are several shortcomings in this include, one of which is that if an admin performs a teleport, they will be detected as using airbreak. You may further develop it on your own if you wish

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Credits

Created by bluffblue
