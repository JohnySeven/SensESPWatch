#include "WifiSettings.h"

static WifiSettings*instance;

void WifiSettings::initialize(lv_obj_t *parent)
{
    instance = this;
    container = lv_cont_create(parent, NULL);
    lv_obj_add_style(container, LV_CONT_PART_MAIN, Styles::getDefault()->popupStyle);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_hidden(container, true);
    lv_cont_set_layout(container, LV_LAYOUT_COLUMN_MID);

    wifiSwitch = lv_switch_create(container, NULL);
    lv_obj_set_event_cb(wifiSwitch, [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_VALUE_CHANGED)
        {
            bool switchStatus = lv_switch_get_state(obj);
            hardware->set_wifi_enabled(switchStatus);
        }
    });

    closeButton = lv_btn_create(container, NULL);
    closeButton->user_data = this;
    lv_obj_set_event_cb(closeButton, [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            ((WifiSettings *)obj->user_data)->hide();
        }
    });

    
    auto closeBtnLabel = lv_label_create(closeButton, NULL);
    lv_label_set_text(closeBtnLabel, "OK");

    scanButton = lv_btn_create(container, NULL);
    scanButton->user_data = this;
    lv_obj_set_event_cb(scanButton, [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            ((WifiSettings *)obj->user_data)->scanWifi();
        }
    });

    auto scanBtnLabel = lv_label_create(scanButton, NULL);
    lv_label_set_text(scanBtnLabel, "Change");
    hide();

    wifiMenu = lv_list_create(parent, NULL);
    lv_obj_set_hidden(wifiMenu, true);
    lv_obj_set_size(wifiMenu, 200, 200);
    lv_obj_align(wifiMenu, NULL, LV_ALIGN_CENTER, 0, 0);
}

void WifiSettings::show()
{
    lv_obj_set_hidden(container, false);
    lv_obj_move_foreground(container);
    
    if (hardware->get_wifi_enabled())
    {
        lv_switch_on(wifiSwitch, LV_ANIM_OFF);
    }
    else
    {
        lv_switch_off(wifiSwitch, LV_ANIM_OFF);
    }
}

void WifiSettings::hide()
{
    lv_obj_set_hidden(container, true);
}

void WifiSettings::scanWifi()
{
    wifiList.clear();
    
    if(WiFi.getMode() != WIFI_STA)
    {
        WiFi.mode(WIFI_STA);
    }

    WiFi.disconnect(true, true);
    WiFi.onEvent([](WiFiEvent_t event)
    {
        if(event == SYSTEM_EVENT_SCAN_DONE)
        {
            auto status = WiFi.scanComplete();
            debugI("Wifi scan complete with status %d!", status);

            if(status > 0)
            {
                for(int i = 0; i < status; i++)
                {
                    instance->addWifi(WiFi.SSID(i));
                    debugI("SSID: %s", WiFi.SSID(i).c_str());
                }

                instance->presentWifiList();
            }

            WiFi.mode(WIFI_OFF);
        }
    }, SYSTEM_EVENT_SCAN_DONE);

    auto result = WiFi.scanNetworks(true);

    debugI("Scanning for Wifi networkings with result=%d", (int)result);
}

void WifiSettings::presentWifiList()
{
    lv_list_clean(wifiMenu);

    for(int i = 0; i < wifiList.size(); i++)
    {
        auto btn = lv_list_add_btn(wifiMenu, LV_SYMBOL_WIFI, wifiList.at(i).c_str());
        lv_obj_set_event_cb(btn, [](lv_obj_t*obj, lv_event_t event)
        {
            if(event == LV_EVENT_CLICKED)
            {
                auto ssid = lv_list_get_btn_text(obj);

                debugI("User selected %s", ssid);
            }
        });
    }

    lv_obj_set_hidden(wifiMenu, false);
}