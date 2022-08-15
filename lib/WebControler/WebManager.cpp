#include <EEPROMManager.h>
#include <WifiManager.h>
#include <WebManager.h>
#include <IOManager.h>
#include <FSManager.h>
#include <Const.h>

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>

Variables varWeb;
FSManager fsManagerWeb;
IOManager ioManagerWeb;
AsyncWebServer server(80);
WifiManager wifiManagerWeb;
EEPROMManager eepromManagerWeb;

String html;
boolean pageDebugOpen = false;

String statusShadowRain = "";
String statusImgRainHTML = "";
String statusNameRainHTML = "";

String statusShadowWindow = "";
String statusImgWindowHTML = "";
String statusNameWindowHTML = "";

WebManager::WebManager(){}

void WebManager::loadHTML(){
    
    AsyncElegantOTA.begin(&server);
    server.begin();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
        request->send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_INDEX));
    });

    server.on("/style", HTTP_GET, [](AsyncWebServerRequest *request){ 
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

    server.on("/info_logger", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);

        json["imgRain"] = statusImgRainHTML;
        json["txtRain"] = statusNameRainHTML;

        json["imgWindow"] = statusImgWindowHTML;
        json["txtWindow"] = statusNameWindowHTML;

        json["shadowRain"] = statusShadowRain;
        json["shadowWindow"] = statusShadowWindow;

        serializeJson(json, *response);
        request->send(response);
    });

    server.on("/get_logger", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);

        json["txtLogger"] = fsManagerWeb.readFileString(varWeb.FILE_LOG);
        
        serializeJson(json, *response);
        request->send(response);
    });
}

void WebManager::refreshStatus(int statusRain, int statusWindow){

    if(statusRain == 0){
        statusImgRainHTML = varWeb.STATUS_IMG_RAIN_DETECTED;
        statusNameRainHTML = varWeb.STATUS_TXT_RAIN_DETECTED;
        statusShadowRain = varWeb.STATUS_SHADOW_RAIN_DETECTED;
    } else{
        statusImgRainHTML = varWeb.STATUS_IMG_RAIN_NOT_DETECTED;
        statusNameRainHTML = varWeb.STATUS_TXT_RAIN_NOT_DETECTED;
        statusShadowRain = varWeb.STATUS_NOT_SHADOW;
    }
    
    if(statusWindow == 0){
        statusImgWindowHTML = varWeb.STATUS_IMG_WINDOW_OPEN;
        statusNameWindowHTML = varWeb.STATUS_TXT_WINDOW_OPEN;
        statusShadowWindow = varWeb.STATUS_SHADOW_WINDOW_OPEN;
    } else{
        statusImgWindowHTML = varWeb.STATUS_IMG_WINDOW_CLOSED;
        statusNameWindowHTML = varWeb.STATUS_TXT_WINDOW_CLOSED;
        statusShadowWindow = varWeb.STATUS_NOT_SHADOW;
    }
}