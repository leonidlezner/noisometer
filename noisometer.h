#ifndef __noisometer_h
#define __noisometer_h


#define SAMPLE_TIME       150   // ms
#define SAMPLE_WINDOW     50    // ms
#define PIN_AUDIO_IN      A2
#define PIN_LIGHT_RED     4
#define PIN_LIGHT_YELLOW  3
#define PIN_LIGHT_GREEN   2



#define TIME_S(T) (T*1000/(SAMPLE_TIME + SAMPLE_WINDOW))
typedef unsigned int SOUND_LEVEL;
typedef unsigned char LEVEL_DURATION;

enum NM_LEVEL {
  NM_LEVEL_GOOD = 0,
  NM_LEVEL_NOTICE,
  NM_LEVEL_WARNING,
  NM_LEVEL_CRITICAL
};

typedef struct __thresholds {
  SOUND_LEVEL good_notice;
  LEVEL_DURATION good_notice_time;
  
  LEVEL_DURATION good_warning_time;
  
  SOUND_LEVEL notice_good;
  LEVEL_DURATION notice_good_time;
  
  SOUND_LEVEL notice_warning;
  LEVEL_DURATION notice_warning_time;
  
  SOUND_LEVEL warning_notice;
  LEVEL_DURATION warning_notice_time;
  
  SOUND_LEVEL warning_critical;
  LEVEL_DURATION warning_critical_time;
  
  SOUND_LEVEL critical_warning;
  LEVEL_DURATION critical_warning_time;
  
} NOISOMETER_THRESHOLDS;

typedef struct _noisometer_state {
  NM_LEVEL level;
  NM_LEVEL lastLevel;
  SOUND_LEVEL soundLevel;
  LEVEL_DURATION duration;
  NOISOMETER_THRESHOLDS thresholds;
} NOISOMETER_STATE;



#endif
