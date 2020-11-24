#pragma once
#include <functional>

class GuiNav
{
private:
    std::function<void(void)> showMainScreenFunc;
    std::function<void(void)> showDeviceSettingsFunc;

public:
    static GuiNav *getDefault()
    {
        static GuiNav instance;

        return &instance;
    }

    void setup(std::function<void(void)> showMainScreen, std::function<void(void)> showDeviceSettings)
    {
        showMainScreenFunc = showMainScreen;
        showDeviceSettingsFunc = showDeviceSettings;
    }

    void showMainScreen() { showMainScreenFunc(); }
    void showDeviceSettings() { showDeviceSettingsFunc(); }
};