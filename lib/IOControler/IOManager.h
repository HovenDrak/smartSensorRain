#include <Arduino.h>

class IOManager{
    public:
        IOManager();
        void cmndBot();
        void pinSetConfig();
        void verifySensors();
        void ledCheckConfig();
        boolean verifyWindowClosed();
        boolean verifyDetectedSensorRain();
        void cmndLed(int led, boolean active);
        void cmndLedWifi(boolean wifiConnected);
};