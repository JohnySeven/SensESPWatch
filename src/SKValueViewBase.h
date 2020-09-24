#ifndef _SIGNALKVALUEVIEW_H
#define _SIGNALKVALUEVIEW_H

#include <sensesp_app.h>
#include <signalk/signalk_value_listener.h>
#include "SKValueUI.h"

class SKValueView
{
public:
    virtual SKValueUI *getView();
};

template <class T>
class SKValueViewBase : public SKValueView
{
protected:
    SKValueListener<T> *valueListener = NULL;
    T value;
    SKValueUI *view;

public:
    SKValueViewBase(SKValueListener<T> *listener, SKValueUI *view)
    {
        this->view = view;
        valueListener = listener;
    }

    SKValueUI *getView() override { return this->view; }
};

class SKIntView : public SKValueViewBase<int>
{
private:
    int multiplyFactor;

public:
    SKIntView(SKIntListener *intListener, SKValueUI *view, int multiply = 1) : SKValueViewBase(intListener, view)
    {
        value = 0;
        multiplyFactor = multiply;

        intListener->attach([this, intListener]() {
            auto newValue = intListener->get() * multiplyFactor;
            if (newValue != value)
            {
                value = newValue;
                this->view->updateNumber(value);
                //update UI
            }
        });
    }
};

class SKFloatView : public SKValueViewBase<float>
{
private:
    float multiplyFactor;
    uint8_t decimalPlaces;

public:
    SKFloatView(SKNumericListener *floatListener, SKValueUI *view, float multiply = 1.0f, uint8_t decimalPlaces = 1) : SKValueViewBase(floatListener, view)
    {
        value = 0.0f;
        multiplyFactor = multiply;
        this->decimalPlaces = decimalPlaces;

        floatListener->attach([this, floatListener]() {
            auto newValue = floatListener->get() * multiplyFactor;
            if (value != newValue)
            {
                value = newValue;
                this->view->updateNumber(value);
                //update UI
            }
        });
    }
};

class SKStringView : public SKValueViewBase<String>
{
public:
    SKStringView(SKStringListener *stringListener, SKValueUI *view) : SKValueViewBase(stringListener, view)
    {
        value = "--";

        stringListener->attach([this, stringListener]() {
            auto newValue = stringListener->get();
            if (!value.equals(newValue))
            {
                value = newValue;
                this->view->updateString(newValue);
            }
        });
    }
};

#endif