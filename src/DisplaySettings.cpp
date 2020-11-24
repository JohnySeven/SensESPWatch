#include "DisplaySettings.h"

static int selectedTimes[] = { 10000,15000,20000,30000,60000,120000,300000};
static const char*selectedTimesLabels = "10 sec\n15 sec\n\20 sec\n30 sec\n1 min\n2 min\n5 min";

void DisplaySettings::initialize(lv_obj_t*parent)
{
    container = lv_cont_create(parent, NULL);
    lv_obj_add_style(container, LV_CONT_PART_MAIN, Styles::getDefault()->popupStyle);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_hidden(container, true);
    lv_cont_set_layout(container, LV_LAYOUT_COLUMN_MID);

    auto sleepInterval = lv_label_create(container, NULL);
    lv_label_set_text(sleepInterval, "Auto sleep in (s)");
    sleepDropDown = lv_dropdown_create(container, NULL);
    lv_obj_set_width(sleepDropDown, 175);
    lv_obj_align(sleepDropDown, NULL, LV_ALIGN_CENTER, 0, 0);


    lv_dropdown_set_options(sleepDropDown, selectedTimesLabels);

    lv_obj_set_event_cb(sleepDropDown, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            auto index = lv_dropdown_get_selected(obj);
            debugI("Auto sleep timeout: selected %d", index);
            hardware->set_sleep_timeout(selectedTimes[index]);
        }
    });

    auto brightnessLabel = lv_label_create(container, NULL);
    lv_label_set_text(brightnessLabel, "Display");
    brightnessSlider = lv_slider_create(container, NULL);
    lv_obj_set_width(brightnessSlider, 175);
    lv_obj_align(brightnessSlider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(brightnessSlider, 25, 255);

    lv_obj_set_event_cb(brightnessSlider, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            debugI("Brightness state: %d", lv_slider_get_value(obj));
            hardware->set_brightness(lv_slider_get_value(obj));
        }
    });

    closeButton = lv_btn_create(container, NULL);
    closeButton->user_data = this;
    auto label = lv_label_create(closeButton, NULL);
    lv_label_set_text(label, "OK");
    lv_obj_set_event_cb(closeButton, [] (lv_obj_t * obj, lv_event_t event)
    {
        if(event == LV_EVENT_CLICKED)
        {
            debugI("Closing display settings");
            hardware->storeConfiguration();

            ((DisplaySettings*)(obj->user_data))->hide();
        }
    });
}

void DisplaySettings::show()
{
    debugI("Showing display settings...");
    auto brightness = hardware->get_brightness() / 1000;
    lv_slider_set_value(brightnessSlider, brightness, LV_ANIM_OFF);
    auto sleepTimeout = hardware->get_sleep_timeout();
    auto selectedIndex = 0;
    for (int i = 0; i < sizeof(selectedTimes); i++)
    {
        if(selectedTimes[i] == sleepTimeout)
        {
            selectedIndex = i;
            break;
        }
    }

    lv_dropdown_set_selected(sleepDropDown, selectedIndex);
    lv_obj_move_foreground(container);
    lv_obj_set_hidden(container, false);
}

void DisplaySettings::hide()
{
    lv_obj_set_hidden(container, true);
}