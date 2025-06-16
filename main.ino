/**
 * @file main.ino
 * @brief Main Arduino sketch for dual buzzer music player with LED effects
 * 
 * @details This sketch implements a karokee system using two buzzers
 * for melody and harmony, with synchronized LED effects and LCD display for
 * lyrics. It includes multiple songs and visual patterns.
 * 
 * @author Elliott Starosta
 * @date June 16, 2025
 */

#include "DualBuzzer.h"
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// Pin definitions
const int MELODY_BUZZER_PIN = 9;
const int HARMONY_BUZZER_PIN = 10;

// LED pin definitions
const int LED_RED_PIN = 3;
const int LED_GREEN_PIN = 5;
const int LED_BLUE_PIN = 6;
const int LED_YELLOW_PIN = 11;
const int LED_WHITE_PIN = 12;

// I2C LCD configuration
const int LCD_ADDRESS = 0x27;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Initialize I2C LCD
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
bool lcdAvailable = false;


// Create DualBuzzer instance
DualBuzzer buzzer(MELODY_BUZZER_PIN, HARMONY_BUZZER_PIN);



// Startup chime notes
const Note startupChime[] PROGMEM = {
  {NOTE_C4, 200}, {NOTE_E4, 200}, {NOTE_G4, 200}, {NOTE_C5, 400},
  {NOTE_G4, 150}, {NOTE_C5, 300}
};

// Song 1: Twinkle Twinkle Little Star - PROGMEM
const Note twinkleMelody[] PROGMEM = {
  // Verse 1: "Twinkle, twinkle, little star"
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
  
  // "How I wonder what you are"
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800},
  
  // Verse 2: "Up above the world so high"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
  
  // "Like a diamond in the sky"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
  
  // Verse 3: "When the blazing sun is gone"
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
  
  // "When he nothing shines upon"
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800},
  
  // Verse 4: "Then you show your little light"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
  
  // "Twinkle, twinkle, all the night"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
  
  // Final verse: "Twinkle, twinkle, little star"
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
  
  // "How I wonder what you are"
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800}
};


const Note twinkleHarmony[] PROGMEM = {
  // Verse 1 harmony
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_B4, 400}, {NOTE_B4, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 800},
  
  // Verse 2 harmony
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_B3, 800},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_B3, 800},
  
  // Verse 3 harmony
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_B4, 400}, {NOTE_B4, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 800},
  
  // Verse 4 harmony
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_B3, 800},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_B3, 800},
  
  // Final verse harmony
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_B4, 400}, {NOTE_B4, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 800}
};

// Word-based lyrics for Twinkle Twinkle Little Star - PROGMEM

const LyricTiming twinkleLyricTimings[] PROGMEM = {
    // First verse
    {"Twinkle", 0}, {"twinkle", 2}, {"little", 4}, {"star", 6},
    {"How", 7}, {"I", 8}, {"wonder", 9}, {"what", 11}, {"you", 12}, {"are", 13},

    // Second verse
    {"Up", 14}, {"above", 15}, {"the", 17}, {"world", 18}, {"so", 19}, {"high", 20},
    {"Like", 21}, {"a", 22}, {"diamond", 23}, {"in", 25}, {"the", 26}, {"sky", 27},

    // Third verse
    {"When", 28}, {"the", 29}, {"blazing", 30}, {"sun", 32}, {"is", 33}, {"gone", 34},
    {"When", 35}, {"he", 36}, {"nothing", 37}, {"shines", 39}, {"upon", 40},

    // Fourth verse
    {"Then", 42}, {"you", 43}, {"show", 44}, {"your", 45}, {"little", 46}, {"light", 48},
    {"Twinkle", 49}, {"twinkle", 51}, {"all", 53}, {"the", 54}, {"night", 55},

    // Final verse
    {"Twinkle", 56}, {"twinkle", 58}, {"little", 60}, {"star", 62},
    {"How", 63}, {"I", 64}, {"wonder", 65}, {"what", 67}, {"you", 68}, {"are", 69}
};


// Song 2: Jingle Bells - PROGMEM
const Note jingleMelody[] PROGMEM = {
  // "Jingle bells, jingle bells"
  {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
  {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
  {NOTE_E4, 300}, {NOTE_G4, 300}, {NOTE_C4, 450}, {NOTE_D4, 150},
  {NOTE_E4, 1200},
  
  // "Jingle all the way"
  {NOTE_F4, 300}, {NOTE_F4, 300}, {NOTE_F4, 450}, {NOTE_F4, 150},
  {NOTE_F4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300},
  {NOTE_E4, 300}, {NOTE_D4, 300}, {NOTE_D4, 300}, {NOTE_E4, 300},
  {NOTE_D4, 600}, {NOTE_G4, 600},
  
  // "Oh what fun it is to ride"
  {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
  {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
  {NOTE_E4, 300}, {NOTE_G4, 300}, {NOTE_C4, 450}, {NOTE_D4, 150},
  {NOTE_E4, 1200},
  
  // "In a one-horse open sleigh"
  {NOTE_F4, 300}, {NOTE_F4, 300}, {NOTE_F4, 450}, {NOTE_F4, 150},
  {NOTE_F4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300},
  {NOTE_G4, 300}, {NOTE_G4, 300}, {NOTE_F4, 300}, {NOTE_D4, 300},
  {NOTE_C4, 1200}
};

const Note jingleHarmony[] PROGMEM = {
  // Harmony for "Jingle bells, jingle bells"
  {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 600},
  {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 600},
  {NOTE_C4, 300}, {NOTE_E4, 300}, {NOTE_G3, 450}, {NOTE_B3, 150},
  {NOTE_C4, 1200},
  
  // Harmony for "Jingle all the way"
  {NOTE_D4, 300}, {NOTE_D4, 300}, {NOTE_D4, 450}, {NOTE_D4, 150},
  {NOTE_D4, 300}, {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 300},
  {NOTE_G3, 300}, {NOTE_B3, 300}, {NOTE_B3, 300}, {NOTE_C4, 300},
  {NOTE_B3, 600}, {NOTE_D4, 600},
  
  // Repeat harmony pattern
  {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 600},
  {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 600},
  {NOTE_C4, 300}, {NOTE_E4, 300}, {NOTE_G3, 450}, {NOTE_B3, 150},
  {NOTE_C4, 1200},
  
  {NOTE_D4, 300}, {NOTE_D4, 300}, {NOTE_D4, 450}, {NOTE_D4, 150},
  {NOTE_D4, 300}, {NOTE_C4, 300}, {NOTE_C4, 300}, {NOTE_C4, 300},
  {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_D4, 300}, {NOTE_B3, 300},
  {NOTE_G3, 1200}
};

const LyricTiming jingleLyricTimings[] PROGMEM = {
    // First verse: "Jingle bells, jingle bells, jingle all the way"
    {"Jingle", 0}, {"bells", 1}, {"jingle", 3}, {"bells", 4}, {"jingle", 6},
    {"all", 7}, {"the", 8}, {"way", 9},

    // Second part: "Oh what fun it is to ride in a one-horse open sleigh"
    {"Oh", 11}, {"what", 12}, {"fun", 13}, {"it", 14}, {"is", 15}, {"to", 16}, {"ride", 17},
    {"in", 18}, {"a", 19}, {"one", 20}, {"horse", 21}, {"open", 22}, {"sleigh", 23},

    // Repeat: "Jingle bells, jingle bells, jingle all the way"
    {"Jingle", 25}, {"bells", 26}, {"jingle", 28}, {"bells", 29}, {"jingle", 31},
    {"all", 32}, {"the", 33}, {"way", 34},

    // Final: "Oh what fun it is to ride in a one-horse open sleigh"
    {"Oh", 36}, {"what", 37}, {"fun", 38}, {"it", 39}, {"is", 40}, {"to", 41}, {"ride", 42},
    {"in", 43}, {"a", 44}, {"one", 45}, {"horse", 46}, {"open", 47}, {"sleigh", 48}
};
// Song 3: ABC
const Note abcMelody[] PROGMEM = {
  // "A B C D E F G"
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},

  // "H I J K L M N"
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800},

  // "O P Q R S"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, 

  // "T U V"
  {NOTE_E4, 400}, {NOTE_D4, 800},

  // "W X Y and Z"
  {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},

  // "Now I know my ABCs"
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},

  // "Next time won't you sing with me"
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800}
};

const Note abcHarmony[] PROGMEM = {
  // "A B C D E F G"
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_B4, 400}, {NOTE_B4, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},

  // "H I J K L M N"
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 800},

  // "O P Q R S"
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400},

  // "T U V"
  {NOTE_C4, 400}, {NOTE_B3, 800},

  // "W X Y and Z"
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_B3, 800},

  // "Now I know my ABCs"
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_B4, 400}, {NOTE_B4, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},

  // "Next time won't you sing with me"
  {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
  {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 800}
};

const LyricTiming abcLyricTimings[] PROGMEM = {
  // "A B C D E F G"
  {"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}, {"E", 4}, {"F", 5}, {"G", 6},

  // "H I J K L M N"
  {"H", 7}, {"I", 8}, {"J", 9}, {"K", 10}, {"L", 11}, {"M", 12}, {"N", 13},

  // "O P Q R S"
  {"O", 14}, {"P", 15}, {"Q", 16}, {"R", 17}, {"S", 18},

  // "T U V"
  {"T", 19}, {"U", 20}, {"V", 21},

  // "W X Y and Z"
  {"W", 22}, {"X", 23}, {"Y", 24}, {"and", 25}, {"Z", 26},

  // "Now I know my ABCs"
  {"Now", 27}, {"I", 28}, {"know", 29}, {"my", 30}, {"A", 31}, {"B", 32}, {"Cs", 33},

  // "Next time won't you sing with me"
  {"Next", 34}, {"time", 35}, {"won't", 36}, {"you", 37}, {"sing", 38}, {"with", 39}, {"me", 40}
};

// Song 4: Mary Had a Little Lamb

const Note maryMelody[] PROGMEM = {
  // Verse 1
  {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_C4, 400}, {NOTE_D4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_E4, 800},
  
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_D4, 800},
  
  {NOTE_E4, 400}, {NOTE_G4, 400}, {NOTE_G4, 800},
  
  {NOTE_E4, 400}, {NOTE_D4, 400}, {NOTE_C4, 400}, {NOTE_D4, 400},
  {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
  
  {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_E4, 400}, {NOTE_D4, 400},
  {NOTE_C4, 800}
};

const Note maryHarmony[] PROGMEM = {
  // Verse 1 harmony
  {NOTE_C4, 400}, {NOTE_B3, 400}, {NOTE_A3, 400}, {NOTE_B3, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_C4, 800},
  
  {NOTE_B3, 400}, {NOTE_B3, 400}, {NOTE_B3, 800},
  
  {NOTE_C4, 400}, {NOTE_E4, 400}, {NOTE_E4, 800},
  
  {NOTE_C4, 400}, {NOTE_B3, 400}, {NOTE_A3, 400}, {NOTE_B3, 400},
  {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_C4, 400},
  
  {NOTE_B3, 400}, {NOTE_B3, 400}, {NOTE_C4, 400}, {NOTE_B3, 400},
  {NOTE_A3, 800}
};

// Lyrics timings: word and index of melody note start
const LyricTiming maryLyricTimings[] PROGMEM = {
  {"Mary", 0}, {"had", 1}, {"a", 2}, {"little", 3},
  {"lamb,", 4}, {"little", 5}, {"lamb,", 6},
  
  {"Little", 7}, {"lamb,", 8}, {"its", 9},
  
  {"Fleece", 10}, {"was", 11}, {"white", 12},
  
  {"As", 13}, {"snow.", 14}, {"Everywhere", 15}, {"that", 16},
  {"Mary", 17}, {"went,", 18}, {"went,", 19},
  
  {"Mary", 20}, {"went,", 21}, {"the", 22}, {"lamb", 23}
};


// Song management variables
struct Song {
  const Note* melody;
  int melodyLength;
  const Note* harmony;
  int harmonyLength;
  const LyricTiming* lyrics;
  int lyricsCount;
  const char* name;
};

const Song songs[] PROGMEM = {
  {
    twinkleMelody, sizeof(twinkleMelody) / sizeof(twinkleMelody[0]),
    twinkleHarmony, sizeof(twinkleHarmony) / sizeof(twinkleHarmony[0]),
    twinkleLyricTimings, sizeof(twinkleLyricTimings) / sizeof(twinkleLyricTimings[0]),
    "Twinkle Little Star"
  },
  {
    jingleMelody, sizeof(jingleMelody) / sizeof(jingleMelody[0]),
    jingleHarmony, sizeof(jingleHarmony) / sizeof(jingleHarmony[0]),
    jingleLyricTimings, sizeof(jingleLyricTimings) / sizeof(jingleLyricTimings[0]),
    "Jingle Bells"
  },
  {
    abcMelody, sizeof(abcMelody) / sizeof(abcMelody[0]),
    abcHarmony, sizeof(abcHarmony) / sizeof(abcHarmony[0]),
    abcLyricTimings, sizeof(abcLyricTimings) / sizeof(abcLyricTimings[0]),
    "ABC Song"
  },
  {
    maryMelody, sizeof(maryMelody) / sizeof(maryMelody[0]),
    maryHarmony, sizeof(maryHarmony) / sizeof(maryHarmony[0]),
    maryLyricTimings, sizeof(maryLyricTimings) / sizeof(maryLyricTimings[0]),
    "Mary Had a Little Lamb"
  }
};


const int SONG_COUNT = sizeof(songs) / sizeof(songs[0]);
int currentSong = 0;
bool autoPlay = false;
bool ledsEnabled = true;
int currentLEDPattern = 0;
const int LED_PATTERN_COUNT = 6;

bool userStopped = false; // Track if user manually stopped/paused
bool waitingForPlayAgain = false; // Track if we're waiting for play again response
bool wasPlaying = false;
unsigned long playAgainTimeout = 0; 
const unsigned long PLAY_AGAIN_WAIT_TIME = 10000; // 10 seconds to respond

// Serial command handling
String serialBuffer = "";
unsigned long lastSerialCheck = 0;
const unsigned long SERIAL_CHECK_INTERVAL = 100; // Check serial every 100ms

// Pattern
String patternNames[] = {"Frequency Bands", "Beat Pulse", "Rainbow Chase", "VU Meter", "Disco Strobe", "Sequential Notes"};

void setup() {
  // Initialize Serial for commands
  Serial.begin(9600);
  Serial.println();
  Serial.println("=== Music Player ===");
  Serial.println("Commands:");
  Serial.println("  play <song_number> - Play specific song (0-" + String(SONG_COUNT-1) + ")");
  Serial.println("  stop - Stop current playback");
  Serial.println("  pause - Pause/resume playback");
  Serial.println("  list - List all available songs");
  Serial.println("  auto on/off - Enable/disable auto-play");
  Serial.println("  led on/off - Enable/disable LEDs");
  Serial.println("  pattern <0-4> - Change LED pattern");
  Serial.println("  status - Show current status");
  Serial.println("  yes/y - PlaYy song again (when prompted)");
  Serial.println("  no/n - Skip to next song (when prompted)");
  Serial.println("  help - Show this help menu");
  Serial.println();

  // Initialize I2C LCD
  lcd.init();
  lcd.backlight();
  lcdAvailable = true;
  
  // Set up the buzzer with LCD display
  buzzer.setLCD(&lcd, LCD_ROWS, LCD_COLS);
  
  // Setup LEDs
  buzzer.setupLEDs(LED_RED_PIN, LED_BLUE_PIN, LED_GREEN_PIN, LED_YELLOW_PIN, LED_WHITE_PIN);
  buzzer.enableLEDs(ledsEnabled);
  buzzer.setLEDPattern(PATTERN_SEQUENTIAL_NOTES);

  // Play startup sequence with chime and animation
  playStartupSequence();
  
  // Allow time to read the startup message
  delay(1500);
  
  // Load and start first song
  // loadSong(currentSong);
  // showStatus();
  buzzer.startIdleMode();
  
  Serial.println("System ready! Type 'help' for commands.");
}

void loop() {
  // Update the buzzer state (handles music and LEDs)
  buzzer.update();
  
  // Check for serial commands periodically
  unsigned long currentTime = millis();
  if (currentTime - lastSerialCheck >= SERIAL_CHECK_INTERVAL) {
    handleSerialCommands();
    lastSerialCheck = currentTime;
  }

  
  // Serial.println(!buzzer.isPlaying());
  if (!buzzer.isPlaying() && !userStopped && !waitingForPlayAgain) {
    
    if (wasPlaying) {
      Serial.println("\n=== Song Finished ===");
      char songName[50];
      strcpy_P(songName, (const char*)pgm_read_ptr(&songs[currentSong].name));
      Serial.println("Play '" + String(songName) + "' again? (yes/no)");
      Serial.println("You have 10 seconds to respond...");
      buzzer.stop();
      
      waitingForPlayAgain = true;
      playAgainTimeout = currentTime + PLAY_AGAIN_WAIT_TIME;
    }
    wasPlaying = false;
  } else if (buzzer.isPlaying()) {
    wasPlaying = true;
  }
  
  
  // Handle play again timeout or auto-play when not waiting for user input
  if (waitingForPlayAgain && currentTime >= playAgainTimeout) {
    // Timeout reached, proceed with auto-play logic
    Serial.println("No response - proceeding with auto-play setting...");
    waitingForPlayAgain = false;
    
    if (autoPlay) {
      moveToNextSong();
    } else {
      Serial.println("Auto-play disabled. Type 'play <song>' to start another song.");
    }
  }
  
  // Reset userStopped flag if we're not playing and not waiting
  if (!buzzer.isPlaying() && !waitingForPlayAgain) {
    userStopped = false;
  }
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    serialBuffer = Serial.readStringUntil('\n');
    processCommand(serialBuffer);
  }
}

void processCommand(String command) {
  command.trim();
  command.toLowerCase();
  
  Serial.println("Command: " + command);
  
  // Handle play again responses
  if (waitingForPlayAgain) {
    if (command == "yes" || command == "y") {
      Serial.println("Playing song again...");
      waitingForPlayAgain = false;
      buzzer.play();
      return;
    } else if (command == "no" || command == "n") {
      Serial.println("Skipping to next song...");
      waitingForPlayAgain = false;
      userStopped = true;
      buzzer.stop();
      if (autoPlay) {
        moveToNextSong();
      } else {
        Serial.println("Auto-play disabled. Type 'play <song>' to start another song.");
        
      }
      return;
    }
    // For other commands during play again prompt, continue processing normally
  }
  
  if (command.startsWith("play ")) {
    String songNumberStr = command.substring(5);
    int songNumber = songNumberStr.toInt();
    
    // Validate song number
    if (songNumber < 0 || songNumber >= SONG_COUNT) {
      Serial.println("ERROR: Invalid song number. Use 0-" + String(SONG_COUNT-1));
      Serial.println("Type 'list' to see available songs.");
      return;
    }
    
    // Stop current playback and load new song
    buzzer.stop();
    buzzer.stopIdleMode();
    currentSong = songNumber;
    loadSong(currentSong);
    delay(2000);
    buzzer.play();
    
    // Reset state flags
    userStopped = false;
    waitingForPlayAgain = false;
    
    char songName[50];
    strcpy_P(songName, (const char*)pgm_read_ptr(&songs[currentSong].name));
    Serial.println("Playing: " + String(songName));
  } 

  else if (command == "play") {
    // Handle "play" without parameters
    Serial.println("What song would you like to play?");
    Serial.println("Available songs:");
    for (int i = 0; i < SONG_COUNT; i++) {
      char songName[50];
      strcpy_P(songName, (const char*)pgm_read_ptr(&songs[i].name));
      Serial.println("  " + String(i) + ": " + String(songName));
    }
    Serial.println("Usage: play <song_number>");
  } else if (command == "stop") {
    buzzer.stop();
    userStopped = true;  // Mark as user-initiated stop
    waitingForPlayAgain = false;  // Cancel any play again prompt
    lcd.clear();
    lcd.print("Stopped");
    Serial.println("Playback stopped.");
    
  } else if (command == "pause") {
    if (buzzer.isPlaying()) {
      buzzer.stop();
      userStopped = true;  // Mark as user-initiated pause
      waitingForPlayAgain = false;  // Cancel any play again prompt
      lcd.clear();
      lcd.print("Paused");
      Serial.println("Playback paused.");
    } else {
      buzzer.play();
      userStopped = false;  // Resume normal operation
      Serial.println("Playback resumed.");
    }
    
  } else if (command == "list") {
    Serial.println("Available songs:");
    for (int i = 0; i < SONG_COUNT; i++) {
      char songName[50];
      strcpy_P(songName, (const char*)pgm_read_ptr(&songs[i].name));
      Serial.println("  " + String(i) + ": " + String(songName));
    }
    
  } else if (command == "auto on") {
    autoPlay = true;
    Serial.println("Auto-play enabled.");
    
  } else if (command == "auto off") {
    autoPlay = false;
    Serial.println("Auto-play disabled.");
    
  } else if (command == "auto") {
    // Handle "auto" without parameters
    Serial.println("Auto-play is currently: " + String(autoPlay ? "Enabled" : "Disabled"));
    Serial.println("Usage: auto <on/off>");
  } else if (command == "led on") {
    ledsEnabled = true;
    buzzer.enableLEDs(true);
    Serial.println("LEDs enabled.");
  } else if (command == "led off") {
    ledsEnabled = false;
    buzzer.enableLEDs(false);
    Serial.println("LEDs disabled.");
    
  } else if (command == "led") {
    // Handle "led" without parameters  
    Serial.println("LEDs are currently: " + String(ledsEnabled ? "Enabled" : "Disabled"));
    Serial.println("Usage: led <on/off>");
  } else if (command.startsWith("pattern ")) {
    String patternStr = command.substring(8);
    int pattern = patternStr.toInt();
    
    if (pattern < 0 || pattern >= LED_PATTERN_COUNT) {
      Serial.println("ERROR: Invalid pattern number. Use 0-" + String(LED_PATTERN_COUNT-1));
      Serial.println("Patterns: 0=Frequency Bands, 1=Beat Pulse, 2=Rainbow Chase, 3=VU Meter, 4=Disco Strobe, 5=Sequential Notes");
      return;
    }
    
    currentLEDPattern = pattern;
    buzzer.setLEDPattern((LEDPattern)pattern);
    
    Serial.println("LED pattern set to: " + patternNames[pattern]);
    
  } else if (command == "pattern") {
    // Handle "pattern" without parameters
    Serial.println("Which LED pattern would you like?");
    Serial.println("Available patterns:");
    Serial.println("  0: Frequency Bands");
    Serial.println("  1: Beat Pulse");
    Serial.println("  2: Rainbow Chase");
    Serial.println("  3: VU Meter");
    Serial.println("  4: Disco Strobe");
    Serial.println("  5: Sequential Notes");
    Serial.println("Current pattern: " + String(patternNames[currentLEDPattern]));
    Serial.println("Usage: pattern <pattern_number>");

  } else if (command == "status") {
    showStatus();
  } else if (command == "help") {
    Serial.println("=== Commands ===");
    Serial.println("play <song_number> - Play specific song (0-" + String(SONG_COUNT-1) + ")");
    Serial.println("stop - Stop current playback");
    Serial.println("pause - Pause/resume playback");
    Serial.println("list - List all available songs");
    Serial.println("auto on/off - Enable/disable auto-play");
    Serial.println("led on/off - Enable/disable LEDs");
    Serial.println("pattern <0-4> - Change LED pattern");
    Serial.println("status - Show current status");
    Serial.println("yes/y - Play song again (when prompted)");
    Serial.println("no/n - Skip to next song (when prompted)");
    Serial.println("help - Show this help menu");
    
  } else if (command == "") {
    // Empty command, ignore
    return;
    
  } else {
    Serial.println("ERROR: Unknown command '" + command + "'");
    Serial.println("Type 'help' for available commands.");
  }
}

void moveToNextSong() {
  // Display song change message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Auto: Next song");
  Serial.println("Auto-play: Switching to next song...");
  delay(1000);
  
  // Switch to next song
  currentSong = (currentSong + 1) % SONG_COUNT;
  loadSong(currentSong);
  buzzer.play();
  
  char songName[50];
  strcpy_P(songName, (const char*)pgm_read_ptr(&songs[currentSong].name));
  Serial.println("Now playing: " + String(songName));
}

void loadSong(int songIndex) {
  if (songIndex < 0 || songIndex >= SONG_COUNT) {
    Serial.println("ERROR: Invalid song index");
    return;
  }

  buzzer.stop();
  
 // Read song data directly from PROGMEM without dynamic allocation
  buzzer.setSong((Note*)pgm_read_ptr(&songs[songIndex].melody), 
                pgm_read_word(&songs[songIndex].melodyLength),
                (Note*)pgm_read_ptr(&songs[songIndex].harmony), 
                pgm_read_word(&songs[songIndex].harmonyLength));
  
  // Read lyrics from PROGMEM
 buzzer.setLyrics((LyricTiming*)pgm_read_ptr(&songs[songIndex].lyrics),
                   pgm_read_word(&songs[songIndex].lyricsCount));
  
  // Display song info on LCD
  lcd.clear();
  lcd.print("Song: ");
  lcd.print(String(songIndex));
  lcd.setCursor(0, 1);
  char songName[50];
  strcpy_P(songName, (const char*)pgm_read_ptr(&songs[songIndex].name));

  String name = String(songName);
  if (name.length() > LCD_COLS) {
    name = name.substring(0, LCD_COLS);
  }
  lcd.print(name);
  
}

void showStatus() {
  Serial.println("=== Current Status ===");
  char songName[50];
  strcpy_P(songName, (const char*)pgm_read_ptr(&songs[currentSong].name));
  Serial.println("Current song: " + String(currentSong) + " (" + String(songName) + ")");
  Serial.println("Playing: " + String(buzzer.isPlaying() ? "Yes" : "No"));
  Serial.println("Auto-play: " + String(autoPlay ? "Enabled" : "Disabled"));
  Serial.println("LEDs: " + String(ledsEnabled ? "Enabled" : "Disabled"));
  Serial.println("User stopped: " + String(userStopped ? "Yes" : "No"));
  Serial.println("Waiting for play again: " + String(waitingForPlayAgain ? "Yes" : "No"));
  
  Serial.println("LED pattern: " + String(currentLEDPattern) + " (" + patternNames[currentLEDPattern] + ")");
  Serial.println("Total songs: " + String(SONG_COUNT));
  Serial.println("=====================");
}

void playStartupSequence() {
  // Clear display
  if (lcdAvailable) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Starting up...");
  }
  
  // Play startup chime with synchronized LEDs
  int chimeLength = sizeof(startupChime) / sizeof(startupChime[0]);
  
  // Use the new synchronized function
  buzzer.playSequenceWithLEDs(startupChime, chimeLength, MELODY_BUZZER_PIN);
  
  // Final flourish - all LEDs flash
  if (lcdAvailable) {
    for (int i = 0; i < 3; i++) {
      buzzer.setLEDColor(255, 255, 255, 255, 255);  // All on
      delay(100);
      buzzer.setLEDColor(0, 0, 0, 0, 0);            // All off
      delay(100);
    }
  }
  
  // Clear startup message
  if (lcdAvailable) {
    lcd.clear();
  }
}

void playStartupSequence1() {
  // Play startup chime
  int chimeLength = sizeof(startupChime) / sizeof(startupChime[0]);
  
  // Welcome animation - Phase 1: Building text
  lcd.clear();
  String welcomeText = "Karokee Machine";
  String authorText = "by Elliott S :)";
  
  // Typewriter effect for welcome text
  for (int i = 0; i <= welcomeText.length(); i++) {
    lcd.setCursor(0, 0);
    lcd.print(welcomeText.substring(0, i));
    
    // Add blinking cursor effect
    if (i < welcomeText.length()) {
      lcd.print("_");
    }
    delay(100);
  }
  
  delay(300);
  
  // Typewriter effect for author text
  for (int i = 0; i <= authorText.length(); i++) {
    lcd.setCursor(0, 1);
    lcd.print(authorText.substring(0, i));
    
    if (i < authorText.length()) {
      lcd.print("_");
    }
    delay(100);
  }
  
  delay(500);
  
  // Phase 2: Play chime with visual effects
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  
  // Play chime notes with LCD animation
  for (int i = 0; i < chimeLength; i++) {
    Note currentNote;
    memcpy_P(&currentNote, &startupChime[i], sizeof(Note));
    
    // Play the note
    tone(MELODY_BUZZER_PIN, currentNote.frequency);
    
    // Create visual effect based on note
    String visualBar = "";
    int barLength = map(currentNote.frequency, 200, 1000, 1, LCD_COLS);
    barLength = constrain(barLength, 1, LCD_COLS);
    
    // Create animated bar
    for (int j = 0; j < LCD_COLS; j++) {
      if (j < barLength) {
        if (j == barLength - 1) {
          visualBar += ">";
        } else {
          visualBar += "=";
        }
      } else {
        visualBar += " ";
      }
    }
    
    lcd.setCursor(0, 1);
    lcd.print(visualBar);
    
    delay(currentNote.duration);
    noTone(MELODY_BUZZER_PIN);
    
    // Brief pause between notes
    delay(50);
  }
  
  // Phase 3: Loading animation with complete visual bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading Songs...");
  
  // Create a complete loading bar that fills up
  for (int progress = 0; progress <= LCD_COLS; progress++) {
    String loadingBar = "";
    
    // Build the loading bar
    for (int j = 0; j < LCD_COLS; j++) {
      if (j < progress) {
        loadingBar += "=";
      } else if (j == progress && progress < LCD_COLS) {
        loadingBar += ">";
      } else {
        loadingBar += " ";
      }
    }
    
    lcd.setCursor(0, 1);
    lcd.print(loadingBar);
    delay(150);
  }
  
  // Show complete bar for a moment
  lcd.setCursor(0, 1);
  String completeBar = "";
  for (int i = 0; i < LCD_COLS; i++) {
    completeBar += "=";
  }
  lcd.print(completeBar);
  delay(500);
  
  // Phase 4: Success message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Ready!    ");
  lcd.setCursor(0, 1);
  lcd.print("Songs: " + String(SONG_COUNT) + " loaded");
  
  // Cool LED wave effect instead of simultaneous flash
  if (ledsEnabled) {
    // Wave effect - chase across LEDs
    for (int wave = 0; wave < 3; wave++) {
      // Forward wave
      for (int led = 0; led < 5; led++) {
        buzzer.setLEDColor(0, 0, 0, 0, 0); // All off first
        
        // Light up current LED with bright color
        switch (led) {
          case 0: buzzer.setLEDColor(255, 0, 0, 0, 0); break; // Red
          case 1: buzzer.setLEDColor(0, 255, 0, 0, 0); break; // Green  
          case 2: buzzer.setLEDColor(0, 0, 255, 0, 0); break; // Blue
          case 3: buzzer.setLEDColor(0, 0, 0, 255, 0); break; // LED 4
          case 4: buzzer.setLEDColor(0, 0, 0, 0, 255); break; // LED 5
        }
        delay(80);
      }
      
      // Backward wave
      for (int led = 4; led >= 0; led--) {
        buzzer.setLEDColor(0, 0, 0, 0, 0); // All off first
        
        // Light up current LED with different color
        switch (led) {
          case 0: buzzer.setLEDColor(0, 255, 255, 0, 0); break;
          case 1: buzzer.setLEDColor(0, 0, 255, 255, 0); break;
          case 2: buzzer.setLEDColor(0, 0, 0, 255, 255); break;
          case 3: buzzer.setLEDColor(255, 0, 0, 0, 255); break;
          case 4: buzzer.setLEDColor(255, 255, 0, 0, 0); break;
        }
        delay(80);
      }
    }
    
    // Final rainbow flash
    buzzer.setLEDColor(255, 128, 0, 255, 128); // Rainbow colors
    delay(200);
    buzzer.setLEDColor(0, 0, 0, 0, 0); // All off
  } else {
    delay(1000); // If LEDs disabled, just wait
  }
  
  delay(500);
  
  // Final startup complete message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  lcd.setCursor(0, 1);
  lcd.print("Type 'help'");
  delay(1500);
}
