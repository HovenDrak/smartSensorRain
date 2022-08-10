#include <Arduino.h>

class WebManager{
    public:
        WebManager();
        void loadHTML();
        void refreshStatus(int statusRain, int statusWindow);
};