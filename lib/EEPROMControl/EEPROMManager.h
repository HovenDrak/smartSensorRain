#include <Arduino.h>

class EEPROMManager{
    public:
        EEPROMManager();
        String getEEPROMString(byte offset);
        void setEEPROMString(byte offset, String s);

        boolean compareWiFiEEPROM(String ssid, String password);
        void setWiFiEEPROM(String ssid, String password);
};