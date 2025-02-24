// #include <Arduino.h>
#include "TempControllerCYD_screens.h"
#include <DS18B20.h>
#include "myWebServer.h"
#include "control.h"

int MaxAcq = 400;
int timeValues[400];
int setpointValues[400];
int temperatureValues[400];
int measIdx = 0;
bool aquisicaoAtiva = false; // Verdadeiro se aqdquirindo dados

Controller* controller = nullptr;

int temperaturaAtual,setPoint,heaterStatus,hysteresis, deltaT;

void startAcq(){
  Serial.println("Started acquisition");
  aquisicaoAtiva = true;
  controller->start();
}

void stopAcq(){
  Serial.println("Stoped acquiring");
  aquisicaoAtiva = false;
  controller->stop();
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
  static MAX6675 thermocouple(thermoCLK, thermoCS, thermoSO);
  delay(500);
  controller = new Controller(&thermocouple);
  controller->setSetpoint(100,10);
}

void loop(){
  controller->controlLoop();
  temperaturaAtual = controller->getValue();
  heaterStatus = controller->getHeaterStatus();
  updateScreen();
  handleNetwork();
}

//AMDG
