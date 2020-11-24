// #include "ArduinoJson.h"
// #include "main.h"
// #include "WatchHardware.h"
// #include <system/configurable.h>
// #include <vector>

// class JsonSettingsValue
// {
// public:
//     String property;
//     String title;
//     String type;
//     bool readonly;
//     void *value;
// };

// class JsonSettingsEditor
// {
// public:
//     JsonSettingsEditor(lv_obj_t *parent, Configurable *configurable)
//     {
//         this->parent = parent;
//         this->configurable = configurable;
//     }

//     ~JsonSettingsEditor()
//     {
//         lv_obj_del(menu);
//         for (auto value : values)
//         {
//             delete value;
//         }

//         buffer.clear();
//         values.clear();
//     }

//     void Load()
//     {
//         menu = lv_list_create(parent, NULL);
//         lv_obj_set_pos(menu, 0, 0);
//         lv_obj_set_size(menu, LV_HOR_RES, LV_VER_RES);
//         /*container = lv_cont_create(page, NULL);
//         lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
//         lv_cont_set_fit(container, LV_FIT_TIGHT);*/

//         buffer.clear();
//         auto &schema = this->buffer.parseObject(configurable->get_config_schema());
//         if (schema.success())
//         {
//             auto &properties = schema.get<JsonObject>("properties");

//             for (const auto &kv : properties)
//             {
//                 auto value = new JsonSettingsValue();
//                 bool append = true;
//                 JsonObject &property = kv.value.as<JsonObject>();
//                 value->title = property["title"].as<String>();
//                 value->type = property["type"].as<String>();
//                 value->readonly = property["readonly"].as<bool>();

//                 lv_list_add_btn(menu, NULL, value->title.c_str());

//                 if (value->type == "boolean")
//                 {
//                     /*value->label = NULL;
//                     value->editor = lv_checkbox_create(container, NULL);

//                     lv_checkbox_set_text(value->editor, value->title.c_str());
//                     if (value->readonly)
//                     {
//                         lv_checkbox_set_disabled(value->editor);
//                     }*/
//                 }
//                 else if (value->type == "integer" || value->type == "string" || value->type == "number")
//                 {
//                     /*if (!value->readonly)
//                     {
//                         value->label = lv_label_create(container, NULL);
//                         lv_label_set_text(value->label, value->title.c_str());

//                         value->editor = lv_textarea_create(container, NULL);
//                         lv_textarea_set_one_line(value->editor, true);

//                         if (value->type == "integer")
//                         {
//                             lv_textarea_set_accepted_chars(value->editor, "0123456789+-");
//                             lv_textarea_set_max_length(value->editor, 10);
//                         }
//                         else if (value->type == "number")
//                         {
//                             lv_textarea_set_accepted_chars(value->editor, "0123456789.+-");
//                             lv_textarea_set_max_length(value->editor, 11);
//                         }
//                     }
//                     else
//                     {
//                         value->editor = lv_label_create(container, NULL);
//                         lv_label_set_text(value->label, "readonly");
//                     }*/
//                 }
//                 else
//                 {
//                     append = false;
//                 }

//                 if (append)
//                 {

//                     values.push_back(value);
//                 }
//                 else
//                 {
//                     delete value;
//                 }
//             }

//             auto *save = lv_list_add_btn(menu, LV_SYMBOL_HOME, "Save");
//             save->user_data = this;
//             lv_obj_set_event_cb(save, [](lv_obj_t *obj, lv_event_t event) {
//                 ((JsonSettingsEditor*)obj->user_data)->save();
//             });


//             auto *back = lv_list_add_btn(menu, LV_SYMBOL_HOME, "Back");
//             back->user_data = this;
//             lv_obj_set_event_cb(back, [](lv_obj_t *obj, lv_event_t event) {
//                 ((JsonSettingsEditor*)obj->user_data)->close();
//             });

//             LoadValues();
//         }
//         else
//         {
//             auto failureLabel = lv_label_create(container, NULL);
//             lv_label_set_text_fmt(failureLabel, "Failed to parse schema for %s configurable!", configurable->config_path.c_str());
//         }
//     }

//     void LoadValues()
//     {
//         if (values.size() > 0)
//         {
//             buffer.clear();
//             auto &json = configurable->get_configuration(buffer);
//             for (auto *value : values)
//             {
//                 if (value->type == "boolean")
//                 {
//                     bool isSet = json[value->property].as<bool>();
//                     value->value = &isSet;
//                     //lv_checkbox_set_state(value->editor, isSet ? LV_BTN_STATE_CHECKED_PRESSED : LV_BTN_STATE_CHECKED_RELEASED);
//                 }
//                 else if (value->type == "integer")
//                 {
//                     int integer = json[value->property].as<int>();
//                     value->value = &integer;
//                     //lv_textarea_set_text(value->editor, String(integer).c_str());
//                 }
//                 else if (value->type == "number")
//                 {
//                     float number = json[value->property].as<float>();
//                     value->value = &number;
//                     //lv_textarea_set_text(value->editor, String(number).c_str());
//                 }
//                 else if (value->type == "string")
//                 {
//                     String text = json[value->property].as<String>();
//                     value->value = &text;
//                     //lv_textarea_set_text(value->editor, text.c_str());
//                 }
//             }
//         }
//     }


//     void close()
//     {
//         lv_obj_set_hidden(menu, true);
//     }
//     void save()
//     {
//         close();
//     }

// private:
//     lv_obj_t *menu;
//     lv_obj_t *parent;
//     lv_obj_t *page;
//     lv_obj_t *container;
//     Configurable *configurable;
//     std::vector<JsonSettingsValue *> values;
//     StaticJsonBuffer<1024> buffer;
// };