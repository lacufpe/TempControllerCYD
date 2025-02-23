#include "TempControllerCYD_screens.h"
#include "betterSlider.h"

// Global objects
TFT_eSPI tft = TFT_eSPI();
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
int x, y, z;

lv_obj_t *scrCtrl;
lv_obj_t *scrGraph;
lv_obj_t *scrComm;
lv_obj_t * sp_slider;
lv_obj_t * sp_slider_label;

static lv_style_t title_style;

lv_chart_series_t *tempSeries;
lv_chart_series_t *setpointSeries;


// Get the Touchscreen data
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(ts.tirqTouched() && ts.touched()) {
    // Get Touchscreen points
    TS_Point p = ts.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    /* Serial.print("X = ");
    Serial.print(x);
    Serial.print(" | Y = ");
    Serial.print(y);
    Serial.print(" | Pressure = ");
    Serial.print(z);
    Serial.println();*/
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

lv_obj_t * createButton(lv_obj_t * screen, const char* label, int x0, int y0, CallbackType callback, bool btnEnabled){
  lv_obj_t * btn_label;
  // Create a Button to change to the Graph screen
  // lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_t * btn = lv_button_create(screen);
  lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, NULL);
  lv_obj_set_pos(btn, x0, y0);
  lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

  btn_label = lv_label_create(btn);
  lv_label_set_text(btn_label, label);
  lv_obj_center(btn_label);

  if(!btnEnabled){
    lv_obj_add_state(btn, LV_STATE_DISABLED);
  }
  return btn;
}

int lastTimeScreenUpdated;
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

static lv_style_t screen_style;

void initScreen(){
  // Start the tft in black
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);

  lv_init();
  lv_style_init(&screen_style);
  lv_style_set_bg_color(&screen_style,lv_color_white());
  // Start the SPI for the touch screen and init the TS library
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(2);
  
  // Create a display object
  lv_display_t * disp;
  // Initialize the TFT display using the TFT_eSPI library
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

  lv_style_init(&title_style);
  lv_style_set_text_font(&title_style, &lv_font_montserrat_30);
  lv_style_set_text_color(&title_style,lv_color_black());
  
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Create the 3 screens and the menu
  createCtrlScreen();
  createGraphScreen();
  createCommScreen();
  
  lastTimeScreenUpdated = millis();
}

void createTabs(int screen){
  switch (screen)
  {
  case 0:
    createButton(scrCtrl, "Setup", 0, 0, NULL, false);
    createButton(scrCtrl, "Graph", 0, 32, event_handler_btnGraph, true);
    createButton(scrCtrl, "Comm", 0, 64, event_handler_btnComm, true);
    break;
  case 1:
    createButton(scrGraph, "Setup", 0, 0, event_handler_btnCtrl, true);
    createButton(scrGraph, "Graph", 0, 32, NULL, false);
    createButton(scrGraph, "Comm", 0, 64, event_handler_btnComm, true);
    break;
  case 2:
    createButton(scrComm, "Setup", 0, 0, event_handler_btnCtrl, true);
    createButton(scrComm, "Graph", 0, 32, event_handler_btnGraph, true);
    createButton(scrComm, "Comm", 0, 64, NULL, false);
    break;
  default:
    break;
  }
}

void createTitle(lv_obj_t* screen, const char* title){
  // Screen title aligned center on top
  lv_obj_t * title_label = lv_label_create(screen);
  // lv_label_set_long_mode(text_label, LV_LABEL_LONG_WRAP);    // Breaks the long lines
  lv_label_set_text(title_label, title);
  lv_obj_set_width(title_label, 150);    // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -100);
  lv_obj_add_style(title_label, &title_style, 0); 
}

lv_obj_t * heaterSymbolCtrl;
lv_obj_t * heaterSymbolGraph;

lv_style_t onStyle;
lv_style_t offStyle;


void heaterOn(bool on) {
  if (heaterSymbolCtrl == nullptr) {
    Serial.println("ERROR: chargeSymbol is NULL!");
    return; // Or handle the error appropriately
  }
  
    if (on) {
      lv_obj_remove_style(heaterSymbolCtrl, &offStyle,LV_STATE_ANY|LV_PART_ANY);
      lv_obj_add_style(heaterSymbolCtrl, &onStyle, 0);      // Add on style
  } else {
      lv_obj_replace_style(heaterSymbolCtrl, &onStyle, &offStyle, LV_STATE_ANY|LV_PART_ANY);      // Add on style
  }
}

void createHeaterSymbol(lv_obj_t * screen, lv_obj_t * heaterSymbol) {
  heaterSymbol = lv_label_create(screen); // Or whatever parent object
  lv_label_set_text(heaterSymbol, LV_SYMBOL_CHARGE);
  lv_obj_set_pos(heaterSymbol,30,160);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_montserrat_48); // Example: Montserrat font size 24
  lv_obj_add_style(heaterSymbol, &style, 0); // 0 means apply to all parts of the object

  lv_style_init(&onStyle);
  lv_style_set_text_color(&onStyle, lv_color_make(255,0,0)); // Red

  lv_style_init(&offStyle);
  lv_style_set_text_color(&offStyle, lv_color_make(0,0,255)); // Blue
  
  lv_obj_add_style(heaterSymbol, &offStyle, 0);

// heaterOn(false);
}

void createCtrlScreen(){
  scrCtrl = lv_obj_create(NULL);
  lv_obj_add_style(scrCtrl, &screen_style, 0);
  createTitle(scrCtrl,"Setup");
  createTabs(0);
  // lv_obj_t * graph_btn_label;
  // // Create a Button to change to the Graph screen
  // // lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  // lv_obj_t * graphBtn = lv_button_create(scrCtrl);
  // lv_obj_add_event_cb(graphBtn, event_handler_btn1, LV_EVENT_CLICKED, NULL);
  // lv_obj_set_pos(graphBtn, 0, 0);
  // lv_obj_remove_flag(graphBtn, LV_OBJ_FLAG_PRESS_LOCK);

  // graph_btn_label = lv_label_create(graphBtn);
  // lv_label_set_text(graph_btn_label, "Graph");
  // lv_obj_center(graph_btn_label);

  // Create a Toggle button (btn2)
  // lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  
  //  BetterSlider(lv_obj_t* parent, int width, int x, int y, const char* title, const char* unit, int minVal, int maxVal);
  // Serial.println("Antes dos Sliders");
  BetterSlider* setPoint = new BetterSlider(scrCtrl, 200, 100, 30, "Setpoint", "°C", 0, 100);
  // Serial.println("1 Slider");
  BetterSlider* hysteresis = new BetterSlider(scrCtrl, 200, 100, 100, "Histerese", "°C", 1, 10);
  // Serial.println("2 Sliders");
  BetterSlider* deltaT = new BetterSlider(scrCtrl, 200, 100, 170, "deltaT", "s", 1, 120);
  // Serial.println("3 Sliders");

  // Create Experiment control interface
  createButton(scrCtrl,LV_SYMBOL_PLAY, 5,110,event_handler_btnGraph,true);
  createButton(scrCtrl,LV_SYMBOL_STOP, 45,110,event_handler_btnGraph,false);

  createHeaterSymbol(scrCtrl,heaterSymbolCtrl);
  
  lv_scr_load(scrCtrl);
   
}

void create_scatter_plot(void) {
    /* Create a chart object */
    lv_obj_t* chart = lv_chart_create(scrGraph);
    lv_obj_set_size(chart, 200, 160); /* Set size */
    lv_obj_align(chart, LV_ALIGN_CENTER, 36, 20); /* Center the chart */

    /* Set type of the chart */
    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);

    /* Set the number of points */
    lv_chart_set_point_count(chart, POINT_COUNT);

    /* Set the range of the chart */
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 1000);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

    /* Create a style for the series (optional) */
    static lv_style_t series_style;
    lv_style_init(&series_style);
    lv_style_set_line_width(&series_style, 2);
    // lv_style_set_ (&series_style, 4); // Adjust point size
    // lv_style_set_point_color(&series_style, lv_palette_main(LV_PALETTE_RED)); // Example color
    lv_style_set_line_color(&series_style, lv_palette_main(LV_PALETTE_RED));   // Line color
    
    /* Add a series */
    tempSeries = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED),LV_CHART_AXIS_PRIMARY_Y);

    /* Create a style for the second series (optional) */
    static lv_style_t series2_style;
    lv_style_init(&series2_style);
    lv_style_set_line_width(&series2_style, 2);
    // lv_style_set_point_size(&series2_style, 4);
    // lv_style_set_point_color(&series2_style, lv_palette_main(LV_PALETTE_BLUE));  // Another color
    lv_style_set_line_color(&series2_style, lv_palette_main(LV_PALETTE_BLUE));    // Line color

    /* Add the second series */
    setpointSeries = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE),LV_CHART_AXIS_PRIMARY_Y);


    // /* Generate example data (you'll replace this with your actual data) */
    // lv_coord_t y_values1[POINT_COUNT];
    // lv_coord_t y_values2[POINT_COUNT];

    // for (int i = 0; i < POINT_COUNT; i++) {
    //   // Example: Sine wave for series 1
    //     y_values1[i] = 50 + 40 * sinf(2 * PI * i / POINT_COUNT);
        
    //     // Example: Cosine wave for series 2
    //     y_values2[i] = 50 + 40 * cosf(2 * PI * i / POINT_COUNT);
    // }

    lv_chart_set_ext_x_array(chart,tempSeries,timeValues);
    lv_chart_set_ext_x_array(chart,setpointSeries,timeValues);
    lv_chart_set_ext_y_array(chart,tempSeries,temperatureValues);
    lv_chart_set_ext_y_array(chart, setpointSeries, setpointValues);

    /* Set the update mode to SHIFT */
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

    /* Optionally, set the x axis division count */
    lv_chart_set_div_line_count(chart, 10, 5); // 10 vertical lines, 5 horizontal lines

}

void createGraphScreen(){
  scrGraph = lv_obj_create(NULL);
  lv_obj_add_style(scrGraph, &screen_style, 0);
  createTitle(scrGraph,"Graph");
  createTabs(1);
  create_scatter_plot();

  // Create Experiment control interface
  createButton(scrGraph,LV_SYMBOL_PLAY, 5,110,event_handler_btnGraph,true);
  createButton(scrGraph,LV_SYMBOL_STOP, 45,110,event_handler_btnGraph,false);

  createHeaterSymbol(scrGraph,heaterSymbolGraph);
  heaterOn(true);
  
  lv_obj_refresh_style(heaterSymbolCtrl,LV_PART_ANY,LV_STYLE_PROP_ANY);

}

lv_obj_t * serverButton;
lv_obj_t * mainPageButton;
lv_obj_t * setupPageButton;

void event_handler_btnServerQr(lv_event_t * e){
  lv_qrcode(String("WIFI:T:WPA;S:TempController;P:12345678;;"));
  lv_obj_set_pos(qr,80,40);
  lv_obj_add_state(serverButton, LV_STATE_DISABLED);
  lv_obj_clear_state(mainPageButton, LV_STATE_DISABLED);
  lv_obj_clear_state(setupPageButton, LV_STATE_DISABLED);
}

lv_obj_t * swNetwork;

static void event_handler_swNetwork(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t *)lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_UNUSED(obj);
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

void event_handler_btnMainPageQr(lv_event_t * e){
  IPAddress ip;
  if (WiFi.status() == WL_CONNECTED) {
    ip = WiFi.localIP();
  } else if (WiFi.getMode() & WIFI_AP) { // Check if SoftAP is enabled
    ip = WiFi.softAPIP();
  }
  String url = "http://" + ip.toString() + ":80/";
  lv_qrcode(url);
  lv_obj_set_pos(qr,80,40);
  lv_obj_clear_state(serverButton, LV_STATE_DISABLED);
  lv_obj_add_state(mainPageButton, LV_STATE_DISABLED);
  lv_obj_clear_state(setupPageButton, LV_STATE_DISABLED);    
}

void event_handler_btnSetupPageQr(lv_event_t * e){
  String ipStr = WiFi.softAPIP().toString();
  String url = "http://" + ipStr + ":80/setup.html";
  lv_qrcode(url);
  lv_obj_set_pos(qr,80,40);
  lv_obj_clear_state(serverButton, LV_STATE_DISABLED);
  lv_obj_clear_state(mainPageButton, LV_STATE_DISABLED);
  lv_obj_add_state(setupPageButton, LV_STATE_DISABLED);
}

void createCommScreen(){
  scrComm = lv_obj_create(NULL);
  lv_obj_add_style(scrComm, &screen_style, 0);
  createTitle(scrComm,"Comm");
  createTabs(2);

  serverButton = createButton(scrComm, "Server", 240, 0, event_handler_btnServerQr, false);
  mainPageButton = createButton(scrComm, "Main page", 240, 32, event_handler_btnMainPageQr, true);
  setupPageButton = createButton(scrComm, "Setup page", 240, 64, event_handler_btnSetupPageQr, true);
  lv_obj_set_size(serverButton,80,32);
  lv_obj_set_size(mainPageButton,80,32);
  lv_obj_set_size(setupPageButton,80,32);


  lv_qrcode("WIFI:T:WPA;S:TempController;P:12345678;;");
  lv_obj_set_pos(qr,80,40);
  
  swNetwork = lv_switch_create(scrComm);
  lv_obj_add_event_cb(swNetwork, event_handler_swNetwork, LV_EVENT_ALL, NULL);
  lv_obj_align_to(swNetwork,qr,LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  lv_obj_t * labelNetAP = lv_label_create(scrComm);
  lv_label_set_text(labelNetAP,"AP");
  lv_obj_align_to(labelNetAP,swNetwork,LV_ALIGN_OUT_LEFT_MID,0,0);

  lv_obj_t * labelNetLocal = lv_label_create(scrComm);
  lv_label_set_text(labelNetLocal,"Local Net");
  lv_obj_align_to(labelNetLocal,swNetwork,LV_ALIGN_OUT_RIGHT_MID,0,0);

  // lv_obj_add_state(sw, LV_STATE_DISABLED);

}

void updateScreen(){
  int now = millis();
  int deltaT = now - lastTimeScreenUpdated;
  if (deltaT >= updateScreenTime){
    lv_task_handler();  // let the GUI do its work
    lv_tick_inc(deltaT);     // tell LVGL how much time has passed
    lastTimeScreenUpdated = now;
    // Serial.println(now);
  }
}

// Callbacks that are triggered when menu btns are clicked
static void event_handler_btnCtrl(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    lv_scr_load(scrCtrl);
  }
}
static void event_handler_btnGraph(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    lv_scr_load(scrGraph);
  }
}
static void event_handler_btnComm(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    lv_scr_load(scrComm);
  }
}

static void event_handler_sp_slider(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t*) lv_event_get_target(e);
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d% °C", (int)lv_slider_get_value(slider));
  lv_label_set_text(sp_slider_label, buf);
  lv_obj_align_to(sp_slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  LV_LOG_USER("Slider changed to %d%%", (int)lv_slider_get_value(slider));
}

// Callback that is triggered when spUpbtn or spDownButton is clicked
static void event_handler_spUpDownBtn(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  
  if(code == LV_EVENT_CLICKED) {
    int value = lv_slider_get_value(sp_slider);
    Serial.println(*(int *) lv_event_get_user_data(e));
    Serial.print("value was ");
    Serial.print(value);
    Serial.print(", and now is ");
    value = (*(int *)(lv_event_get_user_data(e)) == 1)?value+1:value-1;
    Serial.print(value);
    Serial.println(".");
    lv_slider_set_value(sp_slider,value,LV_ANIM_ON);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d% °C", value);
    lv_label_set_text(sp_slider_label, buf);
    lv_obj_align_to(sp_slider_label, sp_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    LV_LOG_USER("Slider changed to %d%%", value);
  }
}