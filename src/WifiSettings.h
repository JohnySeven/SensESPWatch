#pragma once
#include "main.h"
#include "WatchHardware.h"
#include "system/configurable.h"
#include <functional>
#include "GuiNavigation.h"
#include "lv_styles.h"
#include <vector>

class WifiSettings
{
    public:
        void initialize(lv_obj_t*parent);
        void show();
        void hide();
        void scanWifi();
        void addWifi(String ssid) { wifiList.push_back(ssid); }
        void presentWifiList();
    private:
        lv_obj_t*container;
        lv_obj_t*wifiSwitch;
        lv_obj_t*closeButton;
        lv_obj_t*scanButton;
        lv_obj_t*wifiMenu;
        std::vector<String> wifiList;
};