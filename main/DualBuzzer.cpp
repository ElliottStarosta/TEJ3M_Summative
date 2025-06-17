#include "DualBuzzer.h"

/**
 * @brief Constructor for DualBuzzer class
 * @param melodyBuzzerPin Pin number for the melody buzzer
 * @param harmonyBuzzerPin Pin number for the harmony buzzer
 * 
 * Initializes all pins, variables, and default settings for the dual buzzer system.
 * Sets up LED configuration with invalid pins (disabled by default).
 */
DualBuzzer::DualBuzzer(int melodyBuzzerPin, int harmonyBuzzerPin) {
  melodyPin = melodyBuzzerPin;
  harmonyPin = harmonyBuzzerPin;
  
  // Initialize pins as outputs
  pinMode(melodyPin, OUTPUT);
  pinMode(harmonyPin, OUTPUT);
  
  // Initialize note arrays and playback variables
  melodyNotes = NULL;
  harmonyNotes = NULL;
  melodyLength = 0;
  harmonyLength = 0;
  
  melodyIndex = 0;
  harmonyIndex = 0;
  
  melodyPlaying = false;
  harmonyPlaying = false;
  
  melodyStartTime = 0;
  harmonyStartTime = 0;
  
  // Initialize lyrics system
  lyrics = NULL;
  lyricsCount = 0;
  currentLyricIndex = 0;
  
  // Initialize LCD display system
  lcd = NULL;
  lcdRows = 0;
  lcdCols = 0;
  
  // Initialize LED system with default settings
  ledEnabled = false;
  currentPattern = PATTERN_FREQUENCY_BANDS;
  lastLEDUpdate = 0;
  ledUpdateInterval = 50; // 20 FPS for smooth effects
  patternStep = 0;
  beatCounter = 0;
  currentIntensity = 0;
  strobeState = false;
  lastStrobeTime = 0;
  lastMelodyFreq = 0;
  lastHarmonyFreq = 0;
  noteChangeTime = 0;
  noteJustChanged = false;

  // Initialize idle mode variables
  lastIdleUpdate = 0;
  idleAnimationStep = 0;
  isIdleMode = false;
  
  // Initialize LED configuration with invalid pins (disabled)
  ledConfig.redPin = -1;
  ledConfig.bluePin = -1;
  ledConfig.greenPin = -1;
  ledConfig.yellowPin = -1;
  ledConfig.whitePin = -1;
}

/**
 * @brief Configure LED pins for visual effects
 * @param redPin Pin number for red LED (use -1 to disable)
 * @param bluePin Pin number for blue LED (use -1 to disable)
 * @param greenPin Pin number for green LED (use -1 to disable)
 * @param yellowPin Pin number for yellow LED (use -1 to disable)
 * @param whitePin Pin number for white LED (use -1 to disable)
 * 
 * Sets up the LED pins and enables the LED system. Only pins with valid
 * numbers (>= 0) will be configured and used.
 */
void DualBuzzer::setupLEDs(int redPin, int bluePin, int greenPin, int yellowPin, int whitePin) {
  ledConfig.redPin = redPin;
  ledConfig.bluePin = bluePin;
  ledConfig.greenPin = greenPin;
  ledConfig.yellowPin = yellowPin;
  ledConfig.whitePin = whitePin;
  
  // Initialize LED pins as outputs (only for valid pins)
  if (redPin >= 0) pinMode(redPin, OUTPUT);
  if (bluePin >= 0) pinMode(bluePin, OUTPUT);
  if (greenPin >= 0) pinMode(greenPin, OUTPUT);
  if (yellowPin >= 0) pinMode(yellowPin, OUTPUT);
  if (whitePin >= 0) pinMode(whitePin, OUTPUT);
  
  // Turn off all LEDs initially
  setLEDColor(0, 0, 0, 0, 0);
  
  ledEnabled = true;
}

/**
 * @brief Set the LED animation pattern
 * @param pattern The LED pattern to use (see LEDPattern enum)
 * 
 * Changes the LED animation pattern and resets animation counters.
 */
void DualBuzzer::setLEDPattern(LEDPattern pattern) {
  currentPattern = pattern;
  patternStep = 0;
  beatCounter = 0;
}

/**
 * @brief Enable or disable LED effects
 * @param enable True to enable LEDs, false to disable and turn off all LEDs
 */
void DualBuzzer::enableLEDs(bool enable) {
  ledEnabled = enable;
  if (!enable) {
    setLEDColor(0, 0, 0, 0, 0);
  }
}

/**
 * @brief Set the melody note sequence
 * @param notes Pointer to array of Note structures (should be in PROGMEM)
 * @param length Number of notes in the melody
 */
void DualBuzzer::setMelody(Note* notes, int length) {
  melodyNotes = notes;
  melodyLength = length;
  melodyIndex = 0;
}

/**
 * @brief Set the harmony note sequence
 * @param notes Pointer to array of Note structures (should be in PROGMEM)
 * @param length Number of notes in the harmony
 */
void DualBuzzer::setHarmony(Note* notes, int length) {
  harmonyNotes = notes;
  harmonyLength = length;
  harmonyIndex = 0;
}

/**
 * @brief Set both melody and harmony for a complete song
 * @param melody Pointer to melody note array
 * @param melodyLen Length of melody array
 * @param harmony Pointer to harmony note array
 * @param harmonyLen Length of harmony array
 * 
 * Stops current playback and configures a new song with both melody and harmony.
 * Resets lyrics position and LED effects.
 */
void DualBuzzer::setSong(Note* melody, int melodyLen, Note* harmony, int harmonyLen) {
  // Stop current playback if any
  stop();
  
  // Set new melody and harmony
  setMelody(melody, melodyLen);
  setHarmony(harmony, harmonyLen);
  
  // Reset lyrics position
  currentLyricIndex = 0;
  
  // Reset LED effects
  patternStep = 0;
  beatCounter = 0;
}

/**
 * @brief Set lyrics timing for synchronized display
 * @param timings Array of LyricTiming structures
 * @param count Number of lyric entries
 * 
 * Configures lyrics that will be displayed in sync with the melody.
 */
void DualBuzzer::setLyrics(LyricTiming* timings, int count) {
  lyrics = timings;
  lyricsCount = count;
  currentLyricIndex = 0;
}

/**
 * @brief Configure LCD display for lyrics
 * @param display Pointer to LiquidCrystal_I2C display object
 * @param rows Number of rows on the display
 * @param columns Number of columns on the display
 */
void DualBuzzer::setLCD(LiquidCrystal_I2C* display, int rows, int columns) {
  lcd = display;
  lcdRows = rows;
  lcdCols = columns;
}

/**
 * @brief Start playing both melody and harmony
 * 
 * Begins playback of the configured song, resets lyrics display,
 * and shows the first lyric if available.
 */
void DualBuzzer::play() {
  playMelody();
  playHarmony();
  
  // Reset lyrics and display first lyric if available
  currentLyricIndex = 0;
  clearLyrics();
  updateLyrics();
}

/**
 * @brief Start playing the melody only
 * 
 * Begins melody playback from the first note and starts the first tone.
 */
void DualBuzzer::playMelody() {
  if (melodyNotes != NULL && melodyLength > 0) {
    melodyPlaying = true;
    melodyIndex = 0;
    melodyStartTime = millis();
    
    // Start playing the first note
    if (melodyNotes[0].frequency > 0) {
      tone(melodyPin, melodyNotes[0].frequency);
    } else {
      noTone(melodyPin); // Rest note
    }
  }
}

/**
 * @brief Start playing the harmony only
 * 
 * Begins harmony playback from the first note and starts the first tone.
 */
void DualBuzzer::playHarmony() {
  if (harmonyNotes != NULL && harmonyLength > 0) {
    harmonyPlaying = true;
    harmonyIndex = 0;
    harmonyStartTime = millis();
    
    // Start playing the first note
    if (harmonyNotes[0].frequency > 0) {
      tone(harmonyPin, harmonyNotes[0].frequency);
    } else {
      noTone(harmonyPin); // Rest note
    }
  }
}

/**
 * @brief Stop all playback and effects
 * 
 * Stops both melody and harmony, clears lyrics display, starts idle mode,
 * and turns off all LEDs.
 */
void DualBuzzer::stop() {
  stopMelody();
  stopHarmony();
  clearLyrics();
  startIdleMode();
  
  // Turn off LEDs when stopping
  if (ledEnabled) {
    setLEDColor(0, 0, 0, 0, 0);
  }
}

/**
 * @brief Stop melody playback only
 */
void DualBuzzer::stopMelody() {
  melodyPlaying = false;
  noTone(melodyPin);
}

/**
 * @brief Stop harmony playback only
 */
void DualBuzzer::stopHarmony() {
  harmonyPlaying = false;
  noTone(harmonyPin);
}

/**
 * @brief Main update function - call this in your main loop
 * 
 * Handles note timing, advances playback, updates lyrics display,
 * manages idle mode, and updates LED effects. This must be called
 * frequently for proper operation.
 */
void DualBuzzer::update() {
  unsigned long currentTime = millis();
  
  // Update melody playback
  if (melodyPlaying && melodyNotes != NULL) {
    // Read current note from PROGMEM
    Note currentNote;
    memcpy_P(&currentNote, &melodyNotes[melodyIndex], sizeof(Note));
    
    // Check if current note duration has elapsed
    if (currentTime - melodyStartTime >= currentNote.duration) {
      melodyIndex++;
      beatCounter++; // Increment beat counter for LED effects
      
      // Check if we've reached the end of the melody
      if (melodyIndex >= melodyLength) {
        stopMelody();
      } else {
        // Start playing the next note
        melodyStartTime = currentTime;
        
        // Read next note from PROGMEM
        Note nextNote;
        memcpy_P(&nextNote, &melodyNotes[melodyIndex], sizeof(Note));
        
        if (nextNote.frequency > 0) {
          tone(melodyPin, nextNote.frequency);
        } else {
          noTone(melodyPin); // Rest note
        }
        
        // Update lyrics display for the new note
        updateLyrics();
      }
    }
  }
  
  // Update harmony playback (similar to melody)
  if (harmonyPlaying && harmonyNotes != NULL) {
    Note currentNote;
    memcpy_P(&currentNote, &harmonyNotes[harmonyIndex], sizeof(Note));
    
    if (currentTime - harmonyStartTime >= currentNote.duration) {
      harmonyIndex++;
      
      if (harmonyIndex >= harmonyLength) {
        stopHarmony();
      } else {
        harmonyStartTime = currentTime;
        
        Note nextNote;
        memcpy_P(&nextNote, &harmonyNotes[harmonyIndex], sizeof(Note));
        
        if (nextNote.frequency > 0) {
          tone(harmonyPin, nextNote.frequency);
        } else {
          noTone(harmonyPin); // Rest note
        }
      }
    }
  }
  
  // Handle idle mode display when not playing
  if (isIdleMode && !isPlaying()) {
      showIdleLCD();
  }
  
  // Update LED effects at specified intervals
  if (ledEnabled && currentTime - lastLEDUpdate >= ledUpdateInterval) {
    updateLEDs();
    lastLEDUpdate = currentTime;
  }
}

/**
 * @brief Check if any audio is currently playing
 * @return True if melody or harmony is playing, false otherwise
 */
bool DualBuzzer::isPlaying() {
  return melodyPlaying || harmonyPlaying;
}

/**
 * @brief Update lyrics display based on current melody position
 * 
 * Finds the appropriate lyric for the current note and updates
 * the sliding lyrics display.
 */
void DualBuzzer::updateLyrics() {
    if (lcd == NULL || lyrics == NULL || !melodyPlaying) return;
    
    // Find the current lyric based on melody index
    int targetLyricIndex = 0;
    for (int i = 0; i < lyricsCount; i++) {
        if (melodyIndex >= lyrics[i].noteIndex) {
            targetLyricIndex = i;
        } else {
            break;
        }
    }
    
    currentLyricIndex = targetLyricIndex;
    updateSlidingLyrics();
}

/**
 * @brief Update the sliding lyrics display with animation
 * 
 * Creates a scrolling text effect that centers the current word
 * and adds animated dots below it to indicate the active lyric.
 */
void DualBuzzer::updateSlidingLyrics() {
    if (lcd == NULL || lyrics == NULL) return;
    
    // Build the full lyrics text
    String fullText = "";
    for (int i = 0; i < lyricsCount; i++) {
        if (i > 0) fullText += " ";
        fullText += String(lyrics[i].word);
    }
    
    // Find current word position in full text
    int currentWordStart = 0;
    int currentWordEnd = 0;
    int wordIndex = 0;
    
    for (int i = 0; i < lyricsCount && wordIndex <= currentLyricIndex; i++) {
        if (wordIndex == currentLyricIndex) {
            currentWordEnd = currentWordStart + String(lyrics[i].word).length() - 1;
            break;
        }
        currentWordStart += String(lyrics[i].word).length();
        if (i < lyricsCount - 1) currentWordStart++; // Add space
        wordIndex++;
    }
    
    // Calculate display window to center current word
    int displayStart = 0;
    int currentWordLength = currentWordEnd - currentWordStart + 1;
    
    // Center the current word if possible
    if (currentWordStart >= lcdCols / 2) {
        displayStart = currentWordStart - (lcdCols / 2) + (currentWordLength / 2);
    }
    
    // Ensure we don't go past the end
    if (displayStart + lcdCols > fullText.length()) {
        displayStart = max(0, (int)fullText.length() - lcdCols);
    }
    
    // Extract display text and pad with spaces
    String displayText = fullText.substring(displayStart, min(displayStart + lcdCols, (int)fullText.length()));
    while (displayText.length() < lcdCols) {
        displayText += " ";
    }
    
    // Display lyrics on top row
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(displayText);
    
    // Create animated dots on bottom row to highlight current word
    String dotLine = "";
    for (int i = 0; i < lcdCols; i++) {
        dotLine += " ";
    }
    
    // Calculate dot positions for current word
    int wordStartInDisplay = currentWordStart - displayStart;
    int wordEndInDisplay = currentWordEnd - displayStart;
    
    // Only show dots if current word is visible
    if (wordStartInDisplay >= 0 && wordStartInDisplay < lcdCols) {
        int dotCount = min(3, currentWordLength);
        int dotSpacing = max(1, currentWordLength / dotCount);
        
        for (int i = 0; i < dotCount; i++) {
            int dotPos = wordStartInDisplay + (i * dotSpacing);
            if (dotPos >= 0 && dotPos < lcdCols) {
                // Animate dots with different characters for visual effect
                char dotChar = '.';
                int animPhase = (millis() / 200 + i) % 4;
                switch (animPhase) {
                    case 0: dotChar = '.'; break;
                    case 1: dotChar = 'o'; break;
                    case 2: dotChar = 'O'; break;
                    case 3: dotChar = 'o'; break;
                }
                dotLine[dotPos] = dotChar;
            }
        }
    }
    
    // Display animated dots on bottom row
    lcd->setCursor(0, 1);
    lcd->print(dotLine);
}

/**
 * @brief Clear the lyrics display
 */
void DualBuzzer::clearLyrics() {
  if (lcd != NULL) {
    lcd->clear();
  }
}

/**
 * @brief Update LED effects based on current pattern
 * 
 * Calls the appropriate LED pattern function and manages animation timing.
 */
void DualBuzzer::updateLEDs() {
  if (!ledEnabled) return;
  
  switch (currentPattern) {
    case PATTERN_FREQUENCY_BANDS:
      applyFrequencyBands();
      break;
    case PATTERN_BEAT_PULSE:
      applyBeatPulse();
      break;
    case PATTERN_RAINBOW_CHASE:
      applyRainbowChase();
      break;
    case PATTERN_VU_METER:
      applyVUMeter();
      break;
    case PATTERN_DISCO_STROBE:
      applyDiscoStrobe();
      break;
    case PATTERN_SEQUENTIAL_NOTES:
      applySequentialNotes();
      break;
  }
  
  patternStep++;
  if (patternStep > 255) patternStep = 0;
}

/**
 * @brief Set LED colors with PWM brightness control
 * @param red Red LED brightness (0-255)
 * @param green Green LED brightness (0-255)
 * @param blue Blue LED brightness (0-255)
 * @param yellow Yellow LED brightness (0-255)
 * @param white White LED brightness (0-255)
 * 
 * Only sets LEDs that have valid pin assignments (>= 0).
 */
void DualBuzzer::setLEDColor(int red, int green, int blue, int yellow, int white) {
  if (ledConfig.redPin >= 0) analogWrite(ledConfig.redPin, red);
  if (ledConfig.greenPin >= 0) analogWrite(ledConfig.greenPin, green);
  if (ledConfig.bluePin >= 0) analogWrite(ledConfig.bluePin, blue);
  if (ledConfig.yellowPin >= 0) analogWrite(ledConfig.yellowPin, yellow);
  if (ledConfig.whitePin >= 0) analogWrite(ledConfig.whitePin, white);
}

/**
 * @brief Apply frequency-based LED colors with pulsing effect
 * 
 * Maps different frequency ranges to specific LED colors:
 * - Bass (130-260 Hz) -> Red
 * - Mid-low (261-520 Hz) -> Yellow  
 * - Mid (521-1040 Hz) -> Green
 * - High (1041-2080 Hz) -> Blue
 * - Very high (>2080 Hz) -> White
 */
void DualBuzzer::applyFrequencyBands() {
  int melodyFreq = 0, harmonyFreq = 0;
  
  // Get current frequencies from PROGMEM
  if (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) {
    Note currentNote;
    memcpy_P(&currentNote, &melodyNotes[melodyIndex], sizeof(Note));
    melodyFreq = currentNote.frequency;
  }
  
  if (harmonyPlaying && harmonyNotes != NULL && harmonyIndex < harmonyLength) {
    Note currentNote;
    memcpy_P(&currentNote, &harmonyNotes[harmonyIndex], sizeof(Note));
    harmonyFreq = currentNote.frequency;
  }
  
  // Map frequency ranges to colors
  int red = 0, green = 0, blue = 0, yellow = 0, white = 0;
  
  // Bass frequencies (130-260 Hz) -> Red
  if ((melodyFreq >= 130 && melodyFreq <= 260) || (harmonyFreq >= 130 && harmonyFreq <= 260)) {
    red = 255;
  }
  
  // Mid-low frequencies (261-520 Hz) -> Yellow
  if ((melodyFreq >= 261 && melodyFreq <= 520) || (harmonyFreq >= 261 && harmonyFreq <= 520)) {
    yellow = 255;
  }
  
  // Mid frequencies (521-1040 Hz) -> Green
  if ((melodyFreq >= 521 && melodyFreq <= 1040) || (harmonyFreq >= 521 && harmonyFreq <= 1040)) {
    green = 255;
  }
  
  // High frequencies (1041-2080 Hz) -> Blue
  if ((melodyFreq >= 1041 && melodyFreq <= 2080) || (harmonyFreq >= 1041 && harmonyFreq <= 2080)) {
    blue = 255;
  }
  
  // Very high frequencies (>2080 Hz) -> White
  if (melodyFreq > 2080 || harmonyFreq > 2080) {
    white = 255;
  }
  
  // Add pulsing effect using sine wave
  int pulse = (sin(patternStep * 0.1) + 1) * 127;
  red = (red * pulse) / 255;
  green = (green * pulse) / 255;
  blue = (blue * pulse) / 255;
  yellow = (yellow * pulse) / 255;
  white = (white * pulse) / 255;
  
  setLEDColor(red, green, blue, yellow, white);
}

/**
 * @brief Apply beat-synchronized pulsing LED effect
 * 
 * Creates pulsing lights that peak on new beats, with color
 * determined by the current melody frequency.
 */
void DualBuzzer::applyBeatPulse() {
  // Get current melody frequency
  int melodyFreq = (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) ? 
                   melodyNotes[melodyIndex].frequency : 0;
  
  int intensity = mapFrequencyToIntensity(melodyFreq);
  
  // Create a pulsing effect that peaks on new beats
  int pulse = 255 - (patternStep * 8) % 256;
  if (pulse < 0) pulse = 0;
  
  intensity = (intensity * pulse) / 255;
  
  // Distribute intensity across colors based on frequency band
  int band = getFrequencyBand(melodyFreq);
  int red = (band == 0) ? intensity : 0;
  int yellow = (band == 1) ? intensity : 0;
  int green = (band == 2) ? intensity : 0;
  int blue = (band == 3) ? intensity : 0;
  int white = (band == 4) ? intensity : 0;
  
  setLEDColor(red, green, blue, yellow, white);
}

/**
 * @brief Apply rainbow chase LED effect
 * 
 * Cycles through colors in sequence with smooth brightness variation.
 */
void DualBuzzer::applyRainbowChase() {
  // Cycle through colors in sequence
  int step = (patternStep / 20) % 5;
  int brightness = 200 + (sin(patternStep * 0.2) * 55);
  
  int red = (step == 0) ? brightness : 0;
  int yellow = (step == 1) ? brightness : 0;
  int green = (step == 2) ? brightness : 0;
  int blue = (step == 3) ? brightness : 0;
  int white = (step == 4) ? brightness : 0;
  
  setLEDColor(red, green, blue, yellow, white);
}

/**
 * @brief Apply VU meter style LED effect
 * 
 * Lights up more LEDs based on the combined "volume" of melody and harmony.
 * Higher frequency/intensity = more LEDs lit.
 */
void DualBuzzer::applyVUMeter() {
  int melodyFreq = (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) ? 
                   melodyNotes[melodyIndex].frequency : 0;
  int harmonyFreq = (harmonyPlaying && harmonyNotes != NULL && harmonyIndex < harmonyLength) ? 
                    harmonyNotes[harmonyIndex].frequency : 0;
  
  // Calculate "volume" based on both frequencies
  int volume = (mapFrequencyToIntensity(melodyFreq) + mapFrequencyToIntensity(harmonyFreq)) / 2;
  
  // VU meter style - more LEDs light up with higher volume
  int red = (volume > 50) ? 255 : 0;
  int yellow = (volume > 100) ? 255 : 0;
  int green = (volume > 150) ? 255 : 0;
  int blue = (volume > 200) ? 255 : 0;
  int white = (volume > 240) ? 255 : 0;
  
  setLEDColor(red, green, blue, yellow, white);
}

/**
 * @brief Apply disco strobe LED effect
 * 
 * Fast strobing effect with random color selection while music is playing.
 */
void DualBuzzer::applyDiscoStrobe() {
  unsigned long currentTime = millis();
  
  // Fast strobe effect timing
  if (currentTime - lastStrobeTime > 100) {
    strobeState = !strobeState;
    lastStrobeTime = currentTime;
  }
  
  if (strobeState && isPlaying()) {
    // Randomly select colors for strobe
    int randomColor = (patternStep / 10) % 5;
    int brightness = 255;
    
    int red = (randomColor == 0) ? brightness : 0;
    int yellow = (randomColor == 1) ? brightness : 0;
    int green = (randomColor == 2) ? brightness : 0;
    int blue = (randomColor == 3) ? brightness : 0;
    int white = (randomColor == 4) ? brightness : 0;
    
    setLEDColor(red, green, blue, yellow, white);
  } else {
    setLEDColor(0, 0, 0, 0, 0); // All off during strobe off phase
  }
}

/**
 * @brief Get frequency band index for LED mapping
 * @param frequency The frequency to categorize
 * @return Band index (0-4) representing different frequency ranges
 */
int DualBuzzer::getFrequencyBand(int frequency) {
  if (frequency >= 130 && frequency <= 260) return 0;      // Red - Bass
  if (frequency >= 261 && frequency <= 520) return 1;      // Yellow - Mid-low
  if (frequency >= 521 && frequency <= 1040) return 2;     // Green - Mid
  if (frequency >= 1041 && frequency <= 2080) return 3;    // Blue - High
  if (frequency > 2080) return 4;                          // White - Very high
  return 0; // Default to red for silent/low notes
}

/**
 * @brief Applies sequential note lighting effects for dual buzzer melody and harmony playback
 * 
 * This method handles LED visualization during dual buzzer operation by:
 * - Reading current melody and harmony note frequencies from PROGMEM
 * - Detecting note changes and triggering sparkle effects
 * - Randomly selecting LEDs for each note to create visual variety
 * - Preventing back-to-back LED repetition for better visual experience
 */
void DualBuzzer::applySequentialNotes() {
  int melodyFreq = 0, harmonyFreq = 0;
  
  // Get current frequencies from PROGMEM stored note sequences
  if (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) {
    Note currentNote;
    memcpy_P(&currentNote, &melodyNotes[melodyIndex], sizeof(Note));
    melodyFreq = currentNote.frequency;
  }
  
  if (harmonyPlaying && harmonyNotes != NULL && harmonyIndex < harmonyLength) {
    Note currentNote;
    memcpy_P(&currentNote, &harmonyNotes[harmonyIndex], sizeof(Note));
    harmonyFreq = currentNote.frequency;
  }
  
  /**
   * Note change detection system - tracks when frequencies change
   * to trigger special visual effects and prevent LED flicker
   */
  if (melodyFreq != lastMelodyFreq || harmonyFreq != lastHarmonyFreq) {
    lastMelodyFreq = melodyFreq;
    lastHarmonyFreq = harmonyFreq;
    noteChangeTime = millis();
    noteJustChanged = true;
  }
  
  // Use melody frequency as primary, harmony as secondary fallback
  int primaryFreq = (melodyFreq > 0) ? melodyFreq : harmonyFreq;
  
  // Clear all LED states before applying new lighting
  setLEDColor(0, 0, 0, 0, 0);
  
  if (primaryFreq > 0) {
    /**
     * Random LED selection with anti-repetition logic
     * Ensures visual variety while preventing monotonous back-to-back same LED
     */
    static int lastLED = -1;
    int currentLED;
    
    do {
      currentLED = random(0, 5); // 0=Red, 1=Green, 2=Blue, 3=Yellow, 4=White
    } while (currentLED == lastLED && noteJustChanged);
    
    lastLED = currentLED;
    
    // Map LED index to color channels (RGB + Yellow + White)
    int red = (currentLED == 0) ? 255 : 0;
    int green = (currentLED == 1) ? 255 : 0;
    int blue = (currentLED == 2) ? 255 : 0;
    int yellow = (currentLED == 3) ? 255 : 0;
    int white = (currentLED == 4) ? 255 : 0;
    
    setLEDColor(red, green, blue, yellow, white);
    
    /**
     * Sparkle effect on note transitions
     * Creates a brief 100ms flash of all LEDs when notes change
     */
    if (noteJustChanged && millis() - noteChangeTime < 100) {
      // Brief flash of all LEDs when note changes
      setLEDColor(100, 100, 100, 100, 100);
    }
  }
  
  // Reset note change flag after sparkle duration expires
  if (millis() - noteChangeTime > 100) {
    noteJustChanged = false;
  }
}

/**
 * @brief Plays a musical sequence with synchronized LED visualization
 * 
 * @param sequence Pointer to PROGMEM array of Note structures
 * @param length Number of notes in the sequence
 * @param buzzerPin Arduino pin number for the buzzer output
 * 
 * This method provides blocking playback of musical sequences with:
 * - Frequency-mapped LED lighting effects
 * - Proper timing for note duration and inter-note pauses
 * - Support for rest notes (frequency = 0)
 */
void DualBuzzer::playSequenceWithLEDs(const Note* sequence, int length, int buzzerPin) {
  for (int i = 0; i < length; i++) {
    Note currentNote;
    memcpy_P(&currentNote, &sequence[i], sizeof(Note)); // Read from PROGMEM
    
    // Clear all LEDs before each note
    setLEDColor(0, 0, 0, 0, 0);
    
    if (currentNote.frequency > 0) {
      // Generate audio tone
      tone(buzzerPin, currentNote.frequency);
      
      // Map note frequency to appropriate LED color/pattern
      lightLEDForNote(currentNote.frequency);
      
      // Hold note for specified duration
      delay(currentNote.duration);
      
      // Stop audio output
      noTone(buzzerPin);
    } else {
      // Handle rest notes - silence with all LEDs off
      setLEDColor(0, 0, 0, 0, 0);
      delay(currentNote.duration);
    }
    
    // Brief inter-note pause for musical clarity
    delay(50);
    setLEDColor(0, 0, 0, 0, 0);
  }
}

/**
 * @brief Maps musical note frequencies to specific LED colors and patterns
 * 
 * @param frequency The frequency in Hz of the note to visualize
 * 
 * This function implements a comprehensive frequency-to-color mapping system:
 * - Natural notes (C,D,E,F,G,A,B) map to primary colors
 * - Sharp/flat notes use blended colors
 * - Octave variations (4th and 5th octaves) use same color schemes
 * - Very high frequencies (>1000Hz) cycle through rainbow colors
 * - Very low frequencies (<260Hz) use dim red pulsing effect
 */
void DualBuzzer::lightLEDForNote(int frequency) {
  if (!ledEnabled) return;
  
  // Map specific frequencies to LEDs with full brightness
  // C notes -> Red LED (Do)
  if ((frequency >= 260 && frequency <= 267) ||   // C4
      (frequency >= 520 && frequency <= 530)) {   // C5
    setLEDColor(255, 0, 0, 0, 0);
  }
  // D notes -> Yellow LED (Re)
  else if ((frequency >= 290 && frequency <= 300) ||  // D4
           (frequency >= 580 && frequency <= 595)) {  // D5
    setLEDColor(0, 0, 0, 255, 0);
  }
  // E notes -> Green LED (Mi)
  else if ((frequency >= 325 && frequency <= 335) ||  // E4
           (frequency >= 650 && frequency <= 670)) {  // E5
    setLEDColor(0, 255, 0, 0, 0);
  }
  // F notes -> Blue LED (Fa)
  else if ((frequency >= 345 && frequency <= 355) ||  // F4
           (frequency >= 690 && frequency <= 710)) {  // F5
    setLEDColor(0, 0, 255, 0, 0);
  }
  // G notes -> White LED (Sol)
  else if ((frequency >= 387 && frequency <= 400) ||  // G4
           (frequency >= 775 && frequency <= 795)) {  // G5
    setLEDColor(0, 0, 0, 0, 255);
  }
  // A notes -> Red + Yellow = Orange (La)
  else if ((frequency >= 435 && frequency <= 450) ||  // A4
           (frequency >= 870 && frequency <= 890)) {  // A5
    setLEDColor(255, 0, 0, 255, 0);
  }
  // B notes -> Blue + Green = Cyan (Ti)  
  else if ((frequency >= 490 && frequency <= 500) ||  // B4
           (frequency >= 980 && frequency <= 1000)) { // B5
    setLEDColor(0, 255, 255, 0, 0);
  }
  /**
   * Sharp/Flat note handling with blended colors
   * Creates intermediate colors for chromatic notes
   */
  else if (frequency >= 277 && frequency <= 285) {    // C#/Db
    setLEDColor(255, 0, 0, 127, 0);  // Red + half Yellow
  }
  else if (frequency >= 311 && frequency <= 320) {    // D#/Eb
    setLEDColor(0, 0, 0, 255, 127);  // Yellow + half White
  }
  else if (frequency >= 370 && frequency <= 380) {    // F#/Gb
    setLEDColor(0, 0, 255, 0, 127);  // Blue + half White
  }
  else if (frequency >= 415 && frequency <= 425) {    // G#/Ab
    setLEDColor(127, 0, 0, 0, 255);  // Half Red + White
  }
  /**
   * Very high frequency rainbow cycling effect
   * Creates dynamic color changes for extreme high notes
   */
  else if (frequency > 1000) {
    // Cycle through colors based on frequency modulo
    int colorIndex = (frequency / 100) % 5;
    switch(colorIndex) {
      case 0: setLEDColor(255, 0, 0, 0, 0); break;    // Red
      case 1: setLEDColor(0, 0, 0, 255, 0); break;    // Yellow
      case 2: setLEDColor(0, 255, 0, 0, 0); break;    // Green
      case 3: setLEDColor(0, 0, 255, 0, 0); break;    // Blue
      case 4: setLEDColor(0, 0, 0, 0, 255); break;    // White
    }
  }
  // Very low notes -> Pulsing dim red for sub-bass frequencies
  else if (frequency < 260 && frequency > 0) {
    setLEDColor(200, 0, 0, 0, 0);  // Dim red
  }
  // Default fallback for unrecognized frequencies
  else {
    setLEDColor(0, 0, 0, 0, 100);  // Dim white
  }
}

/**
 * @brief Maps frequency values to LED intensity levels
 * 
 * @param frequency Input frequency in Hz (0 for silence)
 * @return int Mapped intensity value (0-255)
 * 
 * Provides linear mapping from frequency range to LED brightness:
 * - 130Hz maps to minimum intensity (100)
 * - 2093Hz maps to maximum intensity (255)
 * - 0Hz (silence) maps to 0 intensity
 */
int DualBuzzer::mapFrequencyToIntensity(int frequency) {
  if (frequency == 0) return 0;
  
  // Linear mapping: higher frequencies = higher LED intensity
  int intensity = map(frequency, 130, 2093, 100, 255);
  return constrain(intensity, 0, 255);
}

/**
 * @brief Displays animated idle screen when no music is playing
 * 
 * Creates an engaging idle animation with:
 * - Scrolling text message on top LCD line
 * - Wave animation pattern on bottom LCD line
 * - Musical note symbols integrated into wave pattern
 * - Controlled update timing (300ms intervals) for smooth animation
 * - Overflow protection for animation counters
 */
void DualBuzzer::showIdleLCD() {
  unsigned long currentTime = millis();
  
  // Throttle animation updates to 300ms intervals for smooth motion
  if (currentTime - lastIdleUpdate < 300) { 
    return;
  }
  
  lastIdleUpdate = currentTime;
  isIdleMode = true;
  
  /**
   * Scrolling message system
   * Creates horizontal scrolling text with padding for smooth wrap-around
   */
  String message = "     Please select a new song to play!     ";
  int messageLength = message.length();
  
  // Calculate scroll position with reduced speed (divide by 2)
  int scrollPos = (idleAnimationStep / 2) % (messageLength + lcdCols);
  
  // Build display line by extracting characters at scroll position
  String displayLine = "";
  for (int i = 0; i < lcdCols; i++) {
    int charIndex = (scrollPos + i) % messageLength;
    displayLine += message.charAt(charIndex);
  }
  
  // Display scrolling message on top line
  lcd->setCursor(0, 0);
  lcd->print(displayLine);
  
  /**
   * Wave animation generator for bottom LCD line
   * Creates flowing wave pattern using ASCII characters
   * with integrated musical note symbols
   */
  String bottomLine = "";
  for (int i = 0; i < lcdCols; i++) {
    // Phase calculation for wave effect across screen width
    int animPhase = ((idleAnimationStep / 3) + i * 3) % 16;
    char animChar;
    
    // Wave transition sequence using ASCII characters
    switch (animPhase) {
      case 0: case 8: animChar = ' '; break;  // Empty space
      case 1: case 9: animChar = '.'; break;  // Small dot
      case 2: case 10: animChar = '-'; break; // Dash
      case 3: case 11: animChar = '='; break; // Double dash
      case 4: case 12: animChar = '#'; break; // Peak
      case 5: case 13: animChar = '='; break; // Double dash
      case 6: case 14: animChar = '-'; break; // Dash
      case 7: case 15: animChar = '.'; break; // Small dot
      default: animChar = ' '; break;
    }
    
    // Inject musical note symbols at regular intervals during peaks
    if (i % 6 == 0 && (animPhase == 4 || animPhase == 12)) {
      animChar = '*'; // Musical note representation
    }
    
    bottomLine += animChar;
  }
  
  // Display wave animation on bottom line
  lcd->setCursor(0, 1);
  lcd->print(bottomLine);
  
  idleAnimationStep++;
  
  // Prevent integer overflow by resetting counter periodically
  if (idleAnimationStep > 2000) {
    idleAnimationStep = 0;
  }
}

/**
 * @brief Stops the idle mode animation and clears idle state
 * 
 * Used when transitioning from idle to active music playback
 */
void DualBuzzer::stopIdleMode() {
  isIdleMode = false;
}

/**
 * @brief Initializes and starts the idle mode animation
 * 
 * Prepares the system for idle state by:
 * - Turning off all LEDs
 * - Setting idle mode flag
 * - Resetting animation counters
 * - Triggering initial LCD display update
 */
 
void DualBuzzer::startIdleMode() {
  setLEDColor(0,0,0,0,0);  // Clear all LED outputs
  isIdleMode = true;
  idleAnimationStep = 0;   // Reset animation counter
  lastIdleUpdate = 0;      // Force immediate update
  showIdleLCD();           // Display initial animation frame
}