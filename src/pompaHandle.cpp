#include "Arduino.h"

#define POMPA_AIR_PUTIH    15
#define POMPA_TEH   16
#define POMPA_ES    17
#define POMPA_JERUK 18
#define POMPA_GULA  19
#define POMPA_AIR_PANAS 25


#define M_ADUK      20
#define M_GELAS_1   21
#define M_GELAS_2   22
#define M_GELAS_3   23
#define M_GELAS_4   24

#define SENSOR_1    A5
#define SENSOR_2    A6
#define SENSOR_3    A7
#define SENSOR_4    A8

long pompaDelay = 0;
int pompaTime = 1000; //1 detik

bool order_esteh = false;
bool order_esJeruk = false;
bool order_tehPanas = false;
bool order_jerukPanas = false;

int jmlOrder = 0;
int orderStep = 0;

void buatEsteh();



void init_pompa(){
    pinMode(POMPA_AIR_PUTIH,OUTPUT);
    pinMode(POMPA_TEH,OUTPUT);
    pinMode(POMPA_ES,OUTPUT);
    pinMode(POMPA_JERUK,OUTPUT);
    pinMode(POMPA_GULA,OUTPUT);
    pinMode(POMPA_AIR_PANAS,OUTPUT);

    pinMode(M_ADUK,OUTPUT);
    pinMode(M_GELAS_1,OUTPUT);
    pinMode(M_GELAS_2,OUTPUT);
    pinMode(M_GELAS_3,OUTPUT);
    pinMode(M_GELAS_4,OUTPUT);

    //aktif low
    digitalWrite(POMPA_AIR_PUTIH,HIGH);
    digitalWrite(POMPA_TEH,HIGH);
    digitalWrite(POMPA_ES,HIGH);
    digitalWrite(POMPA_JERUK,HIGH);
    digitalWrite(POMPA_GULA,HIGH);
    digitalWrite(POMPA_AIR_PANAS,HIGH);

    digitalWrite(M_ADUK,HIGH);
    digitalWrite(M_GELAS_1,HIGH);
    digitalWrite(M_GELAS_2,HIGH);
    digitalWrite(M_GELAS_3,HIGH);
    digitalWrite(M_GELAS_4,HIGH);

}

void pompa_loop(){
    if((millis() - pompaDelay) > pompaTime){
        pompaDelay = millis();

        if(order_esteh){
            buatEsteh();
        }
    }
}

void buatEsteh(){
    //cek gelas
    
    //masukan gula

    //masukan tes 

    //tambah sedikit air panas

    //geser gelas 

    //aduk

    //tambah es
}