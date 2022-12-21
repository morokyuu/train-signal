/*
 * uart format
 * [command][sub][value0]..[valueN]
 *
 * TS : time setting
 * TS01xx train detection frame
 * TS02xx transition frame for each signal state
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

bool timer_flag = false;

void timerTask(){
    timer_flag = true;
}

void setup(){
    pinMode(YELLOW1,OUTPUT);
    pinMode(RED,OUTPUT);
    pinMode(YELLOW2,OUTPUT);
    pinMode(GREEN,OUTPUT);
    MsTimer2::set(100,timerTask);
    MsTimer2::start();

    Serial.begin(115200);
}

int sens_count = 0;
int TRAIN_DETECT_TH = 410;
int TRAIN_DETECT_FRAME = 5;
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
int transition_count = 0;
const int TRANSITION_FRAME = 10;

void loop(){
    event = CALL;

    if(timer_flag){
        timer_flag = false;
    }

    int sensor = analogRead(A0);
    int volume = analogRead(A3);

    if(sensor < volume){
        sens_count++;
        if(sens_count > TRAIN_DETECT_TH){
            sens_count = 0;
            event = SENS;
//            digitalWrite(YELLOW2,HIGH);
        }
    }

    if(status != GO){
        transition_count++;
        if(transition_count > TRANSITION_FRAME){
            transition_count = 0;
            event = TIMEOUT;
        }
    }
    else{
        transition_count = 0;
    }

    switch(status){
        case GO:
            digitalWrite(YELLOW1,LOW);
            digitalWrite(RED,LOW);
            digitalWrite(YELLOW2,LOW);
            digitalWrite(GREEN,HIGH);
            if(event == SENS){
                status = STOP;
            }
            break;
//        case SLOW:
//            digitalWrite(YELLOW1,HIGH);
//            digitalWrite(RED,LOW);
//            digitalWrite(YELLOW2,LOW);
//            digitalWrite(GREEN,HIGH);
//            if(event == TIMEOUT){
//                status = GO;
//            }
//            break;
//        case WARN:
//            digitalWrite(YELLOW1,LOW);
//            digitalWrite(RED,LOW);
//            digitalWrite(YELLOW2,HIGH);
//            digitalWrite(GREEN,LOW);
//            if(event == TIMEOUT){
//                status = SLOW;
//            }
//            break;
        case STOP:
            digitalWrite(YELLOW1,LOW);
            digitalWrite(RED,HIGH);
            digitalWrite(YELLOW2,LOW);
            digitalWrite(GREEN,LOW);
            if(event == TIMEOUT){
                //status = WARN;
                status = GO;
            }
            break;
        default:
            break;
    }

//    analogWrite(RED,120);

    Serial.print(volume);
    Serial.print(", ");
    Serial.print(sensor);
    Serial.println("");
}


