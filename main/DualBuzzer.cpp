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
  
  // Initialize lyrics variables
  lyricsString = "";
  wordsArray = NULL;
  wordCount = 0;
  currentWordIndex = 0;
  
  // Initialize I2C LCD pointer to NULL
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
  currentWordIndex = 0;
  
  // Reset LED effects
  patternStep = 0;
  beatCounter = 0;
}

void DualBuzzer::setLyrics(String lyrics) {
  lyricsString = lyrics;
  splitLyrics();
  currentWordIndex = 0;
}

void DualBuzzer::splitLyrics() {
  // Clean up previous words array
  if (wordsArray != NULL) {
    delete[] wordsArray;
    wordsArray = NULL;
  }
  
  // Count words (including empty strings for pauses)
  wordCount = 0;
  String tempString = lyricsString;
  
  // Split by spaces and count
  int startIndex = 0;
  int spaceIndex = tempString.indexOf(' ');
  
  while (spaceIndex != -1) {
    wordCount++;
    startIndex = spaceIndex + 1;
    spaceIndex = tempString.indexOf(' ', startIndex);
  }
  
  // Add the last word if there's any remaining text
  if (startIndex < tempString.length()) {
    wordCount++;
  }
  
  // Create words array
  if (wordCount > 0) {
    wordsArray = new String[wordCount];
    
    // Split the string into words
    int wordIndex = 0;
    startIndex = 0;
    spaceIndex = lyricsString.indexOf(' ');
    
    while (spaceIndex != -1 && wordIndex < wordCount) {
      String word = lyricsString.substring(startIndex, spaceIndex);
      wordsArray[wordIndex] = word;
      wordIndex++;
      startIndex = spaceIndex + 1;
      spaceIndex = lyricsString.indexOf(' ', startIndex);
    }
    
    // Add the last word
    if (startIndex < lyricsString.length() && wordIndex < wordCount) {
      wordsArray[wordIndex] = lyricsString.substring(startIndex);
    }
  }
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
  currentWordIndex = 0;
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
    // Check if current note duration has elapsed
    if (currentTime - melodyStartTime >= melodyNotes[melodyIndex].duration) {
      melodyIndex++;
      beatCounter++; // Increment beat counter for LED effects
      
      // Advance to next word for each note
      if (currentWordIndex < wordCount) {
        currentWordIndex++;
      }
      
      // Check if we've reached the end of the melody
      if (melodyIndex >= melodyLength) {
        stopMelody();
      } else {
        // Start playing the next note
        melodyStartTime = currentTime;
        
        if (melodyNotes[melodyIndex].frequency > 0) {
          tone(melodyPin, melodyNotes[melodyIndex].frequency);
        } else {
          noTone(melodyPin);
        }
        
        // Update lyrics display for the new note
        updateLyrics();
      }
    }
  }
  
  // Update harmony
  if (harmonyPlaying && harmonyNotes != NULL) {
    // Check if current note duration has elapsed
    if (currentTime - harmonyStartTime >= harmonyNotes[harmonyIndex].duration) {
      harmonyIndex++;
      
      // Check if we've reached the end of the harmony
      if (harmonyIndex >= harmonyLength) {
        stopHarmony();
      } else {
        // Start playing the next note
        harmonyStartTime = currentTime;
        
        if (harmonyNotes[harmonyIndex].frequency > 0) {
          tone(harmonyPin, harmonyNotes[harmonyIndex].frequency);
        } else {
          noTone(harmonyPin);
        }
      }
    }
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
  if (lcd != NULL && wordsArray != NULL && wordCount > 0) {
    // Check if we need to update the current word (advance on each note)
    if (currentWordIndex < wordCount && melodyIndex < melodyLength) {
      // Update the sliding window display
      updateSlidingLyrics();
    }
  }
}

void DualBuzzer::updateSlidingLyrics() {
  if (lcd == NULL || wordsArray == NULL) return;
  
  // Clear LCD
  lcd->clear();
  
  // Top line with karaoke header
  if (lcdRows > 1) {
    lcd->setCursor(0, 0);
    String header = "♪ DISCO MODE ♪";
    
    // Center the header
    int headerStart = max(0, (lcdCols - header.length()) / 2);
    lcd->setCursor(headerStart, 0);
    lcd->print(header.substring(0, min((int)header.length(), lcdCols)));
  }
  
  // Build sliding window of lyrics
  String slidingText = "";
  
  // Start with the current word (highlighted)
  if (currentWordIndex < wordCount) {
    String currentWord = wordsArray[currentWordIndex];
    
    // Handle pauses (empty words or specific pause markers)
    if (currentWord.length() == 0 || currentWord == "~" || currentWord == "-") {
      // For pauses, show the previous word if available
      if (currentWordIndex > 0) {
        slidingText = wordsArray[currentWordIndex - 1];
        slidingText.toUpperCase(); // Keep it highlighted during pause
      }
    } else {
      slidingText = currentWord;
      slidingText.toUpperCase(); // Highlight current word
    }
  }
  
  // Add upcoming words that fit in the display
  int nextIndex = currentWordIndex + 1;
  while (nextIndex < wordCount && slidingText.length() < lcdCols) {
    String nextWord = wordsArray[nextIndex];
    
    // Skip empty words (pauses) when building preview
    if (nextWord.length() == 0 || nextWord == "~" || nextWord == "-") {
      nextIndex++;
      continue;
    }
    
    // Check if adding this word (with space) would exceed display width
    String testText = slidingText;
    if (testText.length() > 0) {
      testText += " ";
    }
    testText += nextWord;
    
    // Only add if the complete word fits
    if (testText.length() <= lcdCols) {
      slidingText = testText;
      nextIndex++;
    } else {
      break; // Stop adding words if this one doesn't fit
    }
  }
  
  // Display the sliding window on the lyrics row
  int lyricRow = (lcdRows > 1) ? 1 : 0;
  lcd->setCursor(0, lyricRow);
  lcd->print(slidingText.substring(0, min((int)slidingText.length(), lcdCols)));
}

// String DualBuzzer::highlightCurrentWord(String text, int currentIndex) {
//   if (currentIndex < 0 || currentIndex >= lyricsLength) {
//     return text;
//   }
  
//   String currentWord = String(lyrics[currentIndex].text);
//   if (currentWord.length() == 0) {
//     return text;
//   }
  
//   // Find the current word in the text and make it uppercase
//   int wordStart = text.indexOf(currentWord);
//   if (wordStart >= 0) {
//     String before = text.substring(0, wordStart);
//     String highlighted = currentWord;
//     highlighted.toUpperCase();
//     String after = text.substring(wordStart + currentWord.length());
//     return before + highlighted + after;
//   }
  
//   return text;
// }

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
  int melodyFreq = (melodyPlaying && melodyNotes != NULL && melodyIndex < melodyLength) ? 
                   melodyNotes[melodyIndex].frequency : 0;
  int harmonyFreq = (harmonyPlaying && harmonyNotes != NULL && harmonyIndex < harmonyLength) ? 
                    harmonyNotes[harmonyIndex].frequency : 0;
  
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

int DualBuzzer::mapFrequencyToIntensity(int frequency) {
  if (frequency == 0) return 0;
  
  // Map frequency to intensity (higher frequencies = higher intensity)
  int intensity = map(frequency, 130, 2093, 100, 255);
  return constrain(intensity, 0, 255);
}