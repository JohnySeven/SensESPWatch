#pragma once
#include "main.h"
#include "WatchHardware.h"
#include "system/configurable.h"
#include <functional>
#include "GuiNavigation.h"
#include "lv_styles.h"

class DisplaySettings
{
    public:
        void initialize(lv_obj_t*parent);
        void show();
        void hide();
    private:
        lv_obj_t*container;
        lv_obj_t*brightnessSlider;
        lv_obj_t*sleepDropDown;
        lv_obj_t*closeButton;
};