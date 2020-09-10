#ifndef _WATCHHARDWARE_H
#define _WATCHHARDWARE_H

#include "main.h"
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

static QueueHandle_t g_event_queue_handle = NULL;
static EventGroupHandle_t g_event_group = NULL;
static EventGroupHandle_t isr_group = NULL;

class WatchHardware
{
    public:
        void Initialize(TTGOClass*watch);
        void Loop();
        void ResetSleepTimeout() { uptime = millis(); }
    private:
        TTGOClass*watch;
        bool sleepMode = false;
        unsigned long uptime = 0;
        void InitAcc();
        void InitPMU();
        void LowPower();
};
#endif