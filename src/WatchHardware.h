#ifndef _WATCHHARDWARE_H
#define _WATCHHARDWARE_H

#include "main.h"
#include "system/configurable.h"

#define G_EVENT_VBUS_PLUGIN         _BV(0)
#define G_EVENT_VBUS_REMOVE         _BV(1)
#define G_EVENT_CHARGE_DONE         _BV(2)

#define G_EVENT_WIFI_SCAN_START     _BV(3)
#define G_EVENT_WIFI_SCAN_DONE      _BV(4)
#define G_EVENT_WIFI_CONNECTED      _BV(5)
#define G_EVENT_WIFI_BEGIN          _BV(6)
#define G_EVENT_WIFI_OFF            _BV(7)

#define DEFAULT_SCREEN_TIMEOUT  45*1000

#define WATCH_FLAG_SLEEP_MODE   _BV(1)
#define WATCH_FLAG_SLEEP_EXIT   _BV(2)
#define WATCH_FLAG_BMA_IRQ      _BV(3)
#define WATCH_FLAG_AXP_IRQ      _BV(4)

enum {
    Q_EVENT_WIFI_SCAN_DONE,
    Q_EVENT_WIFI_CONNECT,
    Q_EVENT_BMA_INT,
    Q_EVENT_AXP_INT,
} ;

class WatchHardware : Configurable
{
    public:
        WatchHardware();
        void Initialize(TTGOClass*watch);
        void Loop();
        bool get_ischarging() { return isCharging; }
        uint32_t get_sleep_timeout() { return sleepTimeout; }
        uint8_t get_brightness() { return brightness;}
        void set_brightness(uint8_t value) { brightness = value; }
        bool get_wifi_enabled() { return enabledWifi; }
        void set_wifi_enabled(bool value) { enabledWifi = value; wifiChangePending = true; }
        void set_sleep_timeout(unsigned int timeout) { sleepTimeout = timeout; }
        virtual JsonObject& get_configuration(JsonBuffer& buf) override final;
        virtual bool set_configuration(const JsonObject& config) override final;
        virtual String get_config_schema() override;
        void storeConfiguration()
        {
            this->save_configuration();
        }
    private:
        TTGOClass*watch;
        bool sleepMode = false;
        bool isCharging = false;
        bool vBusConnected = false;
        bool enabledWifi = false;
        bool tiltWakeup = false;
        bool doubleTapWakeup = true;
        bool wifiChangePending = false;
        uint8_t brightness = 120;
        uint8_t setBrightness = 120;
        unsigned int sleepTimeout = 30 * 1000;
        void InitAcc();
        void InitPMU();
        void LowPower();
};

extern WatchHardware*hardware;
#endif