#include <Arduino.h>

class Variables{
    public:

        // -------------X CONST WIFI X------------- // 
        const char* WIFI_SSID_OFF = "Sensor de Chuva";
        const char* WIFI_PASS_OFF = "12345678";
        
        // -------------X CONST PINS X------------- // 
        volatile int TICK_EVENT_COUNTER = 1500000;

        int PIN_OUTS[5] = {16, 4, 14, 5, LED_BUILTIN};
        int PIN_OUT_BOT = PIN_OUTS[0];
        int PIN_LED_WIFI = PIN_OUTS[1];
        int PIN_LED_WINDOW = PIN_OUTS[2];
        int PIN_LED_SENSOR_RAIN = PIN_OUTS[3];
        int PIN_LED_BUILTIN = PIN_OUTS[4];
        
        int PIN_IN[2] = {12, 13};
        int PIN_IN_SENSOR_RAIN = PIN_IN[0];
        int PIN_IN_WINDOW = PIN_IN[1];
        
        // -------------X CONST API X------------- // 
        String TOKEN = "d417cb59bf5794971b80480eeb7c73ed7470eaca"; //token permanente
        String HOST = "https://trigger.fullarm.com";
        String PATH_TRIGGER = "/api/v1/trigger/2369";

        // -------------X CONST WEB X------------- //
        String FILE_LOG = "/log.txt";
        String FILE_LOG_STATUS = "/logStatus.txt";

        String IMG_WINDOW_OPEN = "/icon_sensor_open.png";
        String IMG_WINDOW_CLOSED = "/icon_sensor_close.png";
        String IMG_RAIN_DETECTED = "/icon_rain_detected.png";
        String IMG_RAIN_NOT_DETECTED = "/icon_rain_not_detected.png";

        String FILE_CSS = "/style.css";
        String PATH_ROOT_HTML = "/html";
        String FILE_JS_LOGGER = "/js/logger.js";
        String FILE_HTML_WIFI = PATH_ROOT_HTML + "/wifi.html";
        String FILE_HTML_MQTT = PATH_ROOT_HTML + "/mqtt.html";
        String FILE_HTML_INDEX = PATH_ROOT_HTML + "/index.html";
        String FILE_HTML_LOGGER = PATH_ROOT_HTML + "/logger.html";
        
        String STATUS_TXT_WINDOW_OPEN = "Aberto";
        String STATUS_IMG_WINDOW_OPEN = "window_open";
        String STATUS_SHADOW_WINDOW_OPEN = "shadow-status-window-open";

        String STATUS_TXT_WINDOW_CLOSED = "Fechado";
        String STATUS_IMG_WINDOW_CLOSED = "window_closed";

        String STATUS_NOT_SHADOW = "shadow-status-ok";

        String STATUS_IMG_RAIN_DETECTED = "rainy";
        String STATUS_TXT_RAIN_DETECTED = "Com Chuva";
        String STATUS_SHADOW_RAIN_DETECTED = "shadow-status-rain-detected";

        String STATUS_IMG_RAIN_NOT_DETECTED = "cloudy";
        String STATUS_TXT_RAIN_NOT_DETECTED = "Sem Chuva";


        // -------------X CONST EPROM X------------- //
        const byte POSITION_EEPROM_SSID = 0;
        const byte POSITION_EEPROM_PASSWORD = 40 + POSITION_EEPROM_SSID;
        const byte POSITION_EEPROM_TOTAL = 40 + POSITION_EEPROM_PASSWORD;

        // -------------X CONST ESP X------------- //
        int SERIAL_RATE = 115200;
        int WATCHDOG_TIME = 17000;

};