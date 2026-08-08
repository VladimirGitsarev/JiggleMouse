#include <Servo.h>
#include "Constants.h"
#include "Structs.h"
#include "Commands.h"
#include "EEPROMManager.h" 

Servo jigglerServo;

Settings config; 
unsigned long previousMillis = 0;
JigglerState currentState = STATE_SPIN_FWD; 

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  loadSettings();
  
  jigglerServo.attach(config.servoPin);
  
  if (config.startOnBoot) {
    config.isRunning = true;
    currentState = STATE_SPIN_FWD; 
    previousMillis = millis(); 
    jigglerServo.write(STOP_MOTOR + config.fwdSpeed);
  }
  
  Serial.println("Jiggler Ready. Waiting for commands...");
  printCurrentSettings();
}

void loop() {
  handleSerialCommands();
  runJigglerLogic();
}

// Servo logic
void runJigglerLogic() {
  if (!config.isRunning) {
    return;
  }

  unsigned long currentMillis = millis();
  unsigned long waitTime = 0;

  if (currentState == STATE_SPIN_FWD || currentState == STATE_SPIN_REV) {
    waitTime = config.spinTime;
  } else {
    waitTime = config.pauseTime;
  }

  if (currentMillis - previousMillis >= waitTime) {
    previousMillis = currentMillis;
    currentState = static_cast<JigglerState>((currentState + 1) % 4);

    switch (currentState) {
      case STATE_SPIN_FWD:
        jigglerServo.write(STOP_MOTOR + config.fwdSpeed);
        break;
      case STATE_PAUSE_1:
        jigglerServo.write(STOP_MOTOR);
        break;
      case STATE_SPIN_REV:
        jigglerServo.write(STOP_MOTOR - config.revSpeed);
        break;
      case STATE_PAUSE_2:
        jigglerServo.write(STOP_MOTOR);
        break;
    }
  }
}

// Serial Listener
void handleSerialCommands() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); 

    if (input == CMD_START) {
      config.isRunning = true;
      currentState = STATE_SPIN_FWD; 
      previousMillis = millis(); 
      jigglerServo.write(STOP_MOTOR + config.fwdSpeed); 
      saveSettings();
      Serial.println("Jiggler STARTED.");
    }
    else if (input == CMD_STOP) {
      config.isRunning = false;
      jigglerServo.write(STOP_MOTOR); 
      saveSettings();
      Serial.println("Jiggler STOPPED.");
    }
    else if (input.startsWith(CMD_SET_FWD)) {
      int newSpeed = input.substring(CMD_SET_FWD.length()).toInt();
      config.fwdSpeed = constrain(newSpeed, MIN_SPEED, MAX_SPEED);
      saveSettings();
      Serial.println("Forward speed updated to: " + String(config.fwdSpeed));
    } 
    else if (input.startsWith(CMD_SET_REV)) {
      int newSpeed = input.substring(CMD_SET_REV.length()).toInt();
      config.revSpeed = constrain(newSpeed, MIN_SPEED, MAX_SPEED);
      saveSettings();
      Serial.println("Reverse speed updated to: " + String(config.revSpeed));
    }
    else if (input.startsWith(CMD_SET_SPIN)) {
      config.spinTime = input.substring(CMD_SET_SPIN.length()).toInt();
      saveSettings();
      Serial.println("Spin time updated to: " + String(config.spinTime) + "ms");
    } 
    else if (input.startsWith(CMD_SET_PAUSE)) {
      config.pauseTime = input.substring(CMD_SET_PAUSE.length()).toInt();
      saveSettings();
      Serial.println("Pause time updated to: " + String(config.pauseTime) + "ms");
    } 
    else if (input.startsWith(CMD_SET_START_ON_BOOT)) {
      config.startOnBoot = (input.substring(CMD_SET_START_ON_BOOT.length()).toInt() == 1);
      saveSettings();
      Serial.println("Start On Boot updated to: " + String(config.startOnBoot ? "TRUE" : "FALSE"));
    }
    else if (input.startsWith(CMD_SET_PIN)) {
      int newPin = input.substring(CMD_SET_PIN.length()).toInt();
      
      if (newPin >= MIN_SERVO_PIN && newPin <= MAX_SERVO_PIN) {
        config.servoPin = newPin;
        jigglerServo.detach(); 
        jigglerServo.attach(config.servoPin); 
        saveSettings();
        Serial.println("Servo Pin updated to: D" + String(config.servoPin));
      } else {
        Serial.println("Error: Invalid pin. Please use pins " + String(MIN_SERVO_PIN) + " through " + String(MAX_SERVO_PIN) + ".");
      }
    }
    else if (input == CMD_GET_ALL) {
      printCurrentSettings();
    } 
    else if (input.length() > 0) {
      Serial.println("Unknown Command.");
    }
  }
}