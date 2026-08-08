#ifndef CONSTANTS_H
#define CONSTANTS_H

// Hardware & EEPROM
const int SERVO_PIN = 2;
const long SERIAL_BAUD_RATE = 115200;
const int EEPROM_ADDR = 0;

// Motor Control
const int STOP_MOTOR = 90;
const int MIN_SPEED = 1;
const int MAX_SPEED = 90;

// Default Settings
const int DEFAULT_FWD_SPEED = 10;
const int DEFAULT_REV_SPEED = 10;
const unsigned long DEFAULT_SPIN_TIME = 1500;
const unsigned long DEFAULT_PAUSE_TIME = 500;
const int DEFAULT_SERVO_PIN = 9;

// Pin Limits
const int MIN_SERVO_PIN = 2;
const int MAX_SERVO_PIN = 19;
#endif