#include "MiniSettings.h"

static MiniSettings*instance;

void MiniSettings::initialize(lv_obj_t*parent)
{
    instance = this;
    static lv_style_t popupStyle;
    lv_style_init(&popupStyle);
    lv_style_set_radius(&popupStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    //lv_style_set_bg_opa(&popupStyle, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_border_width(&popupStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_RED);
    lv_style_set_image_recolor(&popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    container = lv_cont_create(parent, NULL);
    lv_obj_add_style(container, LV_CONT_PART_MAIN, &popupStyle);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_hidden(container, true);
    //lv_cont_set_fit2(container, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_cont_set_layout(container, LV_LAYOUT_COLUMN_MID);
    auto wifiLabel = lv_label_create(container, NULL);
    lv_label_set_text(wifiLabel, "WiFi");
    wifiToggle = lv_switch_create(container, NULL);
    lv_obj_set_event_cb(wifiToggle, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            debugI("Wifi state: %d", lv_switch_get_state(obj));
            hardware->set_wifi_enabled(lv_switch_get_state(obj));            
        }
    });
    auto brightnessLabel = lv_label_create(container, NULL);

    lv_label_set_text(brightnessLabel, "Display");

    brightnessSlider = lv_slider_create(container, NULL);
    lv_obj_set_width(brightnessSlider, 200);
    lv_slider_set_range(brightnessSlider, 0, 255);

    lv_obj_set_event_cb(brightnessSlider, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            debugI("Brightness state: %d", lv_slider_get_value(obj));
            hardware->set_brightness(lv_slider_get_value(obj));
        }
    });

    closeButton = lv_btn_create(container, NULL);
    auto label = lv_label_create(closeButton, NULL);
    lv_label_set_text(label, "Save");
    lv_obj_set_event_cb(closeButton, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_CLICKED)
        {
            debugI("Closing mini settings");
            hardware->storeConfiguration();
            instance->hide();
        }
    });
}

void MiniSettings::show()
{
    auto brightness = hardware->get_brightness();
    lv_slider_set_value(brightnessSlider, brightness, LV_ANIM_OFF);
    auto wifiEnabled = hardware->get_wifi_enabled();
    if(wifiEnabled)
    {
        lv_switch_on(wifiToggle, LV_ANIM_OFF);
    }
    else
    {
        lv_switch_off(wifiToggle, LV_ANIM_OFF);
    }
    lv_obj_set_hidden(container, false);
}

void MiniSettings::hide()
{
    lv_obj_set_hidden(container, true);
}