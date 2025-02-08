#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>

extern int MaxAcq;
extern int timeValues[400];
extern int setpointValues[400];
extern int temperatureValues[400];
extern int measIdx;

void SPIFFS_test(void);
extern bool aquisicaoAtiva; // Verdadeiro se aqdquirindo dados
extern unsigned int htmlUpdateTime;


extern void startAcq(void); // Comando para iniciar Controle e aquisição
extern void stopAcq(void); // Comando para parar Controle e aquisição

extern void startNetwork(void);

extern void handleNetwork(void);

// AMDG