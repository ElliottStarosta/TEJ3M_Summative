#ifndef DUAL_BUZZER_H
#define DUAL_BUZZER_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Structure to hold a musical note and its duration
struct Note {
  int frequency; 
  int duration;
};

// LED configuration structure
struct LEDConfig {
  int redPin;
  int bluePin;
  int greenPin;
  int yellowPin;
  int whitePin;
};

// LED pattern types
enum LEDPattern {
  PATTERN_FREQUENCY_BANDS,  // Different colors for frequency ranges
  PATTERN_BEAT_PULSE,       // Pulse on beat with intensity
  PATTERN_RAINBOW_CHASE,    // Cycling through colors
  PATTERN_VU_METER,         // VU meter style with frequency
  PATTERN_DISCO_STROBE      // Disco strobe effect
};

class DualBuzzer {
  private:
    int melodyPin;
    int harmonyPin;
    
    Note* melodyNotes;
    Note* harmonyNotes;
    
    int melodyLength;
    int harmonyLength;
    
    unsigned long melodyStartTime;
    unsigned long harmonyStartTime;
    
    int melodyIndex;
    int harmonyIndex;

    // Status flags
    bool melodyPlaying; 
    bool harmonyPlaying;
    
    // Lyrics management
    String lyricsString;
    String* wordsArray;
    int wordCount;
    int currentWordIndex;
    
    // I2C LCD Display
    LiquidCrystal_I2C* lcd;
    int lcdRows;
    int lcdCols;
    
    // LED disco system
    LEDConfig ledConfig;
    bool ledEnabled;
    LEDPattern currentPattern;
    unsigned long lastLEDUpdate;
    int ledUpdateInterval;
    int patternStep;
    int beatCounter;
    
    // LED effect variables
    int currentIntensity;
    bool strobeState;
    unsigned long lastStrobeTime;
    
  public:
    // Constructor
    DualBuzzer(int melodyBuzzerPin, int harmonyBuzzerPin);
    
    // Set the melody and harmony note arrays
    void setMelody(Note* notes, int length);
    void setHarmony(Note* notes, int length);
    
    // Set both melody and harmony at once (change song)
    void setSong(Note* melodyNotes, int melodyLength, Note* harmonyNotes, int harmonyLength);
    
    // Set lyrics for the song (simplified - just a string)
    void setLyrics(String lyrics);
    
    // Set I2C LCD display
    void setLCD(LiquidCrystal_I2C* display, int rows, int columns);
    
    // LED system functions
    void setupLEDs(int redPin, int bluePin, int greenPin, int yellowPin, int whitePin);
    void setLEDPattern(LEDPattern pattern);
    void enableLEDs(bool enable);
    
    // Start playing both parts
    void play();
    
    // Start playing individual parts
    void playMelody();
    void playHarmony();
    
    // Stop playing
    void stop();
    void stopMelody();
    void stopHarmony();
    
    // Update function to be called in the main loop
    void update();
    
    // Check if playing is complete
    bool isPlaying();
    
    // Internal function to display lyrics
    void updateLyrics();
    
    // Sliding lyrics functions
    void updateSlidingLyrics();
    
    // Clear lyrics display
    void clearLyrics();
    
  private:
    // Helper function to split lyrics into words
    void splitLyrics();
    
    // LED control functions
    void updateLEDs();
    void setLEDColor(int red, int green, int blue, int yellow, int white);
    void applyFrequencyBands();
    void applyBeatPulse();
    void applyRainbowChase();
    void applyVUMeter();
    void applyDiscoStrobe();
    int getFrequencyBand(int frequency);
    int mapFrequencyToIntensity(int frequency);
};

#endif