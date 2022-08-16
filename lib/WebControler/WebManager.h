#include <Arduino.h>

class WebManager{
    public:
        WebManager();
        void loadHTML();
        void refreshLogger();
        void refreshStatus(int statusRain, int statusWindow);
};