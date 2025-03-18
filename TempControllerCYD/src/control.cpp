// controller.cpp
#include "control.h"

Controller::Controller(MAX6675* thermocouple) :
thermocouple(thermocouple) {

    pinMode(relayPin,OUTPUT);
    relayValue = 0;
    digitalWrite(relayPin,relayValue);
    state = 0; // 0 - stopped
               // 1 - on, heating
               // 2 - on, cooling
    next = millis()+250;
}

Controller::~Controller() {
}

void Controller::controlLoop(){
    if(millis()>=next){
        next +=controlDeltaT;
        oldValue = value;
        value = thermocouple->readCelsius();
        deltaTemp = 0.8*deltaTemp + 0.2*(value - oldValue);
        // Serial.print("T: ");
        // Serial.print(value);
        // Serial.print(". Status: ");
        // Serial.print(state);
        // Serial.print(". minValue: ");
        // Serial.print(minValue);
        // Serial.print(". maxValue: ");
        // Serial.print(maxValue);
        // Serial.print(". relayValue: ");
        // Serial.println(relayValue);
        
        switch(state){
        case 0: // stopped
            relayValue = 0;
            break;
        case 1: // on, heating
            if ((value + 10*deltaTemp) >= maxValue){
                state = 2;
                relayValue = 0;
            } else {
                relayValue = 1;
            }
            break;
        case 2: // off, cooling
            if ((value + 10*deltaTemp) <= minValue){
                state = 1;
                relayValue = 1;
            } else {
                relayValue = 0;
            }
            break;
        }
        digitalWrite(relayPin,relayValue);
    }
}

int Controller::getSetpoint(){
    return setPoint;
}

float Controller::getValue(){
    return value;
}

int Controller::getHeaterStatus(){
    return relayValue;
}

void Controller::setSetpoint(int setpoint, int histerese){
    setPoint = setpoint;
    int delta = histerese>>1;
    maxValue = setPoint+delta;
    minValue = setPoint-delta;
}

void Controller::start(){
    next = millis() + 250;
    if(value>setPoint){
        state = 2;
    } else {
        state = 1;
    }
}
void Controller::stop(){
    state = 0;
}
