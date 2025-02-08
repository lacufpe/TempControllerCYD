// #include <Arduino.h>
#include "TempControllerCYD_screens.h"
#include <DS18B20.h>
#include "myWebServer.h"

int MaxAcq = 400;
int timeValues[400];
int setpointValues[400];
int temperatureValues[400];
int measIdx = 0;
bool aquisicaoAtiva = false; // Verdadeiro se aqdquirindo dados

void startAcq(){
  aquisicaoAtiva = true;
}

void stopAcq(){
  aquisicaoAtiva = false;
}

void setup() {
  Serial.begin(115200);
  startNetwork();
  // SPIFFS_test();
  initScreen();
  for(int i =0;i<MaxAcq;i++){
    timeValues[i] = 10*i;
    setpointValues[i] = 50 + 40 * sinf(2 * PI * i / 50);
    temperatureValues[i] = 50 + 40 * cosf(2 * PI * i / 100);
  }
}

void loop(){
  updateScreen();
  handleNetwork();
}

//AMDG
