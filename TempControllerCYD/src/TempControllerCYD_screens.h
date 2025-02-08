// screens.h
#pragma once
#ifndef SCREENS_H  // If MY_HEADER_H is NOT defined...
#define SCREENS_H  // Define it now to "mark" this file as included

// Include libraries headers
#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// Typedef for callbacks
typedef void (*CallbackType)(lv_event_t * e);

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Global objects
extern TFT_eSPI tft;
extern SPIClass mySpi;
extern XPT2046_Touchscreen ts;

// Touchscreen coordinates: (x, y) and pressure (z)
extern int x, y, z;

#define updateScreenTime 5 
// int lastTimeScreenUpdated;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

#define POINT_COUNT 100
// uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// Define the screens
extern lv_obj_t *scrCtrl;
extern lv_obj_t *scrGraph;
extern lv_obj_t *scrComm;

extern lv_chart_series_t *tempSeries;
extern lv_chart_series_t *setpointSeries;

extern int timeValues[400];
extern int setpointValues[400];
extern int temperatureValues[400];
extern int setpoint;

extern lv_image_dsc_t qrcode_dsc;
extern uint8_t qrcode_map[];
extern void invertImage(uint8_t* data, int len);


// Function declarations
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data);
void initScreen();
void createCtrlScreen();
void createGraphScreen();
void createCommScreen();
void updateScreen();
void createButton(lv_obj_t * screen, String label, int x0, int y0, CallbackType callback, bool btnEnabled);

// Callbacks that is triggered when btn1 is clicked
static void event_handler_btnCtrl(lv_event_t * e);
static void event_handler_btnGraph(lv_event_t * e);
static void event_handler_btnComm(lv_event_t * e);

// Slider callbacks
extern lv_obj_t * sp_slider;
extern lv_obj_t * sp_slider_label;
static void event_handler_sp_slider(lv_event_t * e);
static void event_handler_spUpDownBtn(lv_event_t * e);

#endif

// AMDG