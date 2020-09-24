#pragma once
#include "main.h"
#include "WatchHardware.h"
#include "system/configurable.h"

class MiniSettings
{
    public:
        void initialize(lv_obj_t*parent);
        void show();
        void hide();
    private:
        lv_obj_t*container;
        lv_obj_t*wifiToggle;
        lv_obj_t*brightnessSlider;
        lv_obj_t*closeButton;
};