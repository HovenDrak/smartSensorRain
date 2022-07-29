#include <Arduino.h>
#include <FSManager.h>
#include <LittleFS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Const.h>

Variables varFS;

FSManager::FSManager(){}

String dia[] = {"Domingo", "Segunda-Feira", "Terça-Feira", "Quarta-Feira", 
"Quinta-Feira", "Sexta-Feira", "Sábado"};
WiFiUDP udp;
NTPClient ntpClient(udp, "b.ntp.br", -3 * 3600);

void FSManager::beginFS(){
    if(LittleFS.begin()){
        createFile(varFS.FILE_LOG);
        createFile(varFS.FILE_LOG_STATUS);
    }
}

String FSManager::readFileString(String fileName){
    
    if(LittleFS.exists(fileName)){
        Serial.println("[FS] LENDO O ARQUVO: " + fileName);
        File f = LittleFS.open(fileName, "r");
        String data = f.readString();
        f.close();
        return data;
    }
    Serial.println("[FS] ARQUIVO: " + fileName + " NÃO EXISTE!!!");
    return "";
}

void FSManager::writeFile(String fileName, String msg){

    File rFile = LittleFS.open(fileName, "a+");

    if (!rFile){
        Serial.println("[FS] ERRO AO ABRIR ARQUIVO");
        return;
    } 

    ntpClient.update();
    String hora = ntpClient.getFormattedTime();
    time_t epoch = ntpClient.getEpochTime();
    
    struct tm ts;
    char date[80];
    time(&epoch);
    ts = *localtime(&epoch);
    strftime(date, sizeof(date), "%d/%m/%Y ", &ts);

    Serial.printf("\n[%s]: %s", (date + hora).c_str(), msg.c_str());
    rFile.printf("\n[%s]: %s", (date + hora).c_str(), msg.c_str());
    rFile.close();
}

void FSManager::createFile(String nameFile){
    
    if (LittleFS.exists(nameFile)){
        return;
    }
    File wFile = LittleFS.open(nameFile, "w+");

    if(!wFile){
        createFile(nameFile);
    }
}