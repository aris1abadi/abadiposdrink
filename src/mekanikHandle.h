#ifndef MEKANIKHANDLE_H
#define MEKANIKHANDLE_H

#define POMPA_AIR_PUTIH     21
#define POMPA_AIR_PANAS     22
#define POMPA_TEH           23
#define POMPA_ES            25
#define POMPA_JERUK         26
#define POMPA_GULA          27

#define M_ADUK              32

#define SENSOR_1    A5
#define SENSOR_2    A6
#define SENSOR_3    A7
#define SENSOR_4    A8

#define SERVO_MAIN1 16
#define SERVO_MAIN2 17
#define SERVO_GULA  15


#define ES_TEH 1
#define ES_JERUK 2
#define TEH_PANAS 3
#define JERUK_PANAS 4
#define KOPIMIX_PANAS 5
#define ES_KOPIMIX 6
#define KOPIHITAM_PANAS 7

void servoInit();
void pompaInit();
void setPos(int servo,int pos);
void servoLoop();
void bikinEsTeh();
void bikinMinum(int order);
void prosesBikinMinum();
#endif