#include <Arduino.h>
#include "main.h"
#include "EEPROM.h"
#include "sensesp_app.h"
#include "sensors/analog_input.h"
#include "transforms/linear.h"
#include "signalk/signalk_output.h"
#include "signalk/signalk_listener.h"
#include "signalk/signalk_value_listener.h"
#include "sensors/digital_output.h"
#include "transforms/threshold.h"
#include "signalk/signalk_value_listener.h"
#include "sensesp_app_builder.h"
#include "WatchHardware.h"
#include "gui.h"
#include "SKTextView.h"

Gui *gui;
TTGOClass *watch;
SKNumericListener *speedValue;
SKNumericListener *depthValue;

// SensESP builds upon the ReactESP framework. Every ReactESP application
// defines an "app" object vs defining a "main()" method.
ReactESP app([]() {

// Some initialization boilerplate when in debug mode...
#ifndef SERIAL_DEBUG_DISABLED
       Serial.begin(115200);

       // A small arbitrary delay is required to let the
       // serial port catch up
       delay(100);
       Debug.setSerialEnabled(true);
#endif
       watch = TTGOClass::getWatch();
       watch->begin();
       watch->lvgl_begin();
       //gui = new WatchGui(watch);
       // Create the global SensESPApp() object.
       auto *builder = (new SensESPAppBuilder())
                           ->set_standard_sensors(NONE)
                           ->set_led_blinker(false, 0, 0, 0)
                           ->set_hostname("watch")
                           ->set_sk_server("pi.boat", 3000) //Local net IP
                           ->set_wifi("DryII", "wifi4boat");

       sensesp_app = builder->get_app();
       
       hardware = new WatchHardware();
       hardware->Initialize(watch);
       gui = new Gui();

       app.onRepeat(1000, []() {
              gui->tick();
       });

       /*hardware->setGuiTick([](){
       
              gui->tick();
       });*/

       app.onRepeat(5, []() {
              hardware->Loop();
       });


       if (!hardware->get_wifi_enabled())
       {
              sensesp_app->get_networking()->set_offline(false);
       }

       sensesp_app->add_http_handler("/watch", HTTP_GET, [](AsyncWebServerRequest *request) {
              request->send(200, "text/html", String(SetWatchTimeHtml));
       });

       sensesp_app->add_http_handler("/updatetime", HTTP_GET, [](AsyncWebServerRequest *request) {
              if (request->hasArg("time"))
              {
                     if (UpdateRTCTimeFromString(request->arg("time")))
                     {
                            watch->rtc->syncToSystem();
                            request->send(200, "text/plain", "OK");
                     }
                     else
                     {
                            request->send(200, "text/plain", "Wrong time format!");
                     }
              }
              else
              {
                     request->send(400, "text/plain", "Time argument must be present!");
              }
       });

       // Start the SensESP application running
       gui->initialize(watch, hardware);

       speedValue = new SKNumericListener("navigation.speedOverGround", 1000);
       depthValue = new SKNumericListener("environment.depth.belowTransducer", 1000);

       gui->add(new SKFloatView(speedValue, new SKTextView("Speed", "Km/h", 0), 3.6f));
       gui->add(new SKFloatView(depthValue, new SKTextView("Depth", "m", 1), 1.0f));
       gui->add(new SKFloatView(new SKNumericListener("navigation.headingTrue", 1000), new SKTextView("Heading", "deg", 0), 57.2957795f));
       gui->add(new SKFloatView(new SKNumericListener("electrical.batteries.rpi.voltage", 5000), new SKTextView("Voltage", "V", 1), 1.0f));

       sensesp_app->get_networking()->set_useWifiManager(false);
       sensesp_app->enable();
});

bool UpdateRTCTimeFromString(String time)
{
       bool ret = false;
       char timeCh[30];
       const char pattern[] = ":T-.";
       time.toCharArray(timeCh, 30);

       //2020-09-15T07:56:44.225Z
       auto *ptr = strtok(timeCh, pattern);
       if (ptr != NULL)
       {
              auto year = atoi(ptr);
              ptr = strtok(NULL, pattern);
              if (ptr != NULL)
              {
                     auto month = atoi(ptr);
                     ptr = strtok(NULL, pattern);

                     if (ptr != NULL)
                     {
                            auto day = atoi(ptr);
                            ptr = strtok(NULL, pattern);

                            if (ptr != NULL)
                            {
                                   auto hour = atoi(ptr);
                                   ptr = strtok(NULL, pattern);

                                   if (ptr != NULL)
                                   {
                                          auto minute = atoi(ptr);
                                          ptr = strtok(NULL, pattern);
                                          if (ptr != NULL)
                                          {
                                                 auto second = atoi(ptr);

                                                 if (year > 2000 && month > 0 && month < 13 && day > 1 && day < 32 && hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59)
                                                 {
                                                        watch->rtc->setDateTime(year, month, day, hour, minute, second);
                                                        debugI("Watch internal clock has been updated to %d-%d-%d %d:%d:%d", year, month, day, hour, minute, second);
                                                        ret = true;
                                                 }
                                          }
                                   }
                            }
                     }
              }
       }

       return ret;
}