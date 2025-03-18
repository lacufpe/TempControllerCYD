// control.h
#ifndef CONTROL_H
#define CONTROL_H

#include "max6675.h"

#define relayPin 4

#define thermoSO 35
#define thermoCS 22
#define thermoCLK 27

#define controlDeltaT 250

class Controller {
private:
    MAX6675* thermocouple;
    float value;
    float oldValue,deltaTemp;
    int relayValue;
    float setPoint;
    float minValue;
    float maxValue;
    int state;
    int next;

public:
    Controller(MAX6675* thermocouple);
    ~Controller(); // Destructor
    void controlLoop();
    int getSetpoint();
    float getValue();
    int getHeaterStatus();
    void setSetpoint(int setpoint, int histerese);
    void start();
    void stop();

};

#endif