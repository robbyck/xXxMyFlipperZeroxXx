#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexStaticFrequency,
    VarItemListIndexRemoveStaticFreq,
    VarItemListIndexAddStaticFreq,
};

void xtreme_app_scene_protocols_freqs_static_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_protocols_freqs_static_frequency_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_static_index = variable_item_get_current_value_index(item);
    uint32_t value = *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
    char text[10] = {0};
    snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
    variable_item_set_current_value_text(item, text);
}

void xtreme_app_scene_protocols_freqs_static_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Static Freq",
        FrequencyList_size(app->subghz_static_freqs),
        xtreme_app_scene_protocols_freqs_static_frequency_changed,
        app);
    app->subghz_static_index = 0;
    variable_item_set_current_value_index(item, app->subghz_static_index);
    if(FrequencyList_size(app->subghz_static_freqs)) {
        uint32_t value = *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
        char text[10] = {0};
        snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
        variable_item_set_current_value_text(item, text);
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Remove Static Freq", 0, NULL, app);

    variable_item_list_add(var_item_list, "Add Static Freq", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_protocols_freqs_static_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneProtocolsFreqsStatic));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_protocols_freqs_static_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneProtocolsFreqsStatic, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRemoveStaticFreq:
            if(!FrequencyList_size(app->subghz_static_freqs)) break;
            uint32_t value =
                *FrequencyList_get(app->subghz_static_freqs, app->subghz_static_index);
            FrequencyList_it_t it;
            FrequencyList_it(it, app->subghz_static_freqs);
            while(!FrequencyList_end_p(it)) {
                if(*FrequencyList_ref(it) == value) {
                    FrequencyList_remove(app->subghz_static_freqs, it);
                } else {
                    FrequencyList_next(it);
                }
            }
            app->save_subghz_freqs = true;
            scene_manager_previous_scene(app->scene_manager);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocolsFreqsStatic);
            break;
        case VarItemListIndexAddStaticFreq:
            scene_manager_set_scene_state(
                app->scene_manager, XtremeAppSceneProtocolsFreqsAdd, false);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocolsFreqsAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_protocols_freqs_static_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
