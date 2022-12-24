/*
 * uart format
 * [command][sub][value0]..[valueN]
 *
 * TS : time setting
 * TS01xx train detection frame
 * TS02xx changeLED frame for each signal state
 *
 * SC : signal control
 * SC01 GO
 * SC02 SLOW
 * SC03 WARN
 * SC04 STOP
 *
 * VN : version display
 * PN : ping (blink signal)
 *
 * */

#include <Arduino.h>
#include <MsTimer2.h>

const int YELLOW1 = 9;
const int RED = 6;
const int YELLOW2 = 5;
const int GREEN = 3;
const int DEBUG_LED = 13;

int sensor = 0;
int volume = 0;

enum Status{
    GO,
    SLOW,
    WARN,
    STOP
};
enum Event{
    CALL,
    TIMEOUT,
    SENS,
    UART
};

Event event = CALL;
Status status = GO;

void changeLED(Status status_to){
    switch(status_to){
        case GO:
            digitalWrite(YELLOW1,LOW);
            digitalWrite(RED,LOW);
            digitalWrite(YELLOW2,LOW);
            digitalWrite(GREEN,HIGH);
            break;
        case SLOW:
            digitalWrite(YELLOW1,HIGH);
            digitalWrite(RED,LOW);
            digitalWrite(YELLOW2,HIGH);
            digitalWrite(GREEN,LOW);
            break;
        case WARN:
            digitalWrite(YELLOW1,LOW);
            digitalWrite(RED,LOW);
            digitalWrite(YELLOW2,HIGH);
            digitalWrite(GREEN,LOW);
            break;
        case STOP:
            digitalWrite(YELLOW1,LOW);
            digitalWrite(RED,HIGH);
            digitalWrite(YELLOW2,LOW);
            digitalWrite(GREEN,LOW);
            break;
        default:
            break;
    }
}

bool timer_flag = false;
void timerTask(){
    timer_flag = true;
}

void setup(){
    MsTimer2::set(100,timerTask);
    MsTimer2::start();

    Serial.begin(115200);

    pinMode(DEBUG_LED,OUTPUT);

    pinMode(YELLOW1,OUTPUT);
    pinMode(RED,OUTPUT);
    pinMode(YELLOW2,OUTPUT);
    pinMode(GREEN,OUTPUT);
    changeLED(GO);
}


int changeLED_count = 0;
const int TRANSITION_FRAME = 10;
int timer_count = 0;

int transitionTriger(){
    if(timer_flag){
        timer_flag = false;
        timer_count++;
    }

    if(timer_count > TRANSITION_FRAME){
        timer_count = 0;
        return 1;
    }
    return 0;
}

int sens_count = 0;
int TRAIN_DETECT_TH = 410;
int TRAIN_DETECT_FRAME = 5;

int detectTrain(){
    if(sensor < volume){
        sens_count++;
        if(sens_count > TRAIN_DETECT_TH){
            sens_count = 0;
            return 1;
        }
    }
    return 0;
}

void loop(){
    sensor = analogRead(A0);
    volume = analogRead(A3);
    if(sensor < volume){
        digitalWrite(DEBUG_LED,HIGH);
    }
    else{
        digitalWrite(DEBUG_LED,LOW);
    }

    Serial.print(volume);
    Serial.print(", ");
    Serial.print(sensor);
    Serial.println("");


    switch(status){
        case GO:
            if(detectTrain()){
                changeLED(STOP);
                status = STOP;
            }
            break;
        case SLOW:
            if(transitionTriger()){
                changeLED(GO);
                status = GO;
            }
            break;
        case WARN:
            if(transitionTriger()){
                changeLED(SLOW);
                status = SLOW;
            }
            break;
        case STOP:
            if(transitionTriger()){
                changeLED(STOP);
                status = STOP;
            }
            break;
        default:
            break;
    };
}


