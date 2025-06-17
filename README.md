# Arduino Karaoke Machine -- TEJ3M Summative

A comprehensive karaoke system built on Arduino featuring dual-buzzer harmonized music, synchronized LED light shows, LCD lyric display with scrolling teleprompter, and full serial command interface.

![Arduino Karaoke](https://img.shields.io/badge/Arduino-Karaoke%20Machine-blue?style=for-the-badge&logo=arduino)
![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)

## Project Overview

This Arduino-based karaoke machine turns your microcontroller into a complete entertainment system. The system includes dual-buzzer harmonized music playback with separate melody and harmony channels, a scrolling LCD teleprompter with word-synchronized lyrics, six different LED visualization patterns that respond to the music, an interactive serial command interface for complete system control, four complete children's songs with full verses and choruses, and an auto-play mode with user interaction prompts.

## Key Features

### Music System
The dual buzzer setup plays separate melody and harmony tracks simultaneously. The system comes loaded with four popular children's songs: Twinkle Twinkle Little Star, Jingle Bells, ABC Song, and Mary Had a Little Lamb.

### LCD Teleprompter System
The LCD display features scrolling lyrics with a custom right-to-left scrolling function and shows current song information including title and number.

### LED Light Show (6 Patterns)
The system offers six distinct LED visualization patterns:


1. **Rainbow Chase** - Colorful sequential LED chasing
2. **Note Mapping** - LEDS that are mapped to different notes"
3. **Random Notes** -- LEDS play randoming after each note

### Serial Command Interface
Complete control via USB serial connection with over 15 commands for playback control, system settings, information queries, and interactive responses.

#### Playback Control
```
play <0-2>     - Play specific song by number
stop           - Stop current playback
pause          - Pause/resume playback
```

#### System Settings
```
auto on/off    - Enable/disable continuous auto-play
led on/off     - Enable/disable LED light show
pattern <0-3>  - Change LED visualization pattern
```

#### Information Commands
```
list           - Show all available songs
status         - Display complete system status
help           - Show all available commands
```

#### Interactive Responses
```
yes/y          - Play song again (when prompted)
no/n           - Skip to next song (when prompted)
```

## Hardware Requirements

### Core Components
- Arduino Uno R4
- 2x Piezo Buzzers (passive, for melody and harmony)
- 2x NPN Transistors
- 16x2 I2C LCD Display (with I2C backpack)
- 5x LEDs: Red, Green, Blue, Yellow, White
- 5x 220Ω Resistors (for LED current limiting)
- 2x 1kΩ Resistors (for buzzer current limiting)
- Breadboard and jumper wires

### Pin Configuration
```cpp
// Buzzer Pins
Melody Buzzer:  Pin 9
Harmony Buzzer: Pin 10

// LED Pins
Red LED:        Pin 3
Green LED:      Pin 5
Blue LED:       Pin 6
Yellow LED:     Pin 11
White LED:      Pin 12

// I2C LCD
SDA:            A4
SCL:            A5
LCD Address:    0x27 (default)
```

## Software Dependencies

### Required Libraries
```cpp
#include <LiquidCrystal_I2C.h>  // For LCD control
#include "DualBuzzer.h"         // Custom buzzer management
#include "pitches.h"            // Musical note frequencies
```

### Installation
Install the LiquidCrystal_I2C library through the Arduino IDE Library Manager. Download the DualBuzzer.h and pitches.h files (included in the project) and place all files in the same directory as main.ino.

## Quick Start Guide

### Hardware Setup
Connect the Arduino Uno with Pin 9 to the melody buzzer positive terminal, Pin 10 to the harmony buzzer positive terminal, Pins 3, 5, 6, 11, and 12 to their respective LEDs through 220Ω resistors to ground, A4 to LCD SDA, A5 to LCD SCL, 5V to LCD VCC and buzzer negative pins, and GND to LCD ground and LED common ground.

### Upload Code
Connect the Arduino via USB, open Arduino IDE, load main.ino, select the correct board and port, then upload the code.

### Getting Started
Open the Serial Monitor at 9600 baud rate. Type "help" to see all available commands, "list" to view available songs, and "play 0" to start the first song.

## Usage Examples

### Basic Playback
```
> list
Available songs:
  0: Twinkle Little Star
  1: Jingle Bells
  2: Mary Had a Little Lamb

> play 1
Playing: Jingle Bells
```

### Customize Experience
```
> led on
LEDs enabled.

> pattern 2
LED pattern set to: Note Mapping

> auto on
Auto-play enabled.
```

### Interactive Mode
```
=== Song Finished ===
Play 'Twinkle Little Star' again? (yes/no)
You have 10 seconds to respond...

> yes
Playing song again...
```

## Technical Implementation

The project demonstrates several key programming concepts including conditional structures with extensive if/else logic for command processing, loops for animations and LED patterns and music playback, arrays for song data storage and lyric timing and LED patterns, functions with modular code organization using over 15 custom functions, PROGMEM for flash memory storage of large datasets, and state management for complex state tracking during playback control.

## Song Library

### Available Songs

| # | Song Title | Duration |
|---|------------|----------|
| 0 | Twinkle Twinkle Little Star | ~45s |
| 1 | Jingle Bells | ~35s |
| 2 | Mary Had a Little Lamb | ~25s |

### Adding New Songs
To add new songs, define melody and harmony note arrays in PROGMEM, create lyric timing arrays with word synchronization, add to the songs[] array structure, and update the SONG_COUNT variable.


## Troubleshooting

### Common Issues

**LCD not displaying**
Check I2C connections (SDA/SCL), verify the LCD address using an I2C scanner, and ensure proper power supply.

**No sound from buzzers**
Verify buzzer polarity, check pin connections (9 and 10), and ensure buzzers are the passive type.

**LEDs not working**
Check resistor values (220Ω recommended), verify pin connections, and ensure adequate power supply.

**Serial commands not responding**
Set baud rate to 9600, check USB cable connection, and ensure proper line endings.

## Contributors
Author: **Elliott Starosta**

## **Happy Singing! 🎵✨**
