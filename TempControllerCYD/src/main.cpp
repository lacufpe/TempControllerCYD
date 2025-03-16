// #include <Arduino.h>
#include "TempControllerCYD_screens.h"
#include <DS18B20.h>
#include "myWebServer.h"
#include "control.h"
#include "fileAccess.h"

int MaxAcq = 100;
int timeValues[100];
int setpointValues[100];
int temperatureValues[100];
int measIdx = 0;
int tempoEmSegundos, tempoInicio, tempoProximaMedida;
int deltaT = 1;
bool aquisicaoAtiva = false; // Verdadeiro se aqdquirindo dados
bool newValueForChart = false;

String filename;

Controller* controller = nullptr;

float temperaturaAtual;
int setPoint,heaterStatus,hysteresis;

void startAcq(){
  Serial.println("Started acquisition");
  aquisicaoAtiva = true;
  controller->start();
  tempoInicio = millis();
  tempoProximaMedida = tempoInicio;
  tempoEmSegundos = 0;
  measIdx = 0;
  filename = initializeNewFile();
}

void stopAcq(){
  Serial.println("Stoped acquiring");
  aquisicaoAtiva = false;
  controller->stop();
}

void setup() {
  Serial.begin(115200); // Start serial communication through USB
  startFS(); // Starts SPIFFS (from fileAccess.h)
  startNetwork(); // Starts the network (from myWebServer.h)
  initScreen(); // from TempControllerCYD_screens.h
  static MAX6675 thermocouple(thermoCLK, thermoCS, thermoSO); // Initiates the thermcouple
  delay(500); // and give it time to correctly start
  controller = new Controller(&thermocouple); // Create the temperatura controller (from controller.h)
  controller->setSetpoint(100,1); // and set the setpoint and hysteresis (from controller.h)
}

void loop(){
  controller->controlLoop(); // (from controller.h)
  // temperaturaAtual = controller->getValue(); // get the temperature value of the thermocouple (from controller.h)
  temperaturaAtual = random(1000);
  heaterStatus = controller->getHeaterStatus(); // And get if the heater is on or off (from controller.h)
  updateScreen(); // from TempCOntrollerCYD_screens.h
  handleNetwork(); // from myWebServer.h

  //acquisition loop
  if (aquisicaoAtiva){ // Se estiver fazendo aquisição
    int agora = millis();
    if (agora >= tempoProximaMedida){ //Checa se é tempo de fazer outra medida
      tempoEmSegundos = (tempoProximaMedida - tempoInicio)/1000; // Salva o tempo em segundos
      // Salva no array circular que faz o gráfico
      int arrayIdx = measIdx%MaxAcq; //Define um índice para o array circular do gráfico
      timeValues[arrayIdx] = tempoEmSegundos; 
      temperatureValues[arrayIdx] = temperaturaAtual;
      setpointValues[arrayIdx] = setPoint;
      newValueForChart = true;
      // Serial.println(timeValues[arrayIdx]);
      //Salva no arquivo 
      appendToCSV(filename.c_str(),createCSVLine(tempoEmSegundos,temperaturaAtual,setPoint));
      
      measIdx++; // Atualiza o índice das medidas
      tempoProximaMedida += deltaT*1000; // e define o tempo da próxima medida
    }
  }

}

//AMDG
