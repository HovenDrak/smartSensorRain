#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <WebManager.h>
#include <Arduino.h>
#include <FSManager.h>
#include <IOManager.h>
#include <EEPROMManager.h>
#include <WifiManager.h>
#include <Const.h>

ESP8266WebServer server(80);
EEPROMManager eepromManagerWeb;
WifiManager wifiManagerWeb;
FSManager fsManagerWeb;
IOManager ioManagerWeb;
Variables varWeb;


WebManager::WebManager(){}

String html;

void WebManager::loadHTML(){
    server.begin();
    ElegantOTA.begin(&server);

    server.on("/", [](){ 
        server.send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_INDEX));
    });

    server.on("/config_wifi", [](){ 
        server.send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_WIFI)); 
    });

    server.on("/config_mqtt", [](){
        server.send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_MQTT)); 
    });

    server.on("/debug", [](){
        html = "";
        html += "<!DOCTYPE html>";
        html += "<html lang=\"pt-br\">";
        html += "<head>";
        html += "<meta charset=\"UTF-8\">";
        html += "<meta http-equiv='refresh' content='5'>";
        html += "<title>Sensor de Chuva</title>";
        html += "<style>";
        html += "body{background-color: navy;text-align: center; align-items: center;}";
        html += "h2{font-family: Arial, Helvetica, sans-serif;color: white;}";
        html += ".button{width: 300px;height: 50px;border-radius: 10px;background-color: grey;position: relative;}";
        html += ".logger{background-color: rgb(38, 70, 80); position:relative; left:30%; width: 40%; height: 400px; overflow: scroll; overflow-x: hidden; align-items: flex-start; border-radius: 15px;border : 2px solid black;}";
        html += ".logger::-webkit-scrollbar{width: 12px; }";
        html += ".logger::-webkit-scrollbar-track{background: rgb(38, 70, 80); border-radius: 100px;}";
        html += ".logger::-webkit-scrollbar-thumb{background-color: dimgray; border-radius: 30px; border: 2px solid black;}";
        html += ".button:hover{background-color:dimgrey;color: white;border: 2px solid black;}";
        html += ".titulo{font-family: Arial, Helvetica, sans-serif;color: white;}";
        html += ".status{width: 24%; height: 200px; background-color: rgb(38, 70, 80); position: absolute; left: 3%; bottom: 45%; border: 2px solid black; border-radius: 10px;}";
        html += "</style>";
        html += "</head>";
        html += "<body>";
        html += "<h2 class=\"titulo\">Sensor de Chuva</h2>";
        html += "<h3 class=\"titulo\">Logger de Informações</h3>";
        html += "<div class=\"logger\" id=\"log\">";
        html += "<p style=\"padding: 10px; text-align: start; color: white;\">";
        html += fsManagerWeb.readFileString(varWeb.FILE_LOG);
        html += "</p>";
        html += "</div>";
        html += "<p></p>";
        html += "<div class=\"status\"><h3 class=\"titulo\">Tabela de Status</h3>";
        html += "<p style=\"padding: 10px; text-align: start; color: white;\">";
        html += fsManagerWeb.readFileString(varWeb.FILE_LOG_STATUS);
        html += "</p>";
        html += "</div>";
        html += "<a href=\"/\">";
        html += "<button class=\"button\">Voltar para Home</button>";
        html += "</a>";
        html += "</body>";
        html += "<script>";
        html += "var logger = document.querySelector('#log');";
        html += "logger.scrollTop = logger.scrollHeight - logger.clientHeight;";
        html += "</script>";
        html += "</html>";
        server.send(200, "text/html", html);
    });

    server.on("/teste-botoeira", [](){
        html = "";
        html += "comando por botoeira...";
        ioManagerWeb.cmndBot();
        fsManagerWeb.writeFile(varWeb.FILE_LOG, "Janela fechada por BOTOEIRA!!!");
        server.send(200, "text/html", html);
    });

    server.on("/reiniciar", [](){
        html = "";
        html += "reiniciando...";
        fsManagerWeb.writeFile(varWeb.FILE_LOG, "Reiniciando ESP...");
        server.send(200, "text/html", html);
        ESP.restart();
    });

    server.on("/salvar-senha", [](){

        String SSID = server.arg(0);
        String PASSWORD = server.arg(1);

        Serial.println("[HTML] NOME DO WIFI INSERIDO HTML: " + SSID);
        Serial.println("[HTML] SENHA DO WIFI INSERIDO HTML: " + PASSWORD);

        eepromManagerWeb.setEEPROMString(varWeb.POSITION_EEPROM_SSID, SSID);
        eepromManagerWeb.setEEPROMString(varWeb.POSITION_EEPROM_PASSWORD, PASSWORD);

        wifiManagerWeb.wifiCheckConnection();

        server.send(200, "text/html", fsManagerWeb.readFileString(varWeb.FILE_HTML_WIFI));
    });
}