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
  Serial.println("[EEPROM] Mensagem: " + msg);
  return msg;
}

void EEPROMManager::setEEPROMString(byte offset, String msg){
  for (byte b = 0; b <= msg.length(); b++){
    EEPROM.write(offset + b, msg[b]);
  }
  EEPROM.commit();
}

void EEPROMManager::setWiFiEEPROM(String ssid, String password){
  EEPROM.begin(98);

  if(!compareWiFiEEPROM(ssid, password)) {

    EEPROM.write(0, ssid.length());
    for(int i = 2; i < 2 + ssid.length(); i++) {
      EEPROM.write(i, ssid.charAt(i-2));
    }

    EEPROM.write(1, password.length());
    for(int j = 2 + ssid.length(); j < 2 + ssid.length()+ password.length(); j++) {
      EEPROM.write(j, password.charAt(j - 2 - ssid.length()));
    }
    EEPROM.commit();
  }
  EEPROM.end();
}

boolean EEPROMManager::compareWiFiEEPROM(String ssid, String password){
  int ssidLenght = int(EEPROM.read(0));
  int passLength = int(EEPROM.read(1)); 
  String ssidWifi = "";
  String pass = "";

  for(int i = 2; i < 2 + ssidLenght; i++) {
    ssid += char(EEPROM.read(i));
  }

  for(int j = 2 + ssidLenght; j < 2+ ssidLenght + passLength; j++) {
    ssidWifi = pass + char(EEPROM.read(j));
  }

  if(ssidWifi.equals(ssid) && pass.equals(password)) {
    return true;
  }
  return false;
}