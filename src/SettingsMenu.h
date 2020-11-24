#pragma once
#include "main.h"
#include "WatchHardware.h"
#include <system/configurable.h>
#include "GuiNavigation.h"
#include "JsonSettingsEditor.h"
#include "sensesp_app.h"
#include "DisplaySettings.h"
#include "WifiSettings.h"

class SettingsMenu
{
    public:
        SettingsMenu();
        ~SettingsMenu();
        void show();
        void hide();
        void initialize(lv_obj_t*parent);
        void openSettings(Configurable*configurable);
        void newMenuButton(const void*icon, const char*text, lv_event_cb_t callback);
        void showDisplaySettings();
        void showWifiSettings();
    private:
        lv_obj_t*menu;
        lv_obj_t*parent;
        lv_style_t menuButtonStyle;
        GuiNav*nav;
        //JsonSettingsEditor*currentEditor = NULL;
        DisplaySettings*displaySettings = NULL;
        WifiSettings*wifiSettings = NULL;
};

void menu_event_handler(lv_obj_t * obj, lv_event_t event);
void menu_close_event_handler(lv_obj_t * obj, lv_event_t event);