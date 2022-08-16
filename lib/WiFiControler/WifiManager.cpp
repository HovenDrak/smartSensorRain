#include <ESP8266WiFi.h>
#include <WifiManager.h>
#include <IOManager.h>
#include <Arduino.h>
#include <FSManager.h>
#include <EEPROMManager.h>
#include <Const.h>
#include <EEPROM.h>

IOManager ioManagerWifi;
FSManager fsManagerWifi;
Variables varWifi;
EEPROMManager eepromManagerWifi;

String WIFI_SSID = "";
String WIFI_PASS = "";

WifiManager::WifiManager(){}

void WifiManager::wifiCheckConnection(){

  if(WIFI_SSID.equals("") || WIFI_PASS.equals("")){
    return;
  }

  if (WiFi.status() == WL_CONNECTED){
    ioManagerWifi.cmndLedWifi(true);
    return;
  } 
  Serial.println("DESCONECTADO!!!");
  ioManagerWifi.cmndLedWifi(false);

  Serial.print("[ESP8266] CONECTANDO AO WIFI: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  ioManagerWifi.cmndLedWifi(true);
  
  Serial.println("[ESP8266] WIFI CONECTADO!!!");

  Serial.println("[ESP8266] IP ADDRESS: ");
  Serial.println(WiFi.localIP());

  Serial.println("[ESP8266] MAC: " + WiFi.macAddress());
}

void WifiManager::wifiOffConfig(){
  IPAddress staticIP(172, 19, 1, 255);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(varWifi.WIFI_SSID_OFF, varWifi.WIFI_PASS_OFF, 2, 0);
  WiFi.config(staticIP, gateway, subnet);
}

void WifiManager::getWifiEEPROM(){
  WIFI_SSID = eepromManagerWifi.getEEPROMString(varWifi.POSITION_EEPROM_SSID);
  WIFI_PASS = eepromManagerWifi.getEEPROMString(varWifi.POSITION_EEPROM_PASSWORD);
}
