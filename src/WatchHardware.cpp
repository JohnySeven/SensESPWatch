#include "WatchHardware.h"
#include "WiFi.h"
//#include "gui/Images/skicon.h"
#include "sensesp_app.h"
#include "net/networking.h"
#include "net/ws_client.h"

static QueueHandle_t g_event_queue_handle = NULL;
static EventGroupHandle_t g_event_group = NULL;
static EventGroupHandle_t isr_group = NULL;
WatchHardware *hardware = NULL;

WatchHardware::WatchHardware() : Configurable("/watch/hardware")
{
    hardware = this;
    load_configuration();
    debugI("Loaded HW config with brightness=%d,wifienabled=%d", brightness, enabledWifi);
    wifiChangePending = true; //update Wifi config
}

void WatchHardware::Initialize(TTGOClass *watch)
{
    setCpuFrequencyMhz(80);
    
    this->watch = watch;
    g_event_queue_handle = xQueueCreate(20, sizeof(uint8_t));
    g_event_group = xEventGroupCreate();
    isr_group = xEventGroupCreate();

    InitPMU();
    InitAcc();
    watch->motor_begin();
    watch->motor->onec();

    //clear wrong date
    watch->rtc->check();
    //Synchronize time to system time
    watch->rtc->syncToSystem();
    watch->tft->setTextFont(2);
    watch->tft->setCursor(0, TFT_HEIGHT - 20);
    watch->tft->println("SensESP Watch v0.1b");
    //watch->tft->drawXBitmap((TFT_WIDTH / 2) - (skIcon_width / 2), (TFT_HEIGHT / 2) - (skIcon_height / 2), skIcon_bits, skIcon_width, skIcon_height, watch->tft->color565(0, 51, 153));
}

void WatchHardware::Loop()
{
    bool rlst;
    uint8_t data;
    //! Fast response wake-up interrupt
    EventBits_t bits = xEventGroupGetBits(isr_group);
    if (bits & WATCH_FLAG_SLEEP_EXIT)
    {
        if (sleepMode)
        {
            sleepMode = false;
            setCpuFrequencyMhz(80);
        }

        LowPower();

        if (bits & WATCH_FLAG_BMA_IRQ)
        {
            do
            {
                rlst = watch->bma->readInterrupt();
            } while (!rlst);
            xEventGroupClearBits(isr_group, WATCH_FLAG_BMA_IRQ);
        }
        if (bits & WATCH_FLAG_AXP_IRQ)
        {
            watch->power->readIRQ();
            watch->power->clearIRQ();
            //TODO: Only accept axp power pek key short press
            xEventGroupClearBits(isr_group, WATCH_FLAG_AXP_IRQ);
        }
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_EXIT);
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_MODE);

        log_i("Woken up from sleep");
    }
    if ((bits & WATCH_FLAG_SLEEP_MODE))
    {
        //! No event processing after entering the information screen
        return;
    }

    //! Normal polling
    if (xQueueReceive(g_event_queue_handle, &data, 5 / portTICK_RATE_MS) == pdPASS)
    {
        switch (data)
        {
        case Q_EVENT_BMA_INT:
            do
            {
                rlst = watch->bma->readInterrupt();
            } while (!rlst);

            break;
        case Q_EVENT_AXP_INT:
            watch->power->readIRQ();
            if (watch->power->isVbusPlugInIRQ())
            {
                debugI("Power charging...");
                isCharging = true;
                vBusConnected = true;
            }
            if (watch->power->isVbusRemoveIRQ())
            {
                debugI("Power not charging...");
                isCharging = false;
                vBusConnected = false;
            }
            if (watch->power->isChargingDoneIRQ())
            {
                isCharging = false;
                debugI("Battery full!");
            }
            if (watch->power->isPEKShortPressIRQ())
            {
                watch->power->clearIRQ();
                debugI("Button press!");
                LowPower();
                return;
            }
            watch->power->clearIRQ();
            break;
        default:
            break;
        }
    }

    if (setBrightness != this->brightness)
    {
        watch->bl->adjust(this->brightness);
        log_i("Updating display brightness to %d", brightness);
        setBrightness = brightness;
    }

    if (wifiChangePending)
    {
        sensesp_app->get_networking()->set_offline(!enabledWifi);
        log_i("Updating WiFi offline mode to %d", enabledWifi);
        if (enabledWifi)
        {
            sensesp_app->get_wsclient()->reconnect();
        }
        wifiChangePending = false;
    }

    if (!vBusConnected && lv_disp_get_inactive_time(NULL) > sleepTimeout)
    {
        log_i("Activity timeout, entering sleep mode!");
        LowPower();
    }
    else
    {
        lv_task_handler();
    }
}

void AXP202Interupt()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    EventBits_t bits = xEventGroupGetBitsFromISR(isr_group);
    if (bits & WATCH_FLAG_SLEEP_MODE)
    {
        //! For quick wake up, use the group flag
        xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_AXP_IRQ, &xHigherPriorityTaskWoken);
    }
    else
    {
        uint8_t data = Q_EVENT_AXP_INT;
        xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
    }
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void WatchHardware::InitPMU()
{
    auto power = watch->power;
    // Turn on the IRQ used
    power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    power->enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ, AXP202_ON);
    power->clearIRQ();

    // Turn off unused power
    power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

    pinMode(AXP202_INT, INPUT);
    attachInterrupt(AXP202_INT, AXP202Interupt, FALLING);

    vBusConnected = watch->power->isVBUSPlug();
    isCharging = watch->power->isChargeing();
}

void BMA423Interupt()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    EventBits_t bits = xEventGroupGetBitsFromISR(isr_group);
    if (bits & WATCH_FLAG_SLEEP_MODE)
    {
        //! For quick wake up, use the group flag
        xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_BMA_IRQ, &xHigherPriorityTaskWoken);
    }
    else
    {
        uint8_t data = Q_EVENT_BMA_INT;
        xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
    }

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void WatchHardware::InitAcc()
{
    if (doubleTapWakeup)
    {
        auto *sensor = watch->bma;
        Acfg cfg;
        cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
        cfg.range = BMA4_ACCEL_RANGE_2G;
        cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
        cfg.perf_mode = BMA4_CONTINUOUS_MODE;
        sensor->accelConfig(cfg);
        sensor->enableAccel();

        pinMode(BMA423_INT1, INPUT);
        attachInterrupt(BMA423_INT1, BMA423Interupt, RISING);
        if (tiltWakeup)
        {
            sensor->enableFeature(BMA423_TILT, true);
            sensor->enableTiltInterrupt();
        }

        if (doubleTapWakeup)
        {
            sensor->enableFeature(BMA423_WAKEUP, true);
            sensor->enableWakeupInterrupt();
        }
    }
}

void WatchHardware::LowPower()
{
    auto *wsclient = sensesp_app->get_wsclient();
    auto *networking = sensesp_app->get_networking();

    if (watch->bl->isOn())
    {
        xEventGroupSetBits(isr_group, WATCH_FLAG_SLEEP_MODE);
        watch->closeBL();
        watch->displaySleep();
        sleepMode = true;
        if (wsclient != NULL && wsclient->is_connected())
        {
            wsclient->takeOffline();
        }

        networking->set_offline(true);
        watch->stopLvglTick();
        WiFi.mode(WIFI_OFF);
        setCpuFrequencyMhz(20);
        Serial.println("ENTER LIGHT SLEEEP MODE");
        gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
        gpio_wakeup_enable((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        esp_light_sleep_start();
    }
    else
    {
        Serial.println("WOKEN UP FROM SLEEP");
        watch->startLvglTick();
        watch->displayWakeup();
        watch->touchToMonitor();
        watch->rtc->syncToSystem();
        watch->openBL();
        watch->setBrightness(this->brightness);
        if (enabledWifi)
        {
            networking->set_offline(false);
            wsclient->reconnect();
        }
        lv_disp_trig_activity(NULL);
    }
}

JsonObject &WatchHardware::get_configuration(JsonBuffer &buf)
{
    JsonObject &root = buf.createObject();
    root["wifiEnabled"] = this->enabledWifi;
    root["sleepTimeout"] = this->sleepTimeout;
    root["brightness"] = this->brightness;

    return root;
}

static const char SCHEMA[] PROGMEM = R"~({
    "type": "object",
    "properties": {
        "wifiEnabled": { "title": "Last state of Wifi", "type": "boolean" },
        "sleepTimeout": { "title": "Inactive sleep interval", "type": "integer" },
        "brightness": { "title": "Display brightness level 0-255", "type": "integer" },
        "tiltWakeup": { "title": "Tilt watch to wakeup", "type": "boolean" },
        "doubleTapWakeup": { "title": "Double tap watch to wakeup", "type": "boolean" }
    }
  })~";

String WatchHardware::get_config_schema()
{
    return FPSTR(SCHEMA);
}

bool WatchHardware::set_configuration(const JsonObject &config)
{
    String expected[] = {"wifiEnabled", "sleepTimeout", "brightness", "tiltWakeup", "doubleTapWakeup"};
    for (auto str : expected)
    {
        if (!config.containsKey(str))
        {
            debugI(
                "Watch hardware configuration update rejected. Missing following "
                "parameter: %s",
                str.c_str());
            return false;
        }
    }

    this->enabledWifi = config["wifiEnabled"].as<bool>();
    this->wifiChangePending = true;
    this->sleepTimeout = config["sleepTimeout"].as<int>();
    this->brightness = config["brightness"].as<uint8_t>();
    this->doubleTapWakeup = config["doubleTapWakeup"].as<bool>();
    this->tiltWakeup = config["tiltWakeup"].as<bool>();

    return true;
}