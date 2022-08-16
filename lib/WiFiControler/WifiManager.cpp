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

WifiManager::WifiManager(){}

void WifiManager::wifiCheckConnection(){

  String WIFI_SSID = eepromManagerWifi.getEEPROMString(varWifi.POSITION_EEPROM_SSID);
  String WIFI_PASS = eepromManagerWifi.getEEPROMString(varWifi.POSITION_EEPROM_PASSWORD);

  Serial.println("[EEPROM] SSID: " + WIFI_SSID + " PASS: " + WIFI_PASS);

  if(WIFI_SSID.equals("") || WIFI_PASS.equals("")){
    return;
  }

  Serial.println("[WIFI] SSID: " + WIFI_SSID);
  Serial.println("[WIFI] PASSWORD: " + WIFI_PASS);
  Serial.print("[WIFI] VERIFICANDO CONEXAO WIFI... ");

  if (WiFi.status() == WL_CONNECTED){
    Serial.println("CONECTADO!!!");
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