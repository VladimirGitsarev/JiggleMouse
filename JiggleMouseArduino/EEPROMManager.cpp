#include "EEPROMManager.h"
#include <EEPROM.h>
#include "Constants.h"

void loadSettings() {
  EEPROM.get(EEPROM_ADDR, config);
  
  if (
    config.isInitialized != true || 
    config.fwdSpeed < MIN_SPEED || 
    config.fwdSpeed > MAX_SPEED || 
    config.revSpeed < MIN_SPEED || 
    config.revSpeed > MAX_SPEED || 
    config.servoPin < MIN_SERVO_PIN || 
    config.servoPin > MAX_SERVO_PIN
    ) {
    Serial.println("No valid settings found. Loading defaults...");
    config.fwdSpeed = DEFAULT_FWD_SPEED;
    config.revSpeed = DEFAULT_REV_SPEED;
    config.spinTime = DEFAULT_SPIN_TIME;
    config.pauseTime = DEFAULT_PAUSE_TIME;
    config.servoPin = DEFAULT_SERVO_PIN; 
    config.isRunning = false; 
    config.startOnBoot = false; 
    config.isInitialized = true;
    saveSettings();
  }
}

void saveSettings() {
  EEPROM.put(EEPROM_ADDR, config);
}

void printCurrentSettings() {
  Serial.println("Current Settings");
  Serial.println("Status: " + String(config.isRunning ? "RUNNING" : "STOPPED"));
  Serial.println("Start On Boot: " + String(config.startOnBoot ? "TRUE" : "FALSE"));
  Serial.println("Servo Pin: D" + String(config.servoPin));
  Serial.println("FWD Speed: " + String(config.fwdSpeed));
  Serial.println("REV Speed: " + String(config.revSpeed));
  Serial.println("Spin Time: " + String(config.spinTime) + "ms");
  Serial.println("Pause Time: " + String(config.pauseTime) + "ms");
  Serial.println("------------------------");
}