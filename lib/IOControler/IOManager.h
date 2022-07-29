#include <Arduino.h>

class IOManager{
    public:
        IOManager();
        void cmndBot();
        void pinSetConfig();
        void verifySensors();
        void ledCheckConfig();
        boolean canCloseWindow();
        boolean verifyWindowClosed();
        boolean verifyDetectedSensorRain();
        void cmndLedWifi(boolean wifiConnected);
        void cmndLedWindow(boolean wifiConnected);
        void cmndLedRainSensor(boolean wifiConnected);
};