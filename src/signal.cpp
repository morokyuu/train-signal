#include <Arduino.h>
#include <MsTimer2.h>

const int YELLOW1 = 9;
const int RED = 6;
const int YELLOW2 = 5;
const int GREEN = 3;

unsigned char blight = 0;

void timerTask(){
    blight += 10;
}

void setup(){
    pinMode(led,OUTPUT);
    MsTimer2::set(100,timerTask);
    MsTimer2::start();

    Serial.begin(115200);
}

void loop(){
    analogWrite(led,blight);

    Serial.print(blight);
    Serial.println("");
}
