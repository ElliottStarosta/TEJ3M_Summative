#ifndef DUAL_BUZZER_H
#define DUAL_BUZZER_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/**
 * @struct Note
 * @brief Structure to hold a musical note and its duration
 */
struct Note {
    int frequency;  // Hz
    int duration;   // milliseconds
};

/**
 * @struct LyricTiming
 * @brief Structure to synchronize lyrics with musical notes
 */
struct LyricTiming {
    String word;      // Word or phrase to display
    int noteIndex;    // Note index for timing
};

/**
 * @struct LEDConfig
 * @brief Configuration structure for LED pin assignments
 */
struct LEDConfig {
    int redPin;
    int bluePin;
    int greenPin;
    int yellowPin;
    int whitePin;
};

/**
 * @enum LEDPattern
 * @brief Enumeration of available LED light show patterns
 */
enum LEDPattern {
    PATTERN_FREQUENCY_BANDS,    // Colors based on frequency ranges
    PATTERN_BEAT_PULSE,         // LEDs pulse with beat intensity
    PATTERN_RAINBOW_CHASE,      // Continuous color cycling
    PATTERN_VU_METER,           // Visual music volume/frequency
    PATTERN_DISCO_STROBE,       // Strobe effect with music
    PATTERN_SEQUENTIAL_NOTES    // Sequential LED activation
};

/**
 * @class DualBuzzer
 * @brief Controls dual buzzers for melody and harmony playback
 */
class DualBuzzer {
private:
    // Hardware pins
    int melodyPin;
    int harmonyPin;

    // Music data
    Note* melodyNotes;
    Note* harmonyNotes;
    int melodyLength;
    int harmonyLength;

    // Timing control
    unsigned long melodyStartTime;
    unsigned long harmonyStartTime;
    int melodyIndex;
    int harmonyIndex;

    // Playback status
    bool melodyPlaying;
    bool harmonyPlaying;

    // Lyrics system
    LyricTiming* lyrics;
    int lyricsCount;
    int currentLyricIndex;

    // LCD display
    LiquidCrystal_I2C* lcd;
    int lcdRows;
    int lcdCols;

    // LED system
    LEDConfig ledConfig;
    bool ledEnabled;
    LEDPattern currentPattern;
    unsigned long lastLEDUpdate;
    int ledUpdateInterval;
    int patternStep;
    int beatCounter;
    int lastMelodyFreq;
    int lastHarmonyFreq;
    unsigned long noteChangeTime;
    bool noteJustChanged;

    // LED effects
    int currentIntensity;
    bool strobeState;
    unsigned long lastStrobeTime;

    // Idle mode
    int idleAnimationStep;
    bool isIdleMode;
    unsigned long lastIdleUpdate;

public:
    // Constructor
    DualBuzzer(int melodyBuzzerPin, int harmonyBuzzerPin);

    // Music setup
    void setMelody(Note* notes, int length);
    void setHarmony(Note* notes, int length);
    void setSong(Note* melodyNotes, int melodyLength, Note* harmonyNotes, int harmonyLength);
    void setLyrics(LyricTiming* timings, int count);

    // Display setup
    void setLCD(LiquidCrystal_I2C* display, int rows, int columns);

    // LED setup and control
    void setupLEDs(int redPin, int bluePin, int greenPin, int yellowPin, int whitePin);
    void setLEDPattern(LEDPattern pattern);
    void enableLEDs(bool enable);

    // Playback control
    void play();              // Start both parts
    void playMelody();        // Start melody only
    void playHarmony();       // Start harmony only
    void stop();              // Stop all
    void stopMelody();        // Stop melody only
    void stopHarmony();       // Stop harmony only

    // Main update loop
    void update();            // Call in main loop
    bool isPlaying();         // Check playback status

    // Display functions
    void updateLyrics();
    void updateSlidingLyrics();
    void clearLyrics();

    // LED control
    void setLEDColor(int red, int green, int blue, int yellow, int white);
    void lightLEDForNote(int freq);
    void playSequenceWithLEDs(const Note* sequence, int length, int buzzerPin);

    // Idle mode
    void startIdleMode();
    void stopIdleMode();

private:
    // Helper functions
    void splitLyrics();

    // LED pattern implementations
    void updateLEDs();
    void applyFrequencyBands();
    void applyBeatPulse();
    void applyRainbowChase();
    void applyVUMeter();
    void applyDiscoStrobe();
    void applySequentialNotes();
    
    // LED utilities
    int getFrequencyBand(int frequency);
    int mapFrequencyToIntensity(int frequency);
    void showIdleLCD();
};

#endif
