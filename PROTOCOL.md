# Levoit Vital 200S UART Protocol Documentation

## Overview

This document describes the UART communication protocol used between an ESP32 and the SC95F8617 chip in the Levoit Vital 200S air purifier. The protocol enables control of various functions including fan levels, modes, display settings, and provides status updates.

**Communication Parameters:**
- Baud Rate: 115200
- Data Bits: 8
- Parity: None
- Stop Bits: 1

## Message Structure

### Command Messages (ESP32 → SC95F8617)

Command messages are sent from the ESP32 to the SC95F8617 chip to control the air purifier.

**Standard Command Format (7-15 bytes):**

| Byte | Field | Description |
|------|-------|-------------|
| 0 | Header | Always `0xA5` |
| 1 | Type | Always `0x22` for commands |
| 2 | Message Counter | Increments with each message sent |
| 3 | Length | Payload length (e.g., `0x07`, `0x0B`, `0x12`, `0x2B`) |
| 4 | Fixed | Always `0x00` |
| 5 | Checksum | Calculated checksum value |
| 6 | Fixed | Always `0x02` |
| 7 | Command Type | Identifies the command type (see table below) |
| 8 | Fixed 2 | Either `0x50` or `0x55` |
| 9 | Fixed | Always `0x00` |
| 10 | Cmd Category | 1=general, 2=automode, 3=filter, 4=sleep modes |
| 11+ | Parameters | Command-specific value(s) |

### Response Messages (SC95F8617 → ESP32)

#### Acknowledgment Messages (12 bytes)

Simple ACK messages confirming command receipt:

| Byte | Field | Description |
|------|-------|-------------|
| 0 | Header | Always `0xA5` |
| 1 | Type | Always `0x12` for ACK |
| 2 | Counter | Counts up with each response |
| 3 | Length | Always `0x04` |
| 4-5 | Unknown | Fixed values |
| 6 | Checksum | Calculated checksum |
| 7 | Fixed | Always `0x02` |
| 8 | Type Echo | Echoes command type from request |
| 9 | Fixed | `0x50` or `0x55` |
| 10-11 | Fixed | Always `0x00` |

#### Status Update Messages (Automatic, every ~4 seconds)

Status messages are sent automatically about 350ms after a command and then every ~4 seconds:

**Status Message Format (114 bytes total, 108 payload):**

| Byte | Field | Description |
|------|-------|-------------|
| 0 | Header | Always `0xA5` |
| 1 | Type | Always `0x22` |
| 2 | Counter | Increments with each status message |
| 3 | Length | `0x6C` (108 bytes payload) |
| 4 | Fixed | `0x00` |
| 5 | Checksum | Calculated checksum |
| 6 | Fixed | `0x02` |
| 7 | Fixed | `0x00` |
| 8 | Fixed | `0x55` |
| 9 | Fixed | `0x00` |
| 10-113 | Status Data | TLV-encoded status fields (see below) |

**TLV Encoding Structure:**

The status data (bytes 10-113) uses Tag-Length-Value encoding. Bytes 0-9 are the message header.

```
Format: [Tag] [Length] [Value bytes...]

TLV payload starts at byte 10:

Byte 10-12:  00 01 02          Tag 0x00, Length=1, Value=[0x02]
Byte 13-17:  01 03 02 00 02    Tag 0x01, Length=3, Value=[0x02, 0x00, 0x02]
Byte 18-20:  02 01 01          Tag 0x02, Length=1, Value=[0x01]
Byte 21-23:  03 01 02          Tag 0x03 (Fan Mode), Length=1, Value=[0x02]
Byte 24-26:  04 01 00          Tag 0x04, Length=1, Value=[0x00]
Byte 27-29:  05 01 01          Tag 0x05, Length=1, Value=[0x01]
Byte 30-32:  06 01 01          Tag 0x06 (Display LED), Length=1, Value=[0x01]
Byte 33-35:  07 01 00          Tag 0x07 (Display Setting), Length=1, Value=[0x00]
Byte 36-38:  08 01 00          Tag 0x08 (Air Filter), Length=1, Value=[0x00]
Byte 39-41:  09 01 01          Tag 0x09 (Air Quality), Length=1, Value=[0x01]
Byte 42-44:  0A 01 64          Tag 0x0A (AQ Score), Length=1, Value=[0x64]
Byte 45-48:  0B 02 01 00       Tag 0x0B (PM2.5), Length=2, Value=[0x01, 0x00]
Byte 49-51:  0E 01 00          Tag 0x0E (Display Lock), Length=1, Value=[0x00]
Byte 52-54:  0F 01 02          Tag 0x0F (Auto Mode), Length=1, Value=[0x02]
Byte 55-58:  10 02 8A 00       Tag 0x10, Length=2, Value=[0x8A, 0x00]
Byte 59-62:  11 02 00 00       Tag 0x11 (Efficient), Length=2, Value=[0x00, 0x00]
Byte 63-65:  12 01 01          Tag 0x12 (Power), Length=1, Value=[0x01]
Byte 66-68:  13 01 01          Tag 0x13 (Light Detect), Length=1, Value=[0x01]
Byte 69-71:  16 01 00          Tag 0x16, Length=1, Value=[0x00]
Byte 72-74:  17 01 01          Tag 0x17, Length=1, Value=[0x01]
Byte 75-77:  18 01 00          Tag 0x18 (Sleep Mode), Length=1, Value=[0x00]
Byte 78-80:  19 01 01          Tag 0x19, Length=1, Value=[0x01]
Byte 81-84:  1A 02 05 00       Tag 0x1A (Fan Level), Length=2, Value=[0x05, 0x00]
Byte 85-87:  1B 01 03          Tag 0x1B, Length=1, Value=[0x03]
Byte 88-90:  1C 01 01          Tag 0x1C, Length=1, Value=[0x01]
Byte 91-94:  1D 02 2D 00       Tag 0x1D, Length=2, Value=[0x2D, 0x00]
Byte 95-97:  1E 01 01          Tag 0x1E, Length=1, Value=[0x01]
Byte 98-101: 20 02 E0 01       Tag 0x20, Length=2, Value=[0xE0, 0x01]
Byte 102-104: 1F 01 05         Tag 0x1F, Length=1, Value=[0x05]
Byte 105-107: 21 01 01         Tag 0x21, Length=1, Value=[0x01]
Byte 108-110: 22 01 02         Tag 0x22, Length=1, Value=[0x02]
Byte 111-113: 23 01 01         Tag 0x23, Length=1, Value=[0x01]
```

Tags range from 0x00 to 0x23, with each tag having a length byte followed by 0-N value bytes.

**Complete Status Data Field Map:**

The TLV payload starts at byte 10. Each entry consists of [Tag][Length][Value bytes...]. Table is sorted by tag number.

| TLV Tag | Tag Pos | Len | Value Pos | Field Name | Values |
|---------|---------|-----|-----------|------------|--------|
| 0x00 | 10 | 1 | 12 | Unknown 00 | Unknown purpose |
| 0x01 | 13 | 3 | 15-17 | Unknown 01 | Unknown purpose |
| 0x02 | 18 | 1 | 20 | **Power State** | 0=OFF, 1=ON |
| 0x03 | 21 | 1 | 23 | **Fan Mode** | 0=Manual, 1=Sleep, 2=Auto, 3=Pet |
| 0x04 | 24 | 1 | 26 | **Fan Speed** | 0x01=Very Low (Sleep), 0x02=Low, 0x03=Medium, 0x04=High, 0xFF=OFF |
| 0x05 | 27 | 1 | 29 | **Last Saved Fan Speed** | Fan speed restored to 0x04 at power-on |
| 0x06 | 30 | 1 | 32 | **Display State (LED)** | 0=OFF, 1=ON (actual LED state) |
| 0x07 | 33 | 1 | 35 | **Display Setting** | 0=OFF, 1=ON (user setting) |
| 0x08 | 36 | 1 | 38 | **Air Filter State** | 0=OK, 1=Replace |
| 0x09 | 39 | 1 | 41 | **Air Quality Level** | 1=Very Good, 2=Good, 3=Moderate, 4=Bad |
| 0x0A | 42 | 1 | 44 | **Air Quality Score** | 0-255 (proprietary score) |
| 0x0B | 45 | 2 | 47-48 | **Particle Density (PM2.5)** | µg/m³ value (16-bit LE) |
| 0x0E | 49 | 1 | 51 | **Display Lock** | 0=Unlocked, 1=Locked |
| 0x0F | 52 | 1 | 54 | **Auto Mode Setting** | 0=Default, 1=Quiet, 2=Efficient |
| 0x10 | 55 | 2 | 57-58 | Unknown 10 | Unknown purpose (16-bit LE) |
| 0x11 | 59 | 2 | 61-62 | **Efficient Value** | 100-1800 (16-bit LE, scaled by 1.3) |
| 0x12 | 63 | 1 | 65 | **Device Power** | 0=OFF, 1=ON |
| 0x13 | 66 | 1 | 68 | **Light Detection** | 0=OFF, 1=ON |
| 0x16 | 69 | 1 | 71 | Unknown 16 | Unknown purpose |
| 0x17 | 72 | 1 | 74 | **Light Detected** | 0=Light detected, 1=No light |
| 0x18 | 75 | 1 | 77 | **Sleep Mode** | 0=OFF, 1=ON |
| 0x19 | 78 | 1 | 80 | Unknown 19 | Unknown purpose |
| 0x1A | 81 | 2 | 83-84 | **Fan Level** | 0=min, 1-4=levels, 5=auto (16-bit LE) |
| 0x1B | 85 | 1 | 87 | **Quick Clean Fan Level** | 1-4 |
| 0x1C | 88 | 1 | 90 | **White Noise Fan Level** | 1-4 |
| 0x1D | 91 | 2 | 93-94 | **White Noise Minutes** | Time in minutes (16-bit LE) |
| 0x1E | 95 | 1 | 97 | Unknown 1E | Unknown purpose |
| 0x1F | 102 | 1 | 104 | **Sleep Mode Fan Level** | 1-4 |
| 0x20 | 98 | 2 | 100-101 | **Sleep Mode Minutes** | Time in minutes (16-bit LE) |
| 0x21 | 105 | 1 | 107 | **Day Time Auto Off** | 0=OFF, 1=ON |
| 0x22 | 108 | 1 | 110 | **Day Time Fan Mode** | Fan mode during day |
| 0x23 | 111 | 1 | 113 | **Day Time Fan Level** | Fan level during day |

*Note: Byte positions are fixed for each tag in the TLV structure. "LE" indicates little-endian byte order for multi-byte values.*

**Example Status Message (with key fields highlighted):**

```
Full message (114 bytes):
A5 22 19 6C 00 DE 02 00 55 00 00 01 02 01 03 02 00 02 02 01 01 03 01 02 04 01 00 05 01 01 06 01 01 07 01 00 08 01 00 09 01 01 0A 01 64 0B 02 01 00 0E 01 00 0F 01 02 10 02 8A 00 11 02 00 00 12 01 01 13 01 01 16 01 00 17 01 01 18 01 00 19 01 01 1A 02 05 00 1B 01 03 1C 01 01 1D 02 2D 00 1E 01 01 20 02 E0 01 1F 01 05 21 01 01 22 01 02 23 01 01

Bytes 0-9: Header (A5 22 19 6C 00 DE 02 00 55 00)
Bytes 10+: TLV payload

Decoded values from this message:
- Device Power (tag 0x12, byte 65): 0x01 = ON
- Fan Mode (tag 0x03, byte 23): 0x02 = Auto
- Fan Level (tag 0x1A, bytes 83-84): 0x05 0x00 = Auto Mode (5)
- PM2.5 (tag 0x0B, bytes 47-48): 0x01 0x00 = 1 µg/m³
- Display LED (tag 0x06, byte 32): 0x01 = ON
- Display Setting (tag 0x07, byte 35): 0x00 = OFF
- Air Filter (tag 0x08, byte 38): 0x00 = OK
- Air Quality Level (tag 0x09, byte 41): 0x01 = Very Good
- Air Quality Score (tag 0x0A, byte 44): 0x64 = 100
- Display Lock (tag 0x0E, byte 51): 0x00 = Unlocked
- Auto Mode (tag 0x0F, byte 54): 0x02 = Efficient
- Light Detection (tag 0x13, byte 68): 0x01 = ON
```

## Command Reference

### Device Power Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Device ON | Turn air purifier on | `A5 22 xx 07 00 CC 02 00 50 00 01 01 01` |
| Device OFF | Turn air purifier off | `A5 22 xx 07 00 CC 02 00 50 00 01 01 00` |

- **Command Type:** `0x00`
- **Fixed2:** `0x50`
- **Category:** `0x01` (general)
- **Value:** `0x01` = ON, `0x00` = OFF

### Fan Level Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Fan Level 1 | Set fan to level 1 | `A5 22 xx 07 00 BA 02 03 55 00 01 01 01` |
| Fan Level 2 | Set fan to level 2 | `A5 22 xx 07 00 BC 02 03 55 00 01 01 02` |
| Fan Level 3 | Set fan to level 3 | `A5 22 xx 07 00 BA 02 03 55 00 01 01 03` |
| Fan Level 4 | Set fan to level 4 | `A5 22 xx 07 00 B8 02 03 55 00 01 01 04` |

- **Command Type:** `0x03`
- **Fixed2:** `0x55`
- **Category:** `0x01` (general)
- **Value:** `0x01`-`0x04` for fan levels 1-4

### Fan Mode Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Auto Mode | Set fan mode to automatic | `A5 22 xx 07 00 98 02 02 55 00 01 01 02` |
| Sleep Mode | Set fan mode to sleep | `A5 22 xx 07 00 98 02 02 55 00 01 01 01` |
| Pet Mode | Set fan mode to pet | `A5 22 xx 07 00 93 02 02 55 00 01 01 05` |

- **Command Type:** `0x02`
- **Fixed2:** `0x55`
- **Category:** `0x01` (general)
- **Value:** `0x02` = Auto, `0x01` = Sleep, `0x05` = Pet

### Auto Mode Configuration

| Command | Description | Bytes |
|---------|-------------|-------|
| Quiet Mode | Set auto mode to quiet | `A5 22 xx 0B 00 A2 02 02 55 00 02 01 01 03 02 00 00` |
| Default Mode | Set auto mode to default | `A5 22 xx 0B 00 A2 02 02 55 00 02 01 00 03 02 00 00` |
| Efficient Mode (Max) | Set efficiency to 1800 | `A5 22 xx 0B 00 8C 02 02 55 00 02 01 02 03 02 24 09` |
| Efficient Mode (Min) | Set efficiency to 100 | `A5 22 xx 0B 00 36 02 02 55 00 02 01 02 03 02 82 00` |

- **Command Type:** `0x02`
- **Fixed2:** `0x55`
- **Category:** `0x02` (automode)
- **Parameters:** Multi-byte values for mode selection

### Display Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Display ON | Turn display fully on | `A5 22 xx 07 00 AF 02 04 55 00 01 01 64` |
| Display OFF | Turn display off | `A5 22 xx 07 00 4A 02 04 55 00 01 01 00` |
| Display Lock ON | Enable display lock | `A5 22 xx 07 00 7B 02 40 51 00 01 01 01` |
| Display Lock OFF | Disable display lock | `A5 22 xx 07 00 7B 02 40 51 00 01 01 00` |

- **Display brightness:**
  - Command Type: `0x04`, Fixed2: `0x55`, Category: `0x01`
  - Value: `0x64` (100) = full brightness, `0x00` = off
- **Display lock:**
  - Command Type: `0x40`, Fixed2: `0x51`, Category: `0x01`
  - Value: `0x01` = locked, `0x00` = unlocked

### Light Detection Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Light Detect ON | Enable auto brightness | `A5 22 xx 07 00 AA 02 11 55 00 01 01 01` |
| Light Detect OFF | Disable auto brightness | `A5 22 xx 07 00 AA 02 11 55 00 01 01 00` |

- **Command Type:** `0x11`
- **Fixed2:** `0x55`
- **Category:** `0x01` (general)
- **Value:** `0x01` = ON, `0x00` = OFF

### Filter Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| Reset Filter | Reset filter timer | `A5 22 xx 06 00 92 02 05 55 00 03 00` |

- **Command Type:** `0x05`
- **Fixed2:** `0x55`
- **Category:** `0x03` (filter)

### WiFi Commands

| Command | Description | Bytes |
|---------|-------------|-------|
| WiFi ON | Enable WiFi | `A5 22 xx 12 00 C1 02 18 50 00 01 01 00 02 02 F4 01 03` |
| WiFi OFF | Disable WiFi | `A5 22 xx 12 00 AC 02 18 50 00 01 01 01 02 02 7D 00 03` |

- **Command Type:** `0x18`
- **Fixed2:** `0x50`
- **Category:** `0x01` (general)
- **Parameters:** Multi-byte WiFi configuration

## Status Field Mapping

### Device Power (Tag 0x12, Byte 65)
| Value | State |
|-------|-------|
| 0x00 | OFF |
| 0x01 | ON |

**Example:** `... 12 01 ►01◄ 13 01 ...` = Device is ON (Tag 0x12, Length 1, Value 0x01)

### Fan Levels (Tag 0x1A, Bytes 83-84)

16-bit value in little-endian format.

| Value | Meaning |
|-------|---------|  
| 0x00 | Minimum fan speed / OFF |
| 0x01 | Fan level 1 |
| 0x02 | Fan level 2 |
| 0x03 | Fan level 3 |
| 0x04 | Fan level 4 (circulation) |
| 0x05 | Auto mode (fan adjusts automatically) |
| 0xFF | Null/Unknown |

**Example:** `... 1A 02 ►05 00◄ 1B ...` = Auto mode (Tag 0x1A, Length 2, Value 0x0005)

### Air Quality Levels (Tag 0x09, Byte 41)

| Value | Quality | LED Color | Description |
|-------|---------|-----------|-------------|
| 1 | Very Good | Blue | Air quality excellent |
| 2 | Good | Green | Air quality good |
| 3 | Moderate | Orange | Air quality moderate |
| 4 | Bad | Red | Air quality poor |

**Example:** `... 09 01 ►01◄ 0A ...` = Very Good (Tag 0x09, Length 1, Value 0x01)

### Display States

**Display LED State (Tag 0x06, Byte 32):**
| Value | State |
|-------|-------|
| 0x00 | LED is OFF / Dimmed |
| 0x01 | LED is ON / Illuminated |

**Display Setting (Tag 0x07, Byte 35):**
| Value | Setting |
|-------|---------|
| 0x00 | User set to OFF |
| 0x01 | User set to ON |

*Note: When light detection is enabled and room is dark, Display LED (byte 32) will be 0x00 even though Display Setting (byte 35) is 0x01*

**Example:** `... 06 01 ►00◄ 07 01 ►01◄ ...` means user wants display ON but light detection dimmed it

### Auto Mode Settings (Tag 0x0F, Byte 54)

| Value | Setting |
|-------|---------|
| 0 | Default |
| 1 | Quiet |
| 2 | Efficient |

**Example:** `... 0F 01 ►02◄ 10 ...` = Efficient mode (Tag 0x0F, Length 1, Value 0x02)

### Particle Density / PM2.5 (Tag 0x0B, Bytes 47-48)

16-bit value in little-endian format representing PM2.5 concentration in µg/m³

**Example:** `... 0B 02 ►01 00◄ 0E ...` = 0x0001 = 1 µg/m³ (Tag 0x0B, Length 2, Value 0x01 0x00)

### Display Lock (Tag 0x0E, Byte 51)
| Value | State |
|-------|-------|
| 0x00 | Unlocked |
| 0x01 | Locked |

### Light Detection (Tag 0x13, Byte 68)
| Value | State |
|-------|-------|
| 0x00 | OFF (manual brightness) |
| 0x01 | ON (auto brightness) |
| 0x02 | ON (alternate encoding) |

### Air Filter State (Tag 0x08, Byte 38)
| Value | State |
|-------|-------|
| 0x00 | Filter OK |
| 0x01 | Replace Filter |

### Light Detected (Tag 0x17, Byte 74)
| Value | State |
|-------|-------|
| 0x00 | Light detected (room is bright) |
| 0x01 | No light detected (room is dark) |

**Example:** `... 17 01 ►00◄ 18 ...` = Light detected (Tag 0x17, Length 1, Value 0x00)

## Checksum Calculation

The checksum byte (position 5 in all messages) is calculated using an additive checksum with modulo 256:

**Algorithm:**
```
1. Sum all bytes in the message EXCEPT byte 5 (the checksum position)
2. Calculate: checksum = (0x1FF - sum) & 0xFF
   Which is equivalent to: (511 - sum) modulo 256
```

**Example Implementation (C++):**
```cpp
uint8_t calculateChecksum(uint8_t *data, uint8_t dataLen)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < dataLen; i++)
    {
        if (i != 5)
            sum += data[i]; // skip the checksum byte position
    }
    return (0x1FF - sum) & 0xFF; // 511 - sum, modulo 256
}
```

**Example:**
For the command `A5 22 10 07 00 CC 02 00 50 00 01 01 01`:
- Sum bytes (skip position 5): 0xA5 + 0x22 + 0x10 + 0x07 + 0x00 + 0x02 + 0x00 + 0x50 + 0x00 + 0x01 + 0x01 + 0x01 = 0x233
- Checksum: (0x1FF - 0x233) & 0xFF = 0xCC ✓

## Implementation Notes

### Timing Requirements

1. **Command Response:** The SC95F8617 chip responds with an ACK within a few milliseconds of receiving a command.
2. **Status Updates:** First status update arrives approximately 350ms after a command is executed.
3. **Periodic Updates:** Subsequent status updates are sent automatically every ~4 seconds.

### Message Counter

- The message counter in byte 2 should increment with each transmitted message
- Counter appears to wrap around after reaching 0xFF
- Status responses use their own independent counter

### Command Categories

Commands are organized into categories (byte 10):
- **Category 1:** General commands (power, fan, display, etc.)
- **Category 2:** Auto mode configurations
- **Category 3:** Filter-related commands
- **Category 4:** Sleep mode configurations (not fully implemented)

### Not Implemented Features

The following commands exist in the protocol but are not yet implemented in the ESPHome component:

- Sleep mode configurations (various timing and fan settings)
- Quick clean modes with custom duration
- White noise mode
- Daytime scheduling (auto ON/OFF at specific times)
- Custom sleep mode settings with 11h59m max duration

These features can be implemented by following the same pattern as existing commands, using the byte sequences documented in the command table.

## Example Message Flows

### Turning Device ON

**Command (ESP32 → SC95F8617):**
```
A5 22 10 07 00 CC 02 00 50 00 01 01 01
│  │  │  │  │  │  │  │  │  │  │  │  └─ Value: 0x01 = ON
│  │  │  │  │  │  │  │  │  │  │  └──── Parameter count
│  │  │  │  │  │  │  │  │  │  └─────── Category: 0x01 = general
│  │  │  │  │  │  │  │  │  └────────── Fixed: 0x00
│  │  │  │  │  │  │  │  └───────────── Fixed2: 0x50
│  │  │  │  │  │  │  └──────────────── Command Type: 0x00 = power
│  │  │  │  │  │  └─────────────────── Fixed: 0x02
│  │  │  │  │  └────────────────────── Checksum
│  │  │  │  └───────────────────────── Fixed: 0x00
│  │  │  └──────────────────────────── Length: 0x07 (7 bytes payload)
│  │  └─────────────────────────────── Counter
│  └────────────────────────────────── Type: 0x22 = command
└───────────────────────────────────── Header: 0xA5
```

**ACK Response (SC95F8617 → ESP32):**
```
A5 12 14 04 00 DE 02 00 50 00
│  │  │  │  │  │  │  │  │  └─ Fixed
│  │  │  │  │  │  │  │  └──── Fixed2: echoes 0x50
│  │  │  │  │  │  │  └─────── Command Type echo: 0x00
│  │  │  │  │  │  └────────── Fixed: 0x02
│  │  │  │  │  └───────────── Checksum
│  │  │  │  └──────────────── Fixed
│  │  │  └─────────────────── Length: 0x04
│  │  └────────────────────── Counter
│  └───────────────────────── Type: 0x12 = ACK
└──────────────────────────── Header: 0xA5
```

**Status Update (~350ms later):**
```
Status message (114 bytes):
A5 22 1E 6C 00 DB 02 00 55 00 ... [byte 20: 01] ... [byte 23: 02] ... [byte 26: 05] ...
                                      └─ Device Power: 0x01 = ON
                                                 └─ Fan Mode: 0x02 = Auto
                                                          └─ Fan Level: 0x05 = Auto

Complete relevant excerpt showing TLV structure:
Bytes 10-30: 00 01 02 01 03 02 00 02 02 01 01 03 01 02 04 01 00 05 01 01 06
Byte 20 = 0x01 ────────────────────────────────────────┘
  This is the ON/OFF state: 0x01 = Device is ON
```

### Setting Fan to Level 3

**Command:**
```
A5 22 18 07 00 BA 02 03 55 00 01 01 03
│  │  │  │  │  │  │  │  │  │  │  │  └─ Fan level: 0x03
│  │  │  │  │  │  │  │  │  │  │  └──── Parameter count
│  │  │  │  │  │  │  │  │  │  └─────── Category: 0x01 = general
│  │  │  │  │  │  │  │  │  └────────── Fixed: 0x00
│  │  │  │  │  │  │  │  └───────────── Fixed2: 0x55
│  │  │  │  │  │  │  └──────────────── Command Type: 0x03 = fan level
│  │  │  │  │  │  └─────────────────── Fixed: 0x02
│  │  │  │  │  └────────────────────── Checksum
│  │  │  │  └───────────────────────── Fixed: 0x00
│  │  │  └──────────────────────────── Length: 0x07
│  │  └─────────────────────────────── Counter: 0x18
│  └────────────────────────────────── Type: 0x22
└───────────────────────────────────── Header: 0xA5
```

**ACK Response:**
```
A5 12 18 04 00 D2 02 03 55 00
                    └─ Echoes command type 0x03
```

**Status Update:**
```
Bytes around position 26 (fan level):
... 19 01 01 1A 02 03 00 1B 01 03 ...
                 └─ Byte 26: 0x03 = Fan Level 3

Complete context:
Byte 23 (fan mode): 0x00 = Manual (because user set specific level)
Byte 26 (fan level): 0x03 = Level 3
```

### Setting Auto Mode to Quiet

**Command:**
```
A5 22 23 0B 00 A9 02 02 55 00 02 01 01 03 02 00 00
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  └─ Param 2 value
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  └──── Param 2 count
│  │  │  │  │  │  │  │  │  │  │  │  │  │  └─────── Param 2 sub-type
│  │  │  │  │  │  │  │  │  │  │  │  │  └────────── Quiet mode value: 0x01
│  │  │  │  │  │  │  │  │  │  │  │  └───────────── Param 1 count
│  │  │  │  │  │  │  │  │  │  │  └──────────────── Param 1 sub-type
│  │  │  │  │  │  │  │  │  │  └─────────────────── Category: 0x02 = automode
│  │  │  │  │  │  │  │  │  └────────────────────── Fixed: 0x00
│  │  │  │  │  │  │  │  └───────────────────────── Fixed2: 0x55
│  │  │  │  │  │  │  └──────────────────────────── Command Type: 0x02 = fan mode
│  │  │  │  │  │  └─────────────────────────────── Fixed: 0x02
│  │  │  │  │  └────────────────────────────────── Checksum
│  │  │  │  └───────────────────────────────────── Fixed: 0x00
│  │  │  └──────────────────────────────────────── Length: 0x0B (11 bytes)
│  │  └─────────────────────────────────────────── Counter
│  └────────────────────────────────────────────── Type: 0x22
└───────────────────────────────────────────────── Header: 0xA5
```

**Status Update Response:**
```
Bytes around position 54 (auto mode setting):
... 1D 02 2D 00 1E 01 01 20 02 ...
                    └─ Byte 54: 0x01 = Quiet mode

Also shows:
Byte 23: 0x02 = Automatic (fan mode)
Byte 26: 0x05 = Auto (fan adjusts based on air quality)
Byte 54: 0x01 = Quiet (auto mode behavior)
```

## Protocol Summary

This protocol provides comprehensive control over the Levoit Vital 200S air purifier through a structured message format. Key features include:

- **Bidirectional communication** with command acknowledgments
- **Automatic status updates** every 4 seconds for real-time monitoring
- **Multiple control modes** (manual, auto, sleep, pet)
- **Granular control** over fan speeds, display, and air quality modes
- **Extensibility** for future features like scheduling and custom modes

The protocol is designed for reliability with checksums and acknowledgments, while maintaining simplicity with fixed header structures and consistent message formats.
