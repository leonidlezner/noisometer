#include <Adafruit_NeoPixel.h>

#include "noisometer.h"

//#define DISPLAY_NEOPIXEL

#ifdef DISPLAY_NEOPIXEL
#define PIN            7
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#endif

NOISOMETER_STATE gState;

/**
 * Function for measuring the sound value. The return value is the peak to peak measurement of the ADC input.
 *
 * Source: https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 */
unsigned int measureSound(unsigned char pinAudioIn, unsigned int sampleWindow) {
  unsigned int soundValue = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int peakToPeak = 0;

  unsigned long startMillis = millis();

  while (millis() - startMillis < sampleWindow)
  {
    soundValue = analogRead(pinAudioIn);

    if (soundValue < 1024) {
      if (soundValue > signalMax) {
        signalMax = soundValue;
      } else if (soundValue < signalMin) {
        signalMin = soundValue;
      }

      peakToPeak = signalMax - signalMin;
    }
  }

  return peakToPeak;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

#ifdef DISPLAY_NEOPIXEL
  pixels.begin();
#endif
  
  gState.level = NM_LEVEL_GOOD;
  gState.lastLevel = NM_LEVEL_CRITICAL;

  gState.thresholds.good_notice = 15;
  gState.thresholds.good_notice_time = TIME_S(3);
  gState.thresholds.good_warning_time = TIME_S(2);

  gState.thresholds.notice_good = 10;
  gState.thresholds.notice_good_time = TIME_S(5);

  gState.thresholds.notice_warning = 30;
  gState.thresholds.notice_warning_time = TIME_S(4);

  gState.thresholds.warning_notice = 20;
  gState.thresholds.warning_notice_time = TIME_S(8);
  
  gState.thresholds.warning_critical = 40;
  gState.thresholds.warning_critical_time = TIME_S(2);
  
  gState.thresholds.critical_warning = 30;
  gState.thresholds.critical_warning_time = TIME_S(5);
}


#ifdef DISPLAY_NEOPIXEL

void displayState(NOISOMETER_STATE *state) {
  static unsigned char blinkCounter = 0;
  static unsigned char blinkState = 0;
  
  switch (state->level) {
    case NM_LEVEL_GOOD:
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      break;

    case NM_LEVEL_NOTICE:
      pixels.setPixelColor(0, pixels.Color(255, 200, 0));
      break;

    case NM_LEVEL_WARNING:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      break;

    case NM_LEVEL_CRITICAL:
      if(blinkCounter == 0) {
        pixels.setPixelColor(0, blinkState ? pixels.Color(0, 0, 0) : pixels.Color(255, 0, 0));
        
        blinkState = !blinkState;
        blinkCounter = 2;
      } else {
        blinkCounter -= 1;
      }
      
      break;
  }
  
  pixels.show();

  Serial.print(state->soundLevel);
  Serial.print(" - ");
  Serial.println(state->level);
}

#else

void displayState(NOISOMETER_STATE *state) {
  static unsigned char blinkCounter = 0;
  static unsigned char blinkState = 0;
  
  switch (state->level) {
    case NM_LEVEL_GOOD:
      
      break;

    case NM_LEVEL_NOTICE:
      
      break;

    case NM_LEVEL_WARNING:
      
      break;

    case NM_LEVEL_CRITICAL:
      if(blinkCounter == 0) {
        
        
        blinkState = !blinkState;
        blinkCounter = 2;
      } else {
        blinkCounter -= 1;
      }
      
      break;
  }

  Serial.println(state->soundLevel);
}

#endif


void processState(NOISOMETER_STATE *state) {
  
  NM_LEVEL next_level = state->level;
  LEVEL_DURATION next_duration = TIME_S(2);
  
  switch (state->level) {
    case NM_LEVEL_GOOD:
      if (state->soundLevel > state->thresholds.notice_warning) {
        next_level = NM_LEVEL_WARNING;
        next_duration = state->thresholds.good_warning_time;
      } else if (state->soundLevel > state->thresholds.good_notice) {
        next_level = NM_LEVEL_NOTICE;
        next_duration = state->thresholds.good_notice_time;
      }
      break;

    case NM_LEVEL_NOTICE:
      if (state->soundLevel < state->thresholds.notice_good) {
        next_level = NM_LEVEL_GOOD;
        next_duration = state->thresholds.notice_good_time;
      } else if (state->soundLevel > state->thresholds.notice_warning) {
        next_level = NM_LEVEL_WARNING;
        next_duration = state->thresholds.notice_warning_time;
      }
      break;

    case NM_LEVEL_WARNING:
      if (state->soundLevel < state->thresholds.warning_notice) {
        next_level = NM_LEVEL_NOTICE;
        next_duration = state->thresholds.warning_notice_time;
      } else if (state->soundLevel > state->thresholds.warning_critical) {
        next_level = NM_LEVEL_CRITICAL;
        next_duration = state->thresholds.warning_critical_time;
      }
      break;

    case NM_LEVEL_CRITICAL:
      if (state->soundLevel < state->thresholds.critical_warning) {
        next_level = NM_LEVEL_WARNING;
        next_duration = state->thresholds.critical_warning_time;
      }

      break;
  }

  if(state->level != next_level && state->duration == 0) {
    // Level has changed
    state->duration = next_duration;
  } else if(gState.duration > 0) {
    gState.duration -= 1;
  }

  if(state->duration == 0) {
    state->lastLevel = state->level;
    state->level = next_level;
  }
}


void loop() {
  gState.soundLevel = measureSound(PIN_AUDIO_IN, SAMPLE_WINDOW);

  processState(&gState);

  displayState(&gState);

  delay(SAMPLE_TIME);
}
