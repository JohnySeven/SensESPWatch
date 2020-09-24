#pragma once
#include "SKValueUI.h"
LV_FONT_DECLARE(roboto80)
LV_FONT_DECLARE(roboto40)
LV_IMG_DECLARE(signalk_x64)

class SKTextView : public SKValueUI
{
private:
    lv_obj_t *container;
    lv_obj_t *titleLabel;
    lv_obj_t *valueLabel;
    lv_obj_t *unitLabel;
    lv_obj_t *led;
    char *title;
    char *units;
public:
    SKTextView(char*title,char*units)
    {
        this->title = title;
        this->units = units;
    }

    void initialize(lv_obj_t *parent) override
    {
        static lv_style_t valueStyle;
        lv_style_init(&valueStyle);
        lv_style_set_text_font(&valueStyle, LV_STATE_DEFAULT, &roboto80);
        static lv_style_t titleStyle;
        lv_style_init(&titleStyle);
        lv_style_set_text_font(&titleStyle, LV_STATE_DEFAULT, &roboto40);
        static lv_style_t mainStyle;
        lv_style_init(&mainStyle);
        lv_style_set_radius(&mainStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_bg_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
        //lv_style_set_bg_opa(&mainStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
        lv_style_set_border_width(&mainStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_text_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
        container = lv_cont_create(parent, NULL);
        lv_obj_set_click(container, false);
        lv_cont_set_layout(container, LV_LAYOUT_COLUMN_MID);
        lv_obj_set_pos(container, 0, 0);
        lv_obj_set_size(container, lv_obj_get_width(parent), lv_obj_get_height(parent));
        lv_obj_add_style(container, LV_OBJ_PART_MAIN, &mainStyle);
        titleLabel = lv_label_create(container, NULL);
        lv_label_set_text(titleLabel, title);
        valueLabel = lv_label_create(container, NULL);
        unitLabel = lv_label_create(container, NULL);
        lv_obj_add_style(valueLabel, LV_LABEL_PART_MAIN, &valueStyle);
        lv_label_set_text(valueLabel, "--");
        lv_obj_add_style(titleLabel, LV_LABEL_PART_MAIN, &titleStyle);
        lv_label_set_text(unitLabel, units);

        led = lv_led_create(container, NULL);
        lv_led_off(led);
        lv_obj_set_size(led, 10,10);
        lv_obj_align(led, NULL, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }

    void updateString(String value) override
    {
        lv_label_set_text(valueLabel, value.c_str());
        toggleLED();
    }
    void updateNumber(int32_t number) override
    {
        lv_label_set_text(valueLabel, String(number).c_str());
        toggleLED();
    }
    void updateNumber(float number) override
    {
        lv_label_set_text(valueLabel, String(number).c_str());
        toggleLED();
    }

    void toggleLED()
    {
        lv_led_toggle(led);
    }

    void show() override
    {
        lv_obj_set_hidden(container, false);
    }

    void hide() override
    {
        lv_obj_set_hidden(container, true);
    }
};