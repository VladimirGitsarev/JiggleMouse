#ifndef STRUCTS_H
#define STRUCTS_H

// State Machine
enum JigglerState {
  STATE_SPIN_FWD = 0,
  STATE_PAUSE_1 = 1,
  STATE_SPIN_REV = 2,
  STATE_PAUSE_2 = 3
};

// Permanent Settings
struct Settings {
  int servoPin;
  int fwdSpeed;
  int revSpeed;
  unsigned long spinTime;
  unsigned long pauseTime;
  bool isRunning;     
  bool startOnBoot;   
  bool isInitialized; 
};

#endif