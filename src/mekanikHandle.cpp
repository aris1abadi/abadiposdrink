/*
pin servo recomended 2,4,12-19,21-23,25-27,32-33
*/

#include <ESP32Servo.h>
#include "Tasker.h"
#include "mekanikHandle.h"

#define ON LOW
#define OFF HIGH

Tasker tasker;

static const int servo1Pin = SERVO_MAIN1;
static const int servo2Pin = SERVO_MAIN2;
static const int servo3Pin = SERVO_GULA;

Servo servo1;
Servo servo2;

unsigned long lastMillis = 0;
extern int antrianCount;

int targetPos1 = 0;
int lastPos1 = 0;
int nowPos1 = 0;

int targetPos2 = 0;
int lastPos2 = 0;
int nowPos2 = 0;

int delayCount = 0;

bool isBusy = true;
bool stepFinish = false;
bool orderReady = false;
int orderCount = 0;
// String orderId[10];

int stepCount = 0;
int stepDelay = 0;
int jenisOrder = ES_TEH;

int onProses[50];

bool sedangProses = false;
int prosesCount = 0;

// prototype
void pos0();
void pos1();
void pos2();
void pos3();
void pos4();
void tambahGula(int jml);
void stopGula();
void tambahTeh(int jml);
void stopTeh();
void tambahJeruk(int jml);
void stopJeruk();
void tambahAirPanas(int jml);
void stopAirPanas();
void tambahAirPutih(int jml);
void stopAirPutih();

void tambahEs();
void stopEs();

void adukStart(int time);
void adukStop();

void cekStep();

void servoInit()
{
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

    // tasker.setInterval(loopServo, 20);
}

void pompaInit()
{
    pinMode(POMPA_AIR_PUTIH, OUTPUT);
    pinMode(POMPA_TEH, OUTPUT);
    pinMode(POMPA_ES, OUTPUT);
    pinMode(POMPA_JERUK, OUTPUT);

    pinMode(POMPA_GULA, OUTPUT);
    pinMode(POMPA_AIR_PANAS, OUTPUT);

    pinMode(M_ADUK, OUTPUT);

    // aktif low
    digitalWrite(POMPA_AIR_PUTIH, HIGH);
    digitalWrite(POMPA_TEH, HIGH);
    digitalWrite(POMPA_ES, HIGH);
    digitalWrite(POMPA_JERUK, HIGH);
    digitalWrite(POMPA_GULA, HIGH);
    digitalWrite(POMPA_AIR_PANAS, HIGH);

    digitalWrite(M_ADUK, HIGH);
}

void delayPosisi()
{
    isBusy = false;
}

void finishPos()
{
    isBusy = true;
    stepFinish = true;
    Serial.println("1 order selesai");
}

void cekStep()
{
    if (!isBusy)
    {
        stepFinish = false;
        isBusy = true;
        // posisi 1
        switch (stepCount)
        {
        case 0: // posisi 1
            pos1();
            tasker.setTimeout(delayPosisi, 1000);
            stepCount += 1;
            break;

        case 1: // isi gula jeruk/teh
            tambahGula(2);
            if ((jenisOrder == ES_TEH) || (jenisOrder == TEH_PANAS))
            {
                tambahTeh(3);
            }
            else if ((jenisOrder == JERUK_PANAS) || (jenisOrder == ES_JERUK))
            {
                tambahJeruk(2);
            }
            tambahAirPanas(2);
            tasker.setTimeout(delayPosisi, 4000);
            stepCount += 1;
            break;

        case 2: // posisi 2
            pos2();
            tasker.setTimeout(delayPosisi, 1000);
            stepCount += 1;
            break;
        case 3: // aduk
            adukStart(4);
            tasker.setTimeout(delayPosisi, 5000);
            stepCount += 1;
            break;

        case 4: // aduk
            pos3();
            tasker.setTimeout(delayPosisi, 1000);
            stepCount += 1;
            break;

        case 5: // aduk
            if ((jenisOrder == ES_JERUK) || (jenisOrder == ES_TEH) || (jenisOrder == ES_KOPIMIX))
            {
                tambahEs();
            }
            else
            {
                tambahAirPanas(3);
            }
            tasker.setTimeout(delayPosisi, 5000);
            stepCount += 1;
            break;

        case 6: // aduk
            pos4();
            tasker.setTimeout(delayPosisi, 3000);
            stepCount += 1;
            break;

        case 7: // aduk
            pos0();
            tasker.setTimeout(finishPos, 5000);
            stepCount = 0;
            break;
        }
    }
}

void setPos(int servo, int pos)
{
    if (servo == 1)
    {
        targetPos1 = pos;
    }
    else
    {
        targetPos2 = pos;
    }
}

void tambahGula(int jml = 1)
{
    digitalWrite(POMPA_GULA, ON);
    tasker.setTimeout(stopGula, (jml * 1000));
}
void stopGula()
{
    digitalWrite(POMPA_GULA, OFF);
}

void tambahTeh(int jml = 1)
{
    digitalWrite(POMPA_TEH, ON);
    tasker.setTimeout(stopTeh, (jml * 1000));
}

void stopTeh()
{
    digitalWrite(POMPA_TEH, OFF);
}

void tambahJeruk(int jml = 1)
{

    digitalWrite(POMPA_JERUK, ON);
    tasker.setTimeout(stopJeruk, (jml * 1000));
}

void stopJeruk()
{
    digitalWrite(POMPA_JERUK, OFF);
}

void tambahAirPanas(int jml = 1)
{
    digitalWrite(POMPA_AIR_PANAS, ON);
    tasker.setInterval(stopAirPanas, (jml * 1000));
}

void stopAirPanas()
{
    digitalWrite(POMPA_AIR_PANAS, OFF);
}

void tambahAirPutih(int jml = 1)
{
    digitalWrite(POMPA_AIR_PUTIH, ON);
    tasker.setTimeout(stopAirPutih, (jml * 1000));
}

void stopAirPutih()
{
    digitalWrite(POMPA_AIR_PUTIH, OFF);
}

void tambahEs()
{
    digitalWrite(POMPA_ES, ON);
    tasker.setTimeout(stopEs, 4000);
}

void stopEs()
{
    digitalWrite(POMPA_ES, OFF);
}

void adukStart(int time = 1)
{
    digitalWrite(M_ADUK, ON);
    tasker.setTimeout(adukStop, (time * 1000));
}

void adukStop()
{
    digitalWrite(M_ADUK, OFF);
}

void pos0()
{
    setPos(1, 0);
    setPos(2, 0);
}

void pos1()
{
    setPos(1, 0);
    setPos(2, 45);
}

void pos2()
{
    setPos(1, 45);
    setPos(2, 45);
}

void pos3()
{
    setPos(1, 90);
    setPos(2, 90);
}

void pos4()
{
    setPos(1, 180);
    setPos(2, 180);
}

void posEnd()
{
    setPos(1, 0);
    setPos(2, 0);
}

void bikinMinum(int order)
{
    jenisOrder = order;
    stepCount = 0;
    isBusy = false;
    sedangProses = true;
}

void bikinEsTeh()
{
    jenisOrder = ES_TEH;
    stepCount = 0;
    isBusy = false;
}

void prosesBikinMinum()
{
    if ((stepFinish) && (sedangProses))
    {
        
        // next order
        if (onProses[prosesCount] == '+')
        {
            // nek order pelanggan
            Serial.println("Order pelanggan selanjutnya");
            prosesCount += 1;

            orderReady = true;
        }
        else if (onProses[prosesCount] == '-')
        {
            // proses selesai
            antrianCount = 0;
            prosesCount = 0;
            sedangProses = false;
            orderReady = true;
            stepFinish = false;
            Serial.println("Order Selesai");
        }
        else
        {
            jenisOrder = onProses[prosesCount];            
            stepCount = 0;
            isBusy = false;
            prosesCount += 1;
            stepFinish = false;
        }
    }
}

void servoLoop()
{
    tasker.loop();
    if ((millis() - lastMillis) > 20)
    {
        lastMillis = millis();
        if (lastPos1 != targetPos1)
        {
            if (lastPos1 > targetPos1)
            {
                lastPos1 -= 1;
            }
            else
            {
                lastPos1 += 1;
            }
            servo1.write(lastPos1);
        }

        if (lastPos2 != targetPos2)
        {
            if (lastPos2 > targetPos2)
            {
                lastPos2 -= 1;
            }
            else
            {
                lastPos2 += 1;
            }
            servo2.write(lastPos2);
        }
        // cek step tiap 200ms
        if (++stepDelay > 10)
        {
            stepDelay = 0;
            cekStep();
            prosesBikinMinum();
        }
    }
}
