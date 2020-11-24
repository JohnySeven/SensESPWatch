#pragma once
#include "main.h"

class Styles
{
public:
    static Styles *getDefault()
    {
        static Styles styles;

        if (!styles.initialized)
        {
            styles.initialize();
        }

        return &styles;
    }
    lv_style_t *popupStyle;
    lv_style_t *greenButtonStyle;
    lv_style_t *redButtonStyle;
    lv_style_t *mainStyle;

private:
    bool initialized = false;

    void initialize()
    {
        //popup style
        popupStyle = new lv_style_t();
        lv_style_init(popupStyle);
        lv_style_set_radius(popupStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_bg_color(popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
        //lv_style_set_bg_opa(&popupStyle, LV_OBJ_PART_MAIN, LV_OPA_70);
        lv_style_set_border_width(popupStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_text_color(popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLUE);
        lv_style_set_image_recolor(popupStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
        //main style
        mainStyle = new lv_style_t();
        lv_style_init(mainStyle);
        lv_style_set_radius(mainStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_bg_color(mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
        //lv_style_set_bg_opa(&mainStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
        lv_style_set_border_width(mainStyle, LV_OBJ_PART_MAIN, 0);
        lv_style_set_text_color(mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
        //lv_style_set_image_recolor(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    }
};