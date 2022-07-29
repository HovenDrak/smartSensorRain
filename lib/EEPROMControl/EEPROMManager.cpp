#include <Arduino.h>
#include <EEPROM.h>
#include <EEPROMManager.h>
#include <Const.h>

Variables varEEPROM;

EEPROMManager::EEPROMManager(){}

String EEPROMManager::getEEPROMString(byte offset){
  String msg = "";
  while (offset < varEEPROM.POSITION_EEPROM_TOTAL){
    if (EEPROM.read(offset) == 0){
      break;
    }
    msg += char(EEPROM.read(offset));
    offset++;
  }
  return msg;
}

void EEPROMManager::setEEPROMString(byte offset, String msg){
  for (byte b = 0; b <= msg.length(); b++){
    EEPROM.write(offset + b, msg[b]);
  }
  EEPROM.commit();
}