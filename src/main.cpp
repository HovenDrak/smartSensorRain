#include <WifiManager.h>
#include <ApiManager.h>
#include <WebManager.h>
#include <IOManager.h>
#include <FSManager.h>
#include <Const.h>

#include <Arduino.h>
#include <EEPROM.h>

WifiManager wifiManagerMain;
WebManager webManagerMain;
ApiManager apiManagerMain;
IOManager ioManagerMain;
FSManager fsManagerMain;
Variables varMain;

int verifySensors;
int verifyWifi;

void setup(){
  
  Serial.begin(varMain.SERIAL_RATE);
  EEPROM.begin(varMain.POSITION_EEPROM_TOTAL);
  ESP.wdtEnable(varMain.WATCHDOG_TIME);

  fsManagerMain.beginFS();
  ioManagerMain.pinSetConfig();
  ioManagerMain.ledCheckConfig();
  wifiManagerMain.getWifiEEPROM();
  wifiManagerMain.wifiOffConfig();
  webManagerMain.loadHTML();
  wifiManagerMain.wifiCheckConnection();
}

void loop(){

  ESP.wdtFeed();

  if((millis() - verifyWifi) >= 1000){
    verifyWifi = millis();
    ioManagerMain.verifySensors();
    yield();
  }

  if((millis() - verifyWifi) >= 10500){
    verifyWifi = millis();
    wifiManagerMain.wifiCheckConnection();
    yield();
  }
}