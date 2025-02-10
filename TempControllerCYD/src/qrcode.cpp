#include "TempControllerCYD_screens.h"

lv_obj_t * qr;

void lv_qrcode(String data)
{
    lv_color_t bg_color = lv_color_white();
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    qr = lv_qrcode_create(scrComm);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Set data*/
    // const char * data = "https://lvgl.io";
    const char * dataChar = data.c_str(); 
    lv_qrcode_update(qr, dataChar, strlen(dataChar));
    // lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
}