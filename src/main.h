#ifndef _MAIN_H
#define _MAIN_H
// => Hardware select
// #define LILYGO_WATCH_2019_WITH_TOUCH     // To use T-Watch2019 with touchscreen, please uncomment this line
// #define LILYGO_WATCH_2019_NO_TOUCH       // To use T-Watch2019 Not touchscreen , please uncomment this line
#define LILYGO_WATCH_2020_V1             //To use T-Watch2020, please uncomment this line
#define LILYGO_WATCH_LVGL                   //To use LVGL, you need to enable the macro LVGL
#define TWATCH_USE_PSRAM_ALLOC_LVGL
// Hardware NOT SUPPORT
//// #define LILYGO_WATCH_BLOCK
// Hardware NOT SUPPORT
#include <LilyGoWatch.h>
#include <signalk/signalk_emitter.h>
#include "ArduinoJson.h"

const char SetWatchTimeHtml[] = "<html><body><h1>Set time</h1><form action=\"/updatetime\" method=\"get\"><input type=\"text\" name=\"time\"><button type='submit'>Save</button></form><script>document.getElementsByName('time')[0].value = new Date().toISOString();</script></body></html>";

bool UpdateRTCTimeFromString(String value);
/*
class Notification : public SKEmitter
{
    public:
        Notification(String skPath, String state = "normal", String message = "")
        {
            this->set_sk_path(skPath);
            this->state = state;
            this->message = message;
        }

        void Update(String state, String message) 
        {
             this->state = state; 
             this->message = message; 
        }
        String as_signalK() override
        {
            auto object = jsonBuffer.createObject();
        }
    private:
        String state = "normal";
        String message = "";
        StaticJsonDocument<200> jsonBuffer;
};*/
#endif