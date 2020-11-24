#include "SettingsMenu.h"
LV_IMG_DECLARE(sk_status)

static SettingsMenu *instance;

SettingsMenu::SettingsMenu()
{
    instance = this;
    nav = nav->getDefault();
    lv_style_init(&menuButtonStyle);
    lv_style_set_pad_all(&menuButtonStyle, LV_STATE_DEFAULT, 20);
}

void SettingsMenu::newMenuButton(const void *icon, const char *text, lv_event_cb_t callback)
{
    lv_obj_t *button = lv_list_add_btn(menu, icon, text);
    lv_obj_add_style(button, LV_BTN_PART_MAIN, &menuButtonStyle);
    lv_obj_set_event_cb(button, callback);

    button->user_data = this;
}

void SettingsMenu::initialize(lv_obj_t *parent)
{
    this->parent = parent;
    menu = lv_list_create(parent, NULL);
    lv_obj_set_hidden(menu, true);
    lv_obj_set_size(menu, 200, 200);
    lv_obj_align(menu, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_event_cb(menu, [](lv_obj_t *obj, lv_event_t event) {
        debugI("Menu event %d", event);
    });

    newMenuButton(LV_SYMBOL_EYE_OPEN, "Display", [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            instance->showDisplaySettings();
        }
    });

    newMenuButton(LV_SYMBOL_SETTINGS, "Device settings", [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            auto start = millis();
            instance->openSettings(hardware);
            debugI("Open json settings took %d ms", millis() - start);
        }
    });

    newMenuButton(LV_SYMBOL_WIFI, "WiFi", [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            instance->showWifiSettings();
        }
    });

    newMenuButton(&sk_status, "Signal K", [](lv_obj_t *obj, lv_event_t event) {
        if (event == LV_EVENT_CLICKED)
        {
            auto start = millis();
            instance->openSettings(sensesp_app->get_wsclient());
            debugI("Open json settings took %d ms", millis() - start);
        }
    });

    newMenuButton(NULL, "Status", menu_event_handler);
    newMenuButton(NULL, "About", menu_event_handler);
    newMenuButton(LV_SYMBOL_CLOSE, "Close", menu_close_event_handler);

    lv_list_up(menu);
    lv_list_up(menu);
    lv_list_up(menu);
}

void SettingsMenu::showDisplaySettings()
{
    if (displaySettings == NULL)
    {
        debugI("Initializing display settings...");

        displaySettings = new DisplaySettings();
        displaySettings->initialize(parent);
    }

    displaySettings->show();
}

void SettingsMenu::showWifiSettings()
{
    if (wifiSettings == NULL)
    {
        debugI("Initializing display settings...");

        wifiSettings = new WifiSettings();
        wifiSettings->initialize(parent);
    }

    wifiSettings->show();
}

SettingsMenu::~SettingsMenu()
{
    lv_obj_del(menu);
}

void SettingsMenu::show()
{
    lv_obj_set_hidden(menu, false);
    lv_obj_move_foreground(menu);
}

void SettingsMenu::hide()
{
    lv_obj_set_hidden(menu, true);
}

void menu_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked: %s\n", lv_list_get_btn_text(obj));
    }
}

void menu_close_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        instance->hide();
    }
}

void SettingsMenu::openSettings(Configurable *configurable)
{
    return;
    /*
    if (currentEditor != NULL)
    {
        delete currentEditor;
    }

    currentEditor = new JsonSettingsEditor(NULL, configurable);
    currentEditor->Load();*/
}
