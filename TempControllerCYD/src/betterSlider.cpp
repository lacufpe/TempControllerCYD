// betterSlider.cpp
#include "betterSlider.h"
#include <stdio.h> // For snprintf

BetterSlider::BetterSlider(lv_obj_t* parent, int width, int x, int y, const char* title, const char* unit, int minVal, int maxVal) :
    minValue(minVal), maxValue(maxVal), unit(unit) {

    titleLabel = lv_label_create(parent);
    lv_label_set_text(titleLabel, title);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, x, y);

    slider = lv_slider_create(parent);
    lv_obj_set_width(slider, width);
    lv_obj_align_to(slider, titleLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_slider_set_range(slider, minVal, maxVal);
    lv_obj_set_style_anim_duration(slider, 200, 0);

    label = lv_label_create(parent);
    updateLabel();
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    label_min = lv_label_create(parent);
    char minText[20];
    snprintf(minText, sizeof(minText), "%d %s", minVal, unit);
    lv_label_set_text(label_min, minText);
    lv_obj_align_to(label_min, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    label_max = lv_label_create(parent);
    char maxText[20];
    snprintf(maxText, sizeof(maxText), "%d %s", maxVal, unit);
    lv_label_set_text(label_max, maxText);
    lv_obj_align_to(label_max, slider, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    upButton = lv_button_create(parent);
    lv_obj_align_to(upButton, label, LV_ALIGN_CENTER, 40, 0);
    lv_obj_remove_flag(upButton, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_size(upButton, 20, 20);
    lv_obj_t* upButtonLabel = lv_label_create(upButton);
    lv_label_set_text(upButtonLabel, "+");
    lv_obj_center(upButtonLabel);
    lv_obj_add_event_cb(upButton, BetterSlider::buttonEventHandler, LV_EVENT_CLICKED, this);

    downButton = lv_button_create(parent);
    lv_obj_align_to(downButton, label, LV_ALIGN_CENTER, -40, 0);
    lv_obj_remove_flag(downButton, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_size(downButton, 20, 20);
    lv_obj_t* downButtonLabel = lv_label_create(downButton);
    lv_label_set_text(downButtonLabel, "-");
    lv_obj_center(downButtonLabel);
    lv_obj_add_event_cb(downButton, BetterSlider::buttonEventHandler, LV_EVENT_CLICKED, this);

    lv_obj_add_event_cb(slider, BetterSlider::sliderEventHandler, LV_EVENT_VALUE_CHANGED, this);
}

BetterSlider::~BetterSlider() {
    lv_obj_del(titleLabel);
    lv_obj_del(slider);
    lv_obj_del(label);
    lv_obj_del(label_min);
    lv_obj_del(label_max);
    lv_obj_del(upButton);
    lv_obj_del(downButton);
}


int BetterSlider::getValue() {
    return lv_slider_get_value(slider);
}

void BetterSlider::setValue(int value) {
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
    lv_slider_set_value(slider, value,LV_ANIM_ON);
    updateLabel();
}

void BetterSlider::enable() {
    lv_obj_clear_state(slider, LV_STATE_DISABLED);
    lv_obj_clear_state(upButton, LV_STATE_DISABLED);
    lv_obj_clear_state(downButton, LV_STATE_DISABLED);
}

void BetterSlider::disable() {
    lv_obj_add_state(slider, LV_STATE_DISABLED);
    lv_obj_add_state(upButton, LV_STATE_DISABLED);
    lv_obj_add_state(downButton, LV_STATE_DISABLED);
}

void BetterSlider::updateLabel() {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d %s", lv_slider_get_value(slider), unit);
    lv_label_set_text(label, buffer);
}

void BetterSlider::sliderEventHandler(lv_event_t* e) {
    BetterSlider* instance = static_cast<BetterSlider*>(e->user_data);
    instance->updateLabel();
}

void BetterSlider::buttonEventHandler(lv_event_t* e) {
    BetterSlider* instance = static_cast<BetterSlider*>(e->user_data);
    lv_obj_t* button = static_cast<lv_obj_t*>(lv_event_get_target(e));

    int increment = 0;
    if (button == instance->upButton) {
        increment = 1;
    } else if (button == instance->downButton) {
        increment = -1;
    }

    int newValue = instance->getValue() + increment;
    instance->setValue(newValue);
}