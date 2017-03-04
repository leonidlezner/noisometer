#ifndef __noisometer_h
#define __noisometer_h



#define SAMPLE_WINDOW   50
#define PIN_AUDIO_IN    A0

typedef unsigned int SOUND_LEVEL;
typedef unsigned int LEVEL_DURATION;

enum NM_LEVEL {
  NM_LEVEL_GOOD = 0,
  NM_LEVEL_NOTICE,
  NM_LEVEL_WARNING,
  NM_LEVEL_CRITICAL
};

typedef struct __thresholds {
  SOUND_LEVEL good_notice;
  SOUND_LEVEL notice_good;
  SOUND_LEVEL notice_warning;
  SOUND_LEVEL warning_notice;
  SOUND_LEVEL warning_critical;
  SOUND_LEVEL critical_warning;
} NOISOMETER_THRESHOLDS;

typedef struct _noisometer_state {
  NM_LEVEL level;
  SOUND_LEVEL soundLevel;
  
  LEVEL_DURATION duration;
  
  NOISOMETER_THRESHOLDS thresholds;
} NOISOMETER_STATE;



#endif
