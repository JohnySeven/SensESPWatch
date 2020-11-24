#pragma once
#include "main.h"

class SKValueUI
{
    public:
        virtual void updateString(String value);
        virtual void updateNumber(int32_t number);
        virtual void updateNumber(float number);
        virtual void initialize(lv_obj_t*parent);
        virtual void show();
        virtual void hide();
        virtual void setEmpty();
};