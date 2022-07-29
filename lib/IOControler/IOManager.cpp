#include <ESP8266WiFi.h>
#include <IOManager.h>
#include <Arduino.h>
#include <Ticker.h>
#include <Const.h>

Ticker timer;
Variables varIO;

boolean canClose = true;

IOManager::IOManager(){}

void IOManager::pinSetConfig(){
    for(int i = 0; i < 5; i++){
        pinMode(varIO.PIN_OUTS[i], OUTPUT);
    }
    digitalWrite(varIO.PIN_OUT_BOT, LOW);

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
    digitalWrite(varIO.PIN_OUT_BOT, HIGH);
    delay(500);
    digitalWrite(varIO.PIN_OUT_BOT, LOW);
}

void IOManager::cmndLedWifi(boolean wifiConnected){
    if(wifiConnected){
        digitalWrite(varIO.PIN_LED_WIFI, HIGH);
        return;
    }
    digitalWrite(varIO.PIN_LED_WIFI, LOW);
}

void IOManager::cmndLedWindow(boolean windowClosed){
    if(windowClosed){
        digitalWrite(varIO.PIN_LED_WINDOW, HIGH);
        return;
    }
    digitalWrite(varIO.PIN_LED_WINDOW, LOW);
}

void IOManager::cmndLedRainSensor(boolean detectedRain){
    if(detectedRain){
        digitalWrite(varIO.PIN_LED_SENSOR_RAIN, HIGH);
        return;
    }
    digitalWrite(varIO.PIN_LED_SENSOR_RAIN, LOW);
}

boolean IOManager::verifyWindowClosed(){
    if(digitalRead(varIO.PIN_IN_WINDOW) == HIGH){
        cmndLedWindow(true);
        return true;
    }
    cmndLedWindow(false);
    return false;
}

boolean IOManager::verifyDetectedSensorRain(){
    if(digitalRead(varIO.PIN_IN_SENSOR_RAIN) == HIGH){
        cmndLedWindow(true);
        return true;
    }
    cmndLedWindow(false);
    return false;
}

void IOManager::verifySensors(){
    if(!verifyDetectedSensorRain()){
        if(!verifyWindowClosed()){
            canClose = true;
            return;
        }
    }
    canClose = false;
}

boolean IOManager::canCloseWindow(){
    if (WiFi.status() == WL_CONNECTED){
        return canClose;
    }
    return false;
}



