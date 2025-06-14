#include "DualBuzzer.h"

DualBuzzer::DualBuzzer(int melodyBuzzerPin, int harmonyBuzzerPin) {
  melodyPin = melodyBuzzerPin;
  harmonyPin = harmonyBuzzerPin;
  
  // Initialize pins
  pinMode(melodyPin, OUTPUT);
  pinMode(harmonyPin, OUTPUT);
  
  // Initialize variables
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
  
  // Initialize Lyrics NULL
  lyrics = NULL;
  lyricsCount = 0;
  currentLyricIndex = 0;
  
  // Initialize I2C LCD to NULL
  lcd = NULL;
  lcdRows = 0;
  lcdCols = 0;
  
  // Initialize LED system
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

  // IDLE Mode
  lastIdleUpdate = 0;
  idleAnimationStep = 0;
  isIdleMode = false;
  
  // Initialize LED config to invalid pins
  ledConfig.redPin = -1;
  ledConfig.bluePin = -1;
  ledConfig.greenPin = -1;
  ledConfig.yellowPin = -1;
  ledConfig.whitePin = -1;
}

void DualBuzzer::setupLEDs(int redPin, int bluePin, int greenPin, int yellowPin, int whitePin) {
  ledConfig.redPin = redPin;
  ledConfig.bluePin = bluePin;
  ledConfig.greenPin = greenPin;
  ledConfig.yellowPin = yellowPin;
  ledConfig.whitePin = whitePin;
  
  // Initialize LED pins
  if (redPin >= 0) pinMode(redPin, OUTPUT);
  if (bluePin >= 0) pinMode(bluePin, OUTPUT);
  if (greenPin >= 0) pinMode(greenPin, OUTPUT);
  if (yellowPin >= 0) pinMode(yellowPin, OUTPUT);
  if (whitePin >= 0) pinMode(whitePin, OUTPUT);
  
  // Turn off all LEDs initially
  setLEDColor(0, 0, 0, 0, 0);
  
  ledEnabled = true;
}

void DualBuzzer::setLEDPattern(LEDPattern pattern) {
  currentPattern = pattern;
  patternStep = 0;
  beatCounter = 0;
}

void DualBuzzer::enableLEDs(bool enable) {
  ledEnabled = enable;
  if (!enable) {
    setLEDColor(0, 0, 0, 0, 0);
  }
}

void DualBuzzer::setMelody(Note* notes, int length) {
  melodyNotes = notes;
  melodyLength = length;
  melodyIndex = 0;
}

void DualBuzzer::setHarmony(Note* notes, int length) {
  harmonyNotes = notes;
  harmonyLength = length;
  harmonyIndex = 0;
}

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

void DualBuzzer::setLyrics(LyricTiming* timings, int count) {
  lyrics = timings;
  lyricsCount = count;
  currentLyricIndex = 0;
}


void DualBuzzer::setLCD(LiquidCrystal_I2C* display, int rows, int columns) {
  lcd = display;
  lcdRows = rows;
  lcdCols = columns;
}

void DualBuzzer::play() {
  playMelody();
  playHarmony();
  
  // Reset lyrics and display first lyric if available
  currentLyricIndex = 0;
  clearLyrics();
  updateLyrics();
}

void DualBuzzer::playMelody() {
  if (melodyNotes != NULL && melodyLength > 0) {
    melodyPlaying = true;
    melodyIndex = 0;
    melodyStartTime = millis();
    
    // Start playing the first note
    if (melodyNotes[0].frequency > 0) {
      tone(melodyPin, melodyNotes[0].frequency);
      
    } else {
      noTone(melodyPin);
    }
  }
}

void DualBuzzer::playHarmony() {
  if (harmonyNotes != NULL && harmonyLength > 0) {
    harmonyPlaying = true;
    harmonyIndex = 0;
    harmonyStartTime = millis();
    
    // Start playing the first note
    if (harmonyNotes[0].frequency > 0) {
      tone(harmonyPin, harmonyNotes[0].frequency);
    } else {
      noTone(harmonyPin);
    }
  }
}

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

void DualBuzzer::stopMelody() {
  melodyPlaying = false;
  noTone(melodyPin);
}

void DualBuzzer::stopHarmony() {
  harmonyPlaying = false;
  noTone(harmonyPin);
}

void DualBuzzer::update() {
  unsigned long currentTime = millis();
  
  // Update melody
  if (melodyPlaying && melodyNotes != NULL) {
    // Read note from PROGMEM
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
          noTone(melodyPin);
        }
        
        // Update lyrics display for the new note
        updateLyrics();
      }
    }

  }
  
  // Update harmony (similar fix)
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
          noTone(harmonyPin);
        }
      }
    }
  }
  // Check if Idle
  if (isIdleMode && !isPlaying()) {
      showIdleLCD();
  }
  
  // Update LEDs
  if (ledEnabled && currentTime - lastLEDUpdate >= ledUpdateInterval) {
    updateLEDs();
    lastLEDUpdate = currentTime;
  }
}


bool DualBuzzer::isPlaying() {
  return melodyPlaying || harmonyPlaying;
}

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
    
    // Calculate display window
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
    
    // Extract display text
    String displayText = fullText.substring(displayStart, min(displayStart + lcdCols, (int)fullText.length()));
    
    // Pad with spaces if needed
    while (displayText.length() < lcdCols) {
        displayText += " ";
    }
    
    // Display lyrics on top row
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(displayText);
    
    // Create animated dots on bottom row
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
                // Animate dots with different characters
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
    
    // Display dots on bottom row
    lcd->setCursor(0, 1);
    lcd->print(dotLine);
}


void DualBuzzer::clearLyrics() {
  if (lcd != NULL) {
    lcd->clear();
  }
}

// LED Control Functions
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

void DualBuzzer::setLEDColor(int red, int green, int blue, int yellow, int white) {
  if (ledConfig.redPin >= 0) analogWrite(ledConfig.redPin, red);
  if (ledConfig.greenPin >= 0) analogWrite(ledConfig.greenPin, green);
  if (ledConfig.bluePin >= 0) analogWrite(ledConfig.bluePin, blue);
  if (ledConfig.yellowPin >= 0) analogWrite(ledConfig.yellowPin, yellow);
  if (ledConfig.whitePin >= 0) analogWrite(ledConfig.whitePin, white);
}

void DualBuzzer::applyFrequencyBands() {
  int melodyFreq = 0, harmonyFreq = 0;
  
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
  
  // Add some pulsing effect
  int pulse = (sin(patternStep * 0.1) + 1) * 127;
  red = (red * pulse) / 255;
  green = (green * pulse) / 255;
  blue = (blue * pulse) / 255;
  yellow = (yellow * pulse) / 255;
  white = (white * pulse) / 255;
  
  setLEDColor(red, green, blue, yellow, white);
}

void DualBuzzer::applyBeatPulse() {
  // Pulse intensity based on beat and frequency
  int melodyFreq = (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) ? 
                   melodyNotes[melodyIndex].frequency : 0;
  
  int intensity = mapFrequencyToIntensity(melodyFreq);
  
  // Create a pulsing effect that peaks on new beats
  int pulse = 255 - (patternStep * 8) % 256;
  if (pulse < 0) pulse = 0;
  
  intensity = (intensity * pulse) / 255;
  
  // Distribute intensity across colors based on frequency
  int band = getFrequencyBand(melodyFreq);
  int red = (band == 0) ? intensity : 0;
  int yellow = (band == 1) ? intensity : 0;
  int green = (band == 2) ? intensity : 0;
  int blue = (band == 3) ? intensity : 0;
  int white = (band == 4) ? intensity : 0;
  
  setLEDColor(red, green, blue, yellow, white);
}

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

void DualBuzzer::applyDiscoStrobe() {
  unsigned long currentTime = millis();
  
  // Fast strobe effect
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
    setLEDColor(0, 0, 0, 0, 0);
  }
}

int DualBuzzer::getFrequencyBand(int frequency) {
  if (frequency >= 130 && frequency <= 260) return 0;      // Red - Bass
  if (frequency >= 261 && frequency <= 520) return 1;      // Yellow - Mid-low
  if (frequency >= 521 && frequency <= 1040) return 2;     // Green - Mid
  if (frequency >= 1041 && frequency <= 2080) return 3;    // Blue - High
  if (frequency > 2080) return 4;                          // White - Very high
  return 0; // Default to red for silent/low notes
}

void DualBuzzer::applySequentialNotes() {
  int melodyFreq = 0, harmonyFreq = 0;
  
  // Get current frequencies
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
  
  // Check if note changed
  if (melodyFreq != lastMelodyFreq || harmonyFreq != lastHarmonyFreq) {
    lastMelodyFreq = melodyFreq;
    lastHarmonyFreq = harmonyFreq;
    noteChangeTime = millis();
    noteJustChanged = true;
  }
  
  // Use melody frequency as primary, harmony as secondary
  int primaryFreq = (melodyFreq > 0) ? melodyFreq : harmonyFreq;
  
  // Turn off all LEDs first
  setLEDColor(0, 0, 0, 0, 0);
  
  if (primaryFreq > 0) {
    // Generate a random LED for each note, ensuring no back-to-back repeats
    static int lastLED = -1;
    int currentLED;
    
    do {
      currentLED = random(0, 5); // 0=Red, 1=Green, 2=Blue, 3=Yellow, 4=White
    } while (currentLED == lastLED && noteJustChanged);
    
    lastLED = currentLED;
    
    // Light up the selected LED
    int red = (currentLED == 0) ? 255 : 0;
    int green = (currentLED == 1) ? 255 : 0;
    int blue = (currentLED == 2) ? 255 : 0;
    int yellow = (currentLED == 3) ? 255 : 0;
    int white = (currentLED == 4) ? 255 : 0;
    
    setLEDColor(red, green, blue, yellow, white);
    
    // Add sparkle effect on note changes
    if (noteJustChanged && millis() - noteChangeTime < 100) {
      // Brief flash of all LEDs when note changes
      setLEDColor(100, 100, 100, 100, 100);
    }
  }
  
  // Reset note change flag after sparkle duration
  if (millis() - noteChangeTime > 100) {
    noteJustChanged = false;
  }
}

void DualBuzzer::playSequenceWithLEDs(const Note* sequence, int length, int buzzerPin) {
  for (int i = 0; i < length; i++) {
    Note currentNote;
    memcpy_P(&currentNote, &sequence[i], sizeof(Note));
    
    // Turn off all LEDs first
    setLEDColor(0, 0, 0, 0, 0);
    
    if (currentNote.frequency > 0) {
      // Start the buzzer
      tone(buzzerPin, currentNote.frequency);
      
      // Light up LED based on the note frequency
      lightLEDForNote(currentNote.frequency);
      
      // Wait for the note duration
      delay(currentNote.duration);
      
      // Stop the buzzer
      noTone(buzzerPin);
    } else {
      // Rest note - all LEDs off
      setLEDColor(0, 0, 0, 0, 0);
      delay(currentNote.duration);
    }
    
    // Brief pause between notes for clarity
    delay(50);
    setLEDColor(0, 0, 0, 0, 0);
  }
}

// Helper function to light specific LED based on note frequency
void DualBuzzer::lightLEDForNote(int frequency) {
  if (!ledEnabled) return;
  
  // Map specific frequencies to LEDs with full brightness
  // C notes -> Red LED
  if ((frequency >= 260 && frequency <= 267) ||   // C4
      (frequency >= 520 && frequency <= 530)) {   // C5
    setLEDColor(255, 0, 0, 0, 0);
  }
  // D notes -> Yellow LED
  else if ((frequency >= 290 && frequency <= 300) ||  // D4
           (frequency >= 580 && frequency <= 595)) {  // D5
    setLEDColor(0, 0, 0, 255, 0);
  }
  // E notes -> Green LED
  else if ((frequency >= 325 && frequency <= 335) ||  // E4
           (frequency >= 650 && frequency <= 670)) {  // E5
    setLEDColor(0, 255, 0, 0, 0);
  }
  // F notes -> Blue LED
  else if ((frequency >= 345 && frequency <= 355) ||  // F4
           (frequency >= 690 && frequency <= 710)) {  // F5
    setLEDColor(0, 0, 255, 0, 0);
  }
  // G notes -> White LED
  else if ((frequency >= 387 && frequency <= 400) ||  // G4
           (frequency >= 775 && frequency <= 795)) {  // G5
    setLEDColor(0, 0, 0, 0, 255);
  }
  // A notes -> Red + Yellow (Orange)
  else if ((frequency >= 435 && frequency <= 450) ||  // A4
           (frequency >= 870 && frequency <= 890)) {  // A5
    setLEDColor(255, 0, 0, 255, 0);
  }
  // B notes -> Blue + Green (Cyan)
  else if ((frequency >= 490 && frequency <= 500) ||  // B4
           (frequency >= 980 && frequency <= 1000)) { // B5
    setLEDColor(0, 255, 255, 0, 0);
  }
  // Sharp/Flat notes -> Combined colors
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
  // Very high notes -> Rainbow cycle effect
  else if (frequency > 1000) {
    // Cycle through colors based on frequency
    int colorIndex = (frequency / 100) % 5;
    switch(colorIndex) {
      case 0: setLEDColor(255, 0, 0, 0, 0); break;    // Red
      case 1: setLEDColor(0, 0, 0, 255, 0); break;    // Yellow
      case 2: setLEDColor(0, 255, 0, 0, 0); break;    // Green
      case 3: setLEDColor(0, 0, 255, 0, 0); break;    // Blue
      case 4: setLEDColor(0, 0, 0, 0, 255); break;    // White
    }
  }
  // Very low notes -> Pulsing Red
  else if (frequency < 260 && frequency > 0) {
    setLEDColor(200, 0, 0, 0, 0);  // Dim red
  }
  // Default for unrecognized frequencies -> Dim white
  else {
    setLEDColor(0, 0, 0, 0, 100);
  }
}

int DualBuzzer::mapFrequencyToIntensity(int frequency) {
  if (frequency == 0) return 0;
  
  // Map frequency to intensity (higher frequencies = higher intensity)
  int intensity = map(frequency, 130, 2093, 100, 255);
  return constrain(intensity, 0, 255);
}

void DualBuzzer::showIdleLCD() {
  unsigned long currentTime = millis();
  
  // Only update animation at specified intervals
  if (currentTime - lastIdleUpdate < 300) { 
    return;
  }
  
  lastIdleUpdate = currentTime;
  isIdleMode = true;
  
  // Create scrolling "Please select a song" message
  String message = "     Please select a new song to play!     ";
  int messageLength = message.length();
  
  // Calculate scroll position with slower movement
  int scrollPos = (idleAnimationStep / 2) % (messageLength + lcdCols); // Divide by 2 for slower scroll
  
  // Create the display line
  String displayLine = "";
  for (int i = 0; i < lcdCols; i++) {
    int charIndex = (scrollPos + i) % messageLength;
    displayLine += message.charAt(charIndex);
  }
  
  // Top line: Scrolling message
  lcd->setCursor(0, 0);
  lcd->print(displayLine);
  
  String bottomLine = "";
  for (int i = 0; i < lcdCols; i++) {
    // Create a wave effect
    int animPhase = ((idleAnimationStep / 3) + i * 3) % 16; //animation cycle
    char animChar;
    
    // Transition between characters
    switch (animPhase) {
      case 0: case 8: animChar = ' '; break;
      case 1: case 9: animChar = '.'; break;
      case 2: case 10: animChar = '-'; break;
      case 3: case 11: animChar = '='; break;
      case 4: case 12: animChar = '#'; break;
      case 5: case 13: animChar = '='; break;
      case 6: case 14: animChar = '-'; break;
      case 7: case 15: animChar = '.'; break;
      default: animChar = ' '; break;
    }
    
    // Add musical symbols occasionally
    if (i % 6 == 0 && (animPhase == 4 || animPhase == 12)) {
      animChar = '*'; // 'Musical note'
    }
    
    bottomLine += animChar;
  }
  
  lcd->setCursor(0, 1);
  lcd->print(bottomLine);
  
  idleAnimationStep++;
  
  // Reset step counter to prevent overflow
  if (idleAnimationStep > 2000) {
    idleAnimationStep = 0;
  }
}

// Helper function to stop idle mode
void DualBuzzer::stopIdleMode() {
  isIdleMode = false;
}

// Helper function to start idle mode
void DualBuzzer::startIdleMode() {
  setLEDColor(0,0,0,0,0);
  isIdleMode = true;
  idleAnimationStep = 0;
  lastIdleUpdate = 0;
  showIdleLCD();
}
