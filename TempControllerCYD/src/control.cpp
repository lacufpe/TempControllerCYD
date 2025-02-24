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
    Serial.print("NExt: ");
    Serial.println(next);
    Serial.print("Millis(): ");
    Serial.println(millis());
    if(millis()>=next){
        next +=controlDeltaT;
        value = thermocouple->readCelsius();
        switch(state){
        case 0: // stopped
            relayValue = 0;
            break;
        case 1: // on, heating
            if (value >= maxValue){
                state = 2;
                relayValue = 0;
            } else {
                relayValue = 1;
            }
            break;
        case 2: // on, cooling
            if (value <= minValue){
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

int Controller::getValue(){
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
