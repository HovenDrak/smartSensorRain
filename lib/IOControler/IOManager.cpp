#include <ESP8266WiFi.h>
#include <IOManager.h>
#include <Arduino.h>
#include <Ticker.h>
#include <Const.h>
#include <ApiManager.h>
#include <WebManager.h>

Ticker timer;
Variables varIO;
WebManager webManagerIO;
ApiManager apiManagerIO;

int STATUS_SENSORS[2] = {0,0};

IOManager::IOManager(){}

void IOManager::pinSetConfig(){
    for(int i = 0; i < 5; i++){
        pinMode(varIO.PIN_OUTS[i], OUTPUT);
        digitalWrite(varIO.PIN_OUTS[i], HIGH);
    }
    

    for(int i = 0; i < 2; i++){
        pinMode(varIO.PIN_IN[i], INPUT_PULLUP);
    }
}

void ICACHE_RAM_ATTR timerLED_OSC(){
  timer1_write(varIO.TICK_EVENT_COUNTER);
  digitalWrite(varIO.PIN_LED_BUILTIN, digitalRead(varIO.PIN_LED_BUILTIN) ^ 1);
}

void IOManager::ledCheckConfig(){
    timer1_attachInterrupt(timerLED_OSC);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(varIO.TICK_EVENT_COUNTER);
}

void IOManager::cmndBot(){
    digitalWrite(varIO.PIN_OUT_BOT, LOW);
    delay(500);
    digitalWrite(varIO.PIN_OUT_BOT, HIGH);
}

void IOManager::cmndLedWifi(boolean wifiConnected){
    if(wifiConnected){
        digitalWrite(varIO.PIN_LED_WIFI, LOW);
        return;
    }
    digitalWrite(varIO.PIN_LED_WIFI, HIGH);
}

void IOManager::cmndLed(int led, boolean active){
    switch(led){
        case 12:
            if(active){
                digitalWrite(varIO.PIN_LED_SENSOR_RAIN, LOW);
                break;
            } 
            digitalWrite(varIO.PIN_LED_SENSOR_RAIN, HIGH);
            break;
            
        case 13:
            if(active){
                digitalWrite(varIO.PIN_LED_WINDOW, LOW);
                break;
            } 
            digitalWrite(varIO.PIN_LED_WINDOW, HIGH);
            break;
    }
}

boolean IOManager::verifyWindowClosed(){
    if(digitalRead(varIO.PIN_IN_WINDOW) == 0){
        cmndLed(varIO.PIN_LED_WINDOW, true);
        return true;
    }
    cmndLed(varIO.PIN_LED_WINDOW, false);
    return false;
}

boolean IOManager::verifyDetectedSensorRain(){
    if(digitalRead(varIO.PIN_IN_SENSOR_RAIN) == LOW){
        cmndLed(varIO.PIN_LED_SENSOR_RAIN, true);
        return true;
    }
    cmndLed(varIO.PIN_LED_SENSOR_RAIN, false);
    return false;
}

void IOManager::verifySensors(){
    boolean canClose = true;

    for(int i = 0; i < 2; i++){
        switch (digitalRead(varIO.PIN_IN[i])){
            case 0:
                if(STATUS_SENSORS[i] != 0){
                    STATUS_SENSORS[i] = 0;
                }
                cmndLed(varIO.PIN_IN[i], true);
                break;
            case 1:
                if(STATUS_SENSORS[i] != 1){
                    STATUS_SENSORS[i] = 1;
                }
                cmndLed(varIO.PIN_IN[i], false);
                canClose = false;
                break;

            default:
                Serial.println("[ESP8266] ERROR AO LER O SENSOR");
                break;
        }
    }

    webManagerIO.refreshStatus(STATUS_SENSORS[0], STATUS_SENSORS[1]);
    
    if(canClose){
        if(WiFi.status() == WL_CONNECTED){
            apiManagerIO.postTrigger();
        } else {
            cmndBot();
            webManagerIO.refreshLogger();
            delay(7000);
        }
    }
}




