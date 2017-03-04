#include "noisometer.h"

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

  gState.level = NM_LEVEL_GOOD;

  gState.thresholds.good_notice = 15;
  gState.thresholds.notice_good = 10;

  gState.thresholds.notice_warning = 60;
  gState.thresholds.warning_notice = 50;

  gState.thresholds.warning_critical = 100;
  gState.thresholds.critical_warning = 80;
}

void displayState(NOISOMETER_STATE *state) {

  Serial.print(state->soundLevel);
  Serial.print(" -> ");
  Serial.println(state->level);
}

void processState(NOISOMETER_STATE *state) {

  NM_LEVEL next_level;

  switch (state->level) {
    case NM_LEVEL_GOOD:
      if (state->soundLevel > state->thresholds.good_notice) {
        state->level = NM_LEVEL_NOTICE;
      }
      break;

    case NM_LEVEL_NOTICE:
      if (state->soundLevel < state->thresholds.notice_good) {
        state->level = NM_LEVEL_GOOD;
      } else if (state->soundLevel > state->thresholds.notice_warning) {
        state->level = NM_LEVEL_WARNING;
      }
      break;

    case NM_LEVEL_WARNING:
      if (state->soundLevel < state->thresholds.warning_notice) {
        state->level = NM_LEVEL_NOTICE;
      } else if (state->soundLevel > state->thresholds.warning_critical) {
        state->level = NM_LEVEL_CRITICAL;
      }
      break;

    case NM_LEVEL_CRITICAL:
      if (state->soundLevel < state->thresholds.critical_warning) {
        state->level = NM_LEVEL_WARNING;
      }

      break;
  }
}


void loop() {
  gState.soundLevel = measureSound(PIN_AUDIO_IN, SAMPLE_WINDOW);

  processState(&gState);

  displayState(&gState);



  delay(100);
}
