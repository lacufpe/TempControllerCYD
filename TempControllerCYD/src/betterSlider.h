// betterSlider.h
#ifndef BETTERSLIDER_H
#define BETTERSLIDER_H

#include <lvgl.h>

extern int temperaturaAtual,setPoint,heaterStatus,hysteresis,deltaT;

class BetterSlider {
private:
    lv_obj_t* slider;
    lv_obj_t* label;
    lv_obj_t* label_min;
    lv_obj_t* label_max;
    lv_obj_t* upButton;
    lv_obj_t* downButton;
    lv_obj_t* titleLabel;
    const char* title;
    int minValue;
    int maxValue;
    const char* unit;

public:
    BetterSlider(lv_obj_t* parent, int width, int x, int y, const char* title, const char* unit, int minVal, int maxVal);
    ~BetterSlider(); // Destructor to clean up LVGL objects

    int getValue();
    void setValue(int value);
    void enable();
    void disable();

private:
    void updateLabel();

    static void sliderEventHandler(lv_event_t* e);
    static void buttonEventHandler(lv_event_t* e);
};

#endif