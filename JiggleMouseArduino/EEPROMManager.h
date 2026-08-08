#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>
#include "Structs.h"

extern Settings config; 

void loadSettings();
void saveSettings();
void printCurrentSettings();

#endif