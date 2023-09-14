

#include <ESP32Servo.h>

static const int servo1Pin = 16;
static const int servo2Pin = 17;

Servo servo1;
Servo servo2;

long lastMillis = 0;

int targetPos1 = 0;
int lastPos1 = 0;
int nowPos1 = 0;

int targetPos2 = 0;
int lastPos2 = 0;
int nowPos2 = 0;

int delayCount = 0;

void servoInit(){
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servo1.setPeriodHertz(50);
    servo2.setPeriodHertz(50);

    servo1.attach(servo1Pin, 500, 2400);
    servo2.attach(servo2Pin, 500, 2400);

    servo1.write(0);
    servo2.write(0);

}

void setPos(int servo,int pos){
    if(servo == 1){
        targetPos1 = pos;
    }else{
        targetPos2 = pos;
    }
}

void bikinEsTeh(){
    servo1.write(0);
    servo2.write(0);

    delay(2000);

    servo1.write(0);
    servo2.write(45);

    delay(2000);

    servo1.write(45);
    servo2.write(45);

    delay(2000);

    servo1.write(90);
    servo2.write(90);

    delay(2000);

    servo1.write(180);
    servo2.write(180);

    delay(2000);

    servo1.write(0);
    servo2.write(0);
}


void servoLoop(){
    //smooth delay
    if((millis() - lastMillis) > 15){
        lastMillis = millis();
        
        if(lastPos1 != targetPos1){
            if(lastPos1 > targetPos1){
                lastPos1 -= 1;                
            }else{
                lastPos1 += 1;
            }
            servo1.write(lastPos1);
        }

        if(lastPos2 != targetPos2){
            if(lastPos2 > targetPos2){
                lastPos2 -= 1;                
            }else{
                lastPos2 += 1;
            }
            servo2.write(lastPos2);
        }

    }
}

