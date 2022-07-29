#include <Arduino.h>

class EEPROMManager{
    public:
        EEPROMManager();
        String getEEPROMString(byte offset);
        void setEEPROMString(byte offset, String s);
};