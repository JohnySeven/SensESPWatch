#pragma once
#include "main.h"
#include "SKValueUI.h"
#include "SKValueViewBase.h"
#include "SettingsMenu.h"
#include "vector"
#include "WatchHardware.h"
#include "GuiNavigation.h"
#include "lv_styles.h"

class Gui
{
public:
    Gui();
    void initialize(TTGOClass*watch, WatchHardware*hardware);
    void tick();
    void add(SKValueView*valueView);
    void showMiniSettings();
    void showDeviceSettings();

    void showSkData()
    {
        lv_obj_set_hidden(skRoot, false);
        lv_obj_set_hidden(mainBar, true);
        skViews.at(skIndex)->getView()->show();
    }

    void showMainBar()
    {
        lv_obj_set_hidden(skRoot, true);
        lv_obj_set_hidden(mainBar, false);
    }

    void toggleSkView()
    {
        skViews.at(skIndex)->getView()->hide();//hide current
        if(skIndex + 1 < skViews.size())
        {
            skIndex++;
        }
        else
        {
            skIndex = 0;
            showMainBar();
        }
        skViews.at(skIndex)->getView()->show();
    }
private:
    lv_obj_t *background;
    lv_obj_t *statusBar;
    lv_obj_t *mainBar;
    lv_obj_t *timeLabel;
    lv_obj_t *dateLabel;
    lv_obj_t *powerLabel;
    lv_obj_t *wifiLabel;
    lv_obj_t *skLabel;
    lv_obj_t *skRoot;
    lv_style_t activeStatusLabel;
    lv_style_t imageDisabled;
    lv_style_t inactiveStatusLabel;
    lv_style_t disabledStatusLabel;
    TTGOClass*watch;
    std::vector<SKValueView*> skViews;
    int skIndex = 0;
    bool isCharging = false;
    int chargePercent = 0;
    int updateCounter;
    int lastWifiStatus = -1;
    bool lastSkConnected = true;
    RTC_Date lastShownDate;
    WatchHardware*hardware;
    SettingsMenu*deviceSettings;
    GuiNav*nav;
};