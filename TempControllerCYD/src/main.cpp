// #include <Arduino.h>
#include "TempControllerCYD_screens.h"
#include <DS18B20.h>

// Objects for the touchscreen
// TFT_eSPI tft = TFT_eSPI();
// SPIClass mySpi = SPIClass(VSPI);
// XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

//DS18B20 ds(27);


// uint8_t address[] = {40, 161, 142, 248, 19, 25, 1, 223};

// GraphWidget gr = GraphWidget(&tft);    // Graph widget gr instance with pointer to tft
// TraceWidget tr = TraceWidget(&gr);     // Graph trace tr with pointer to gr

// const float gxLow  = 0.0;
// const float gxHigh = 100.0;
// const float gyLow  = 0.0;
// const float gyHigh = 40.0;


// #define LOW_ALARM 20
// #define HIGH_ALARM 25

// DS18B20 ds(27);
// uint8_t address[] = {40, 161, 142, 248, 19, 25, 1, 223};
// uint8_t selected;

// float temp;

// #define XPT2046_IRQ 36
// #define XPT2046_MOSI 32
// #define XPT2046_MISO 39
// #define XPT2046_CLK 25
// #define XPT2046_CS 33


// TFT_eSPI_Button menu[3];
// char* menuStr[3] = {"Setup","Graph","Comm"};
// int screen = 0;  // 0 - Setup
//                  // 1 - Graph
//                  // 2 - Comm
// int oldScreen = screen;

// void drawButtons() {
//   uint16_t bWidth = TFT_HEIGHT/6;
//   uint16_t bHeight = TFT_WIDTH/4;
//   // Generate buttons with different size X deltas
//   for (int i = 0; i < 3; i++) {
//     menu[i].initButton(&tft,
//                       bWidth * 5  + bWidth/2,
//                       bHeight * i + bHeight/2,
//                       bWidth,
//                       bHeight,
//                       TFT_GREEN, // Outline
//                       TFT_DARKGREEN, // Fill
//                       TFT_WHITE, // Text
//                       menuStr[i],
//                       1);

//     menu[i].drawButton(screen==i?true:false, menuStr[i]);
//   }
// }

// void changeScreen() {

//   oldScreen = screen;
// }

// void drawSetup(){
//   tft.drawString("Setup",10,10);
// }

// void drawGraph(){
//   static uint32_t plotTime = millis();
//   static float gx = 0.0, gy = 0.0;
//   static float delta = 10.0;

//   // Create a new plot point every 100ms
//   if (millis() - plotTime >= 100) {
//     plotTime = millis();
    
//     if (selected) {
//       temp = float(ds.getTempC());
//       Serial.println(temp);
//     } 

//     // Add a plot, first point in a trace will be a single pixel (if within graph area)
//     tr.addPoint(gx, temp);
//     gx += 1.0;

//     if (gx > gxHigh) {
//       gx = 0.0;
//       gy = 0.0;

//       // Draw empty graph at 40,10 on display to clear old one
//       gr.drawGraph(40, 110);
//       // Start new trace
//       tr.startTrace(TFT_GREEN);
//     }
//   }    
// }

// void drawComm(){
//     tft.drawString("Communication",10,10);
// }

// // void printTouchToDisplay(TS_Point p) {

// //   // Clear screen first
// //   //tft.fillScreen(TFT_BLACK);
// //   tft.setTextColor(TFT_WHITE, TFT_BLACK);

// //   int x = 320 / 2; // center of display
// //   int y = 50;
// //   int fontSize = 2;

// //   String temp = "Pressure = " + String(p.z);
// //   tft.drawCentreString(temp, x, y, fontSize);

// //   y += 16;
// //   temp = "X = " + String(p.x);
// //   tft.drawCentreString(temp, x, y, fontSize);

// //   y += 16;
// //   temp = "Y = " + String(p.y);
// //   tft.drawCentreString(temp, x, y, fontSize);
// // }

int tempo[100];
int setpointValues[100];
int temperatureValues[100];

void setup() {
  Serial.begin(115200);
  initScreen();
  for(int i =0;i<100;i++){
    tempo[i] = 10*i;
    setpointValues[i] = 50 + 40 * sinf(2 * PI * i / 50);
    temperatureValues[i] = 50 + 40 * cosf(2 * PI * i / 100);
  }
}

void loop(){
  updateScreen();
}

//   selected = ds.select(address);

//   if (selected) {
//     ds.setAlarms(LOW_ALARM, HIGH_ALARM);
//   } else {
//     Serial.println("Device not found!");
//   }

//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);

//   // Start the SPI for the touch screen and init the TS library
//   mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
//   ts.begin(mySpi);
//   ts.setRotation(1);
  
//   // Graph area is 200 pixels wide, 150 pixels high, dark grey background
//   gr.createGraph(200, 100, tft.color565(5, 5, 5));

//   // x scale units is from 0 to 100, y scale units is -512 to 512
//   gr.setGraphScale(gxLow, gxHigh, gyLow, gyHigh);

//   // X grid starts at 0 with lines every 20 x-scale units
//   // Y grid starts at -512 with lines every 64 y-scale units
//   // blue grid
//   gr.setGraphGrid(gxLow, 20.0, gyLow, 10.0, TFT_BLUE);

//   // Draw empty graph, top left corner at pixel coordinate 40,10 on TFT
//   gr.drawGraph(40, 110);

//   // Start a trace with using red, trace points are in x and y scale units
//   // In this example a horizontal line is drawn
//   tr.startTrace(TFT_RED);
//   // Add a trace point at 0.0,0.0 on graph
//   tr.addPoint(0.0, 0.0);
//   // Add another point at 100.0, 0.0 this will be joined via line to the last point added
//   tr.addPoint(40.0, 0.0);

//   // Start a new trace with using white
//   tr.startTrace(TFT_WHITE);

//   // Start buttons
//   drawButtons();
// }

// void loop() {
//   if(screen != oldScreen){
//     changeScreen();
//   }
//   switch(screen){
//     case 0:drawSetup();
//     break;
//     case 1:drawGraph();
//     break;
//     case 2:
//     default:drawComm();
//   }

//   // if (ts.tirqTouched() && ts.touched()) {
//   if (ts.touched()) {
//     TS_Point p = ts.getPoint();
//     // printTouchToDisplay(p);
//     int x = map(p.x,245,3823,0,TFT_HEIGHT);
//     int y = map(p.y,233,3851,0,TFT_WIDTH);
//     for (uint8_t b = 0; b < 3; b++) {
//         if ((p.z > 0) && menu[b].contains(x, y)) {
//           menu[b].press(true);  // tell the button it is pressed
//           menu[b].drawButton(true,menuStr[b]);
//           screen = b;
//         } else {
//           menu[b].press(false);  // tell the button it is NOT pressed
//           menu[b].drawButton(false,menuStr[b]);
//         }
//       }
//   }
//   // tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   // tft.drawCentreString(String(screen),5,5,4);
// }
