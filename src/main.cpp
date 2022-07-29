#include <Arduino.h>
#include <EEPROM.h>
#include <Const.h>
#include <FSManager.h>
#include <IOManager.h>
#include <WifiManager.h>
#include <WebManager.h>
#include <ApiManager.h>

IOManager ioManagerMain;
FSManager fsManagerMain;
WifiManager wifiManagerMain;
WebManager webManagerMain;
ApiManager apiManagerMain;
Variables varMain;

int verifyWifi;
int verifySensors;

void setup(){
  Serial.begin(varMain.SERIAL_RATE);
  EEPROM.begin(varMain.POSITION_EEPROM_TOTAL);
  ESP.wdtEnable(varMain.WATCHDOG_TIME);

  fsManagerMain.beginFS();
  ioManagerMain.pinSetConfig();
  ioManagerMain.ledCheckConfig();
  wifiManagerMain.wifiOffConfig();
  webManagerMain.loadHTML();
}

void loop(){

  ESP.wdtFeed();

  if(ioManagerMain.canCloseWindow()){
    apiManagerMain.postTrigger();
  }

  if((millis() - verifyWifi) >= 1000){
    verifyWifi = millis();
    ioManagerMain.verifySensors();
  }

  if((millis() - verifyWifi) >= 20000){
    verifyWifi = millis();
    wifiManagerMain.wifiCheckConnection();
  }
}