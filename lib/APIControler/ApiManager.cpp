#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <ApiManager.h>
#include <FSManager.h>
#include <IOManager.h>
#include <Arduino.h>
#include <Const.h>
#include <vector>

Variables varApi;
IOManager ioManagerApi;
FSManager fsManagerApi;

std::unique_ptr <BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
HTTPClient https;

const uint8_t  fingerprint[] = { 0x31, 0x68, 0x58, 0xc6, 0xb2, 0xbe, 0x94, 0xa4, 0x5c, 0xe2, 0xc8, 0x3e, 0x7d, 0x23, 0xb4, 0xd9, 0xd9, 0xfe, 0xd4, 0xb6};

ApiManager::ApiManager(){}

void ApiManager::postTrigger(){

    client->setFingerprint(fingerprint);

    if(https.begin(*client, varApi.HOST + varApi.PATH_TRIGGER)){
        
        https.addHeader("Authorization", "Bearer " + varApi.TOKEN);
        https.addHeader("Content-Type", "application/json");  
        https.addHeader("Connection", "keep-alive");
        
        int httpCode = https.POST("");
        delay(1000);
        Serial.println(httpCode);
        String httpBody = https.getString();
        https.end();

        if(httpCode != 200){
            fsManagerApi.writeFile(varApi.FILE_LOG, "FALHA NO ACIONAMENTO DA TRIGGER.", true);
            fsManagerApi.writeFile(varApi.FILE_LOG, "[HTTP] CODE: " + httpCode, true);
            fsManagerApi.writeFile(varApi.FILE_LOG, "[HTTP] BODY: " + httpBody, true);
            
            return;
        }
        fsManagerApi.writeFile(varApi.FILE_LOG, "TRIGGER ACIONADA COM SUCESSO.", true);
        fsManagerApi.writeFile(varApi.FILE_LOG, "AGUARDANDO JANELA SER FECHADA.", true);
        delay(7000);

        if(!ioManagerApi.verifyWindowClosed()){
            fsManagerApi.writeFile(varApi.FILE_LOG, "JANELA FECHADA POR REQUEST.", true);
            return;
        }
        fsManagerApi.writeFile(varApi.FILE_LOG, "FALHA EM FECHAR JANELA POR REQUEST.", true);
        ioManagerApi.cmndBot();
        fsManagerApi.writeFile(varApi.FILE_LOG, "BOTEIRA FOI ACIONADO.", true);
        delay(7000);
    }
}
