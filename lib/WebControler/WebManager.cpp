#include <EEPROMManager.h>
#include <WifiManager.h>
#include <WebManager.h>
#include <IOManager.h>
#include <FSManager.h>
#include <Const.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <LittleFS.h>

AsyncWebServer server(80);
EEPROMManager eepromManagerWeb;
WifiManager wifiManagerWeb;
FSManager fsManagerWeb;
IOManager ioManagerWeb;
Variables varWeb;

String html;
boolean pageDebugOpen = false;
String statusImgRainHTML = "";
String statusImgWindowHTML = "";
String statusNameRainHTML = "";
String statusNameWindowHTML = "";

WebManager::WebManager(){}

void WebManager::loadHTML(){
    
    server.begin();
//    ElegantOTA.begin();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_INDEX));
        pageDebugOpen = false;
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){ 
        request->send(200, "text/css", fsManagerWeb.readFileString(varWeb.FILE_CSS));
    });

    server.on("/config_wifi", HTTP_GET, [](AsyncWebServerRequest *request){ 
        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_WIFI)); 
    });

    server.on("/config_mqtt", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_MQTT)); 
    });

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_LOGGER));
    });

    server.on("/teste", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", fsManagerWeb.readFileString("/logger.html"));
    });

    server.on("/teste-botoeira", HTTP_GET, [](AsyncWebServerRequest *request){
        html = "";
        html += "comando por botoeira...";
        ioManagerWeb.cmndBot();
        fsManagerWeb.writeFile(varWeb.FILE_LOG, "Janela fechada por BOTOEIRA!!!", true);
        request->send(200, "text/html", html);
    });

    server.on("/reiniciar", HTTP_GET, [](AsyncWebServerRequest *request){
        html = "";
        html += "reiniciando...";
        fsManagerWeb.writeFile(varWeb.FILE_LOG, "Reiniciando ESP...", true);
        request->send(200, "text/html", html);
        ESP.restart();
    });

    server.on("/salvar-senha", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebParameter* ssid = request->getParam(0);
        AsyncWebParameter* pass = request->getParam(1);
        String SSID = ssid->value();
        String PASSWORD = pass->value();

        Serial.println("[HTML] NOME DO WIFI INSERIDO HTML: " + SSID);
        Serial.println("[HTML] SENHA DO WIFI INSERIDO HTML: " + PASSWORD);

        eepromManagerWeb.setEEPROMString(varWeb.POSITION_EEPROM_SSID, SSID);
        eepromManagerWeb.setEEPROMString(varWeb.POSITION_EEPROM_PASSWORD, PASSWORD);
        
        wifiManagerWeb.wifiCheckConnection();

        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_WIFI));
    });

    server.on("/status_rain", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);
        json["imgRain"] = statusImgRainHTML;
        json["txtRain"] = statusNameRainHTML;
        json["imgWindow"] = statusImgWindowHTML;
        json["txtWindow"] = statusNameWindowHTML;
        serializeJson(json, *response);
        request->send(response);
    });
}

void WebManager::refreshStatus(int statusRain, int statusWindow){

    if(statusRain == 1){
        statusImgRainHTML = varWeb.STATUS_RAIN_DETECTED;
        statusNameRainHTML = "Com Chuva";
        
    } else {
        statusImgRainHTML = varWeb.STATUS_RAIN_NOT_DETECTED;
        statusNameRainHTML = "Sem Chuva";
    }

    if(statusWindow == 1){
        statusImgWindowHTML = varWeb.STATUS_SENSOR_OPEN;
        statusNameWindowHTML = "Aberto";
    } else {
        statusImgWindowHTML = varWeb.STATUS_SENSOR_CLOSED;
        statusNameWindowHTML = "Fechado";
    }
      
}