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
            digitalWrite(YELLOW2,LOW);
            digitalWrite(GREEN,HIGH);
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

volatile bool timer_flag = false;
void timerTask(){
    timer_flag = true;
    Serial.println("nanika");
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
    timer_count++;
    if(timer_count > TRANSITION_FRAME){
        timer_count = 0;
        return 1;
    }
    return 0;
}

int sens_count = 0;
int TRAIN_DETECT_FRAME = 3;

int detectTrain(){
    int flag = 0;
    Serial.print(sens_count);
    Serial.println("");

    if(sensor > volume){
        sens_count++;
        if(sens_count > TRAIN_DETECT_FRAME){
            sens_count = TRAIN_DETECT_FRAME-1;
            flag = 1;
        }
    }
    else{
        sens_count = 0;
        flag = 0;
    }
    return flag;
}

void loop(){
    while(1){
        if(timer_flag == true){
            timer_flag = false;
            break;
        }
    }

    sensor = analogRead(A0);
    volume = analogRead(A3);
    if(sensor > volume){
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
            if(detectTrain()){
                changeLED(STOP);
                status = STOP;
            }
            else{
                if(transitionTriger()){
                    changeLED(GO);
                    status = GO;
                }
            }
            break;
        case WARN:
            if(detectTrain()){
                changeLED(STOP);
                status = STOP;
            }
            else{
                if(transitionTriger()){
                    changeLED(SLOW);
                    status = SLOW;
                }
            }
            break;
        case STOP:
            if(detectTrain()){
                changeLED(STOP);
                status = STOP;
                timer_count = 0;
            }
            else{
                if(transitionTriger()){
                    changeLED(WARN);
                    status = WARN;
                }
            }
            break;
        default:
            break;
    };
}


