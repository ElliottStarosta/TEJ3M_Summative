# Arduino Karaoke Machine 🎤🎵

A comprehensive multi-faceted karaoke system built on Arduino featuring dual-buzzer harmonized music, synchronized LED light shows, LCD lyric display with scrolling teleprompter, and full serial command interface.

![Arduino Karaoke](https://img.shields.io/badge/Arduino-Karaoke%20Machine-blue?style=for-the-badge&logo=arduino)
![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)

## 🎯 Project Overview

This Arduino karaoke machine transforms your microcontroller into a complete entertainment system with:
- **Dual-buzzer harmonized music playback** (melody + harmony channels)
- **Scrolling LCD teleprompter** with word-synchronized lyrics
- **6 different LED visualization patterns** that dance to the music
- **Interactive serial command interface** for full system control
- **4 complete children's songs** with full verses and choruses
- **Auto-play mode** with user interaction prompts

## ✨ Key Features

### 🎵 Music System
- **Dual Buzzer Harmony**: Separate melody and harmony tracks played simultaneously
- **4 Songs**:
  - 🌟 Twinkle Twinkle Little Star
  - 🔔 Jingle Bells
  - 🔤 ABC Song
  - 🐑 Mary Had a Little Lamb

### 📺 LCD Teleprompter System
- **Scrolling Lyrics**: Custom right-to-left scrolling function
- **Song Information Display**: Shows current song title and number

### 💡 LED Light Show (6 Patterns)
1. **Frequency Bands** - LEDs respond to note frequencies
2. **Beat Pulse** - Synchronized pulsing with rhythm
3. **Rainbow Chase** - Colorful sequential LED chasing
4. **VU Meter** - Volume/intensity visualization
5. **Disco Strobe** - High-energy flashing patterns
6. **Sequential Notes** - LEDs light up with individual notes

### 🖥️ Serial Command Interface
Complete control via USB serial connection with 15+ commands:

#### Playback Control
```
play <0-3>     - Play specific song by number
stop           - Stop current playback
pause          - Pause/resume playback
```

#### System Settings
```
auto on/off    - Enable/disable continuous auto-play
led on/off     - Enable/disable LED light show
pattern <0-5>  - Change LED visualization pattern
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

## 🔧 Hardware Requirements

### Core Components
- **Arduino Uno R4**
- **2x Piezo Buzzers** (passive, for melody and harmony)
- **2x NPN Transistors**
- **16x2 I2C LCD Display** (with I2C backpack)
- **5x LEDs**: Red, Green, Blue, Yellow, White
- **5x 220Ω Resistors** (for LED current limiting)
- **2x 1kΩ Resistors** (for buzzer current limiting)
- **Breadboard and jumper wires**

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

## 📦 Software Dependencies

### Required Libraries
```cpp
#include <LiquidCrystal_I2C.h>  // For LCD control
#include "DualBuzzer.h"         // Custom buzzer management
#include "pitches.h"            // Musical note frequencies
```

### Installation
1. Install **LiquidCrystal_I2C** library via Arduino IDE Library Manager
2. Download **DualBuzzer.h** and **pitches.h** (included in project)
3. Place all files in same directory as main.ino

## 🚀 Quick Start Guide

### 1. Hardware Setup
```
Arduino Uno
├── Pin 9  → Melody Buzzer (+)
├── Pin 10 → Harmony Buzzer (+)
├── Pin 3  → Red LED → 220Ω → GND
├── Pin 5  → Green LED → 220Ω → GND
├── Pin 6  → Blue LED → 220Ω → GND
├── Pin 11 → Yellow LED → 220Ω → GND
├── Pin 12 → White LED → 220Ω → GND
├── A4     → LCD SDA
├── A5     → LCD SCL
├── 5V     → LCD VCC & Buzzer (-) pins
└── GND    → LCD GND & LED common
```

### 2. Upload Code
1. Connect Arduino via USB
2. Open Arduino IDE
3. Load main.ino
4. Select correct board and port
5. Upload code

### 3. Start Using
1. Open Serial Monitor (9600 baud)
2. Type `help` to see all commands
3. Type `list` to see available songs
4. Type `play 0` to start first song

## 🎮 Usage Examples

### Basic Playback
```
> list
Available songs:
  0: Twinkle Little Star
  1: Jingle Bells
  2: ABC Song
  3: Mary Had a Little Lamb

> play 1
Playing: Jingle Bells
```

### Customize Experience
```
> led on
LEDs enabled.

> pattern 2
LED pattern set to: Rainbow Chase

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

## 🛠️ Technical Implementation

### Programming Elements Used
- ✅ **Conditional Structures**: Extensive if/else logic for command processing
- ✅ **Loops**: For animations, LED patterns, and music playback
- ✅ **Arrays**: Song data storage, lyric timing, LED patterns
- ✅ **Functions**: Modular code organization with 15+ custom functions
- ✅ **PROGMEM**: Flash memory storage for large datasets
- ✅ **State Management**: Complex state tracking for playback control


## 🎵 Song Library

### Available Songs

| # | Song Title | Duration |
|---|------------|----------|
| 0 | Twinkle Twinkle Little Star | ~45s |
| 1 | Jingle Bells | ~35s |
| 2 | ABC Song | ~30s |
| 3 | Mary Had a Little Lamb | ~25s |

### Adding New Songs
1. Define melody and harmony note arrays in PROGMEM
2. Create lyric timing array with word synchronization
3. Add to songs[] array structure
4. Update SONG_COUNT variable

## 🎨 LED Patterns Explained

### Pattern Details
- **Pattern 0 - Frequency Bands**: LEDs brightness varies with note pitch
- **Pattern 1 - Beat Pulse**: All LEDs pulse together with rhythm
- **Pattern 2 - Rainbow Chase**: Sequential color cycling across LEDs
- **Pattern 3 - VU Meter**: Progressive LED activation like audio meter
- **Pattern 4 - Disco Strobe**: High-energy random flashing
- **Pattern 5 - Sequential Notes**: Individual LED per note played

## 🐛 Troubleshooting

### Common Issues

**LCD not displaying**
- Check I2C connections (SDA/SCL)
- Verify LCD address (use I2C scanner)
- Ensure proper power supply

**No sound from buzzers**
- Verify buzzer polarity
- Check pin connections (9 and 10)
- Ensure buzzers are passive type

**LEDs not working**
- Check resistor values (220Ω recommended)
- Verify pin connections
- Ensure adequate power supply

**Serial commands not responding**
- Set baud rate to 9600
- Check USB cable connection
- Ensure proper line endings

## **Happy Singing! 🎵✨**
