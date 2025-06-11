#ifndef DUAL_BUZZER_H
#define DUAL_BUZZER_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Structure to hold a musical note and its duration
struct Note {
  int frequency; 
  int duration;
};

// Structure to hold lyrics with timing information
struct LyricLine {
  const char* text;
  int noteIndex;
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
    LyricLine* lyrics;
    int lyricsLength;
    int currentLyricIndex;
    
    // I2C LCD Display
    LiquidCrystal_I2C* lcd; // I2C LCD display
    int lcdRows;
    int lcdCols;
    
  public:
    // Constructor
    DualBuzzer(int melodyBuzzerPin, int harmonyBuzzerPin);
    
    // Set the melody and harmony note arrays
    void setMelody(Note* notes, int length);
    void setHarmony(Note* notes, int length);
    
    // Set both melody and harmony at once (change song)
    void setSong(Note* melodyNotes, int melodyLength, Note* harmonyNotes, int harmonyLength);
    
    // Set lyrics for the song
    void setLyrics(LyricLine* lyricLines, int length);
    
    // Set I2C LCD display
    void setLCD(LiquidCrystal_I2C* display, int rows, int columns);
    
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
    
    // Clear lyrics display
    void clearLyrics();
};
#endif