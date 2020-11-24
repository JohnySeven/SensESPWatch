#include "gui.h"
#include <LilyGoWatch.h>
#include <sensesp_app.h>

LV_IMG_DECLARE(bg_default)
LV_IMG_DECLARE(sk_status)
LV_FONT_DECLARE(roboto80)

static Gui *instance;

Gui::Gui()
{
    instance = this;
    nav = nav->getDefault();
}

void Gui::add(SKValueView *valueView)
{
    skViews.push_back(valueView);
    auto view = valueView->getView();
    view->initialize(skRoot);
    view->hide();
}

void Gui::initialize(TTGOClass *watch, WatchHardware *hardware)
{
    this->watch = watch;
    this->hardware = hardware;
    lv_obj_t *screen = lv_scr_act();
    //main style
    lv_style_t *mainStyle = Styles::getDefault()->mainStyle;
    //status bar
    static lv_style_t barStyle;
    lv_style_init(&barStyle);
    lv_style_set_radius(&barStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&barStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    //lv_style_set_bg_opa(&barStyle, LV_OBJ_PART_MAIN, LV_OPA_50);
    lv_style_set_border_width(&barStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&barStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&barStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

    //background image
    //background =
    /*lv_img_create(screen, NULL);
    lv_img_set_src(background, &bg_default);
    lv_obj_set_pos(background, 0, 0);
    lv_obj_set_size(background, LV_HOR_RES, LV_VER_RES);*/
    //main bar
    mainBar = lv_cont_create(screen, NULL);

    lv_obj_set_size(mainBar, LV_HOR_RES, LV_VER_RES - 30);
    lv_obj_set_pos(mainBar, 0, 30);
    lv_obj_add_style(mainBar, LV_OBJ_PART_MAIN, mainStyle);
    lv_cont_set_layout(mainBar, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_event_cb(mainBar, [](_lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            instance->showSkData();
        }
    });

    statusBar = lv_cont_create(screen, NULL);
    lv_obj_set_size(statusBar, LV_HOR_RES, 30);
    lv_obj_add_style(statusBar, LV_OBJ_PART_MAIN, &barStyle);
    lv_cont_set_layout(statusBar, LV_LAYOUT_ROW_MID);
    lv_obj_set_click(statusBar, true);
    lv_obj_set_event_cb(statusBar, [](_lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            instance->showDeviceSettings();
        }
        else if (event == LV_EVENT_LONG_PRESSED)
        {
            debugI("Long press on status bar!");
        }
    });
    //status bar label styles - active/inactive
    lv_style_copy(&activeStatusLabel, mainStyle);
    lv_style_set_text_color(&activeStatusLabel, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_style_copy(&inactiveStatusLabel, mainStyle);
    lv_style_set_text_color(&inactiveStatusLabel, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_style_copy(&disabledStatusLabel, mainStyle);
    lv_style_set_text_color(&disabledStatusLabel, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    //lv_style_set_opa_scale(&disabledStatusLabel, LV_STATE_DEFAULT, LV_OPA_50);

    lv_style_copy(&imageDisabled, mainStyle);
    lv_style_set_image_opa(&imageDisabled, LV_STATE_DEFAULT, LV_OPA_20);
    lv_style_set_image_recolor(&imageDisabled, LV_STATE_DEFAULT, LV_COLOR_RED);

    powerLabel = lv_label_create(statusBar, NULL);
    wifiLabel = lv_label_create(statusBar, NULL);
    lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
    skLabel = lv_img_create(statusBar, NULL);
    lv_img_set_src(skLabel, &sk_status);

    //timelabel and style
    static lv_style_t timeStyle;
    lv_style_copy(&timeStyle, mainStyle);
    lv_style_set_text_font(&timeStyle, LV_STATE_DEFAULT, &roboto80);
    //lv_style_set_text_color(&timeStyle, LV_LABEL_PART_MAIN, LV_COLOR_WHITE);
    timeLabel = lv_label_create(mainBar, NULL);
    lv_obj_set_style_local_margin_top(timeLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 30);
    lv_obj_add_style(timeLabel, LV_LABEL_PART_MAIN, &timeStyle);

    static lv_style_t dateStyle;
    lv_style_copy(&dateStyle, mainStyle);
    dateLabel = lv_label_create(mainBar, NULL);
    lv_obj_add_style(dateLabel, LV_LABEL_PART_MAIN, &dateStyle);

    skRoot = lv_obj_create(screen, NULL);
    lv_obj_set_size(skRoot, LV_HOR_RES, LV_VER_RES - 30);
    lv_obj_set_pos(skRoot, 0, 30);
    lv_obj_set_hidden(skRoot, true);
    lv_obj_set_click(skRoot, true);
    lv_cont_set_layout(mainBar, LV_LAYOUT_CENTER);

    lv_obj_add_style(skRoot, LV_OBJ_PART_MAIN, mainStyle);
    lv_obj_set_event_cb(skRoot, [](_lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            instance->toggleSkView();
        }
    });

    tick();
    watch->openBL();
    watch->bl->adjust(hardware->get_brightness());
    deviceSettings = new SettingsMenu();
    deviceSettings->initialize(screen);

    GuiNav::getDefault()->setup([this]() {
        deviceSettings->hide();
        showMainBar(); },
                                [this]() {
                                    showDeviceSettings();
                                });
}

void Gui::tick()
{
    updateCounter++;
    auto rtc = watch->rtc;
    auto now = rtc->getDateTime();

    if (lastShownDate.day != now.day)
    {
        debugI("Date changed.");
        lv_label_set_text_fmt(dateLabel, "%02u.%02u.%u", now.day, now.month, now.year);
    }

    if (lastShownDate.hour != now.hour || lastShownDate.minute != now.minute)
    {
        debugI("Time changed.");
        lastShownDate = now;
        lv_label_set_text_fmt(timeLabel, "%02u:%02u", now.hour, now.minute);
    }

    //lv_obj_align(timeLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    auto power = watch->power;
    auto battery = power->getBattPercentage();
    debugI("Vbus: %f mA, Acin: %f mA, Batt discharge: %f mA", power->getVbusCurrent(), power->getAcinCurrent(), power->getBattDischargeCurrent());
    
    auto charging = hardware->get_ischarging();

    if (this->isCharging != charging || battery != chargePercent)
    {
        this->isCharging = charging;
        this->chargePercent = battery;

        debugI("Battery status changed.");
        if (hardware->get_ischarging())
        {
            lv_label_set_text(powerLabel, LV_SYMBOL_CHARGE);
        }
        else
        {
            if (battery < 10)
            {
                lv_label_set_text(powerLabel, LV_SYMBOL_BATTERY_EMPTY);
            }
            else if (battery < 25)
            {
                lv_label_set_text(powerLabel, LV_SYMBOL_BATTERY_1);
            }
            else if (battery < 50)
            {
                lv_label_set_text(powerLabel, LV_SYMBOL_BATTERY_2);
            }
            else if (battery < 75)
            {
                lv_label_set_text(powerLabel, LV_SYMBOL_BATTERY_3);
            }
            else
            {
                lv_label_set_text(powerLabel, LV_SYMBOL_BATTERY_FULL);
            }
        }
    }

    auto wifiStatus = 0;
    auto skConnected = false;
    if (hardware->get_wifi_enabled())
    {
        if (sensesp_app->isWifiConnected())
        {
            wifiStatus = 2;
        }
        else
        {
            wifiStatus = 1;
        }

        skConnected = sensesp_app->isSignalKConnected();
    }

    if (wifiStatus != lastWifiStatus)
    {
        debugI("WiFi status changed.");
        lastWifiStatus = wifiStatus;

        lv_obj_clean_style_list(wifiLabel, LV_LABEL_PART_MAIN);

        if (wifiStatus != 0)
        {
            if (wifiStatus == 2)
            {
                lv_obj_add_style(wifiLabel, LV_LABEL_PART_MAIN, &activeStatusLabel);
            }
            else
            {
                lv_obj_add_style(wifiLabel, LV_LABEL_PART_MAIN, &inactiveStatusLabel);
            }
        }
        else
        {
            lv_obj_add_style(wifiLabel, LV_LABEL_PART_MAIN, &disabledStatusLabel);
        }
    }

    if (lastSkConnected != skConnected)
    {
        debugI("SK connection status changed.");
        lastSkConnected = skConnected;
        lv_obj_set_hidden(skLabel, !skConnected);

        if(!skConnected)
        {
            //update sk views and set no data
            for(int i = 0; i < skViews.size();i++)
            {
                this->skViews.at(i)->getView()->setEmpty();
            }
        }
    }
}

void Gui::showDeviceSettings()
{
    debugI("UI: showing device settings...");
    this->deviceSettings->show();
}