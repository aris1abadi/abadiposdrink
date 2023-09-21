
#include <Arduino.h>
// #include "serverHandle.h"
#include "mekanikHandle.h"
#include <koneksiHandle.h>

String serBuff = "";

bool isReady = true;

void serialLoop()
{
	if (Serial.available())
	{
		char c = Serial.read();
		if (c == '\n')
		{
			// format data 1-020;\n
			if (serBuff.length() == 2)
			{
				int n_servo = (serBuff.charAt(0) - '0');
				int pos = (serBuff.substring(2, 5)).toInt();
				
				if(n_servo < 5){
					bikinMinum(n_servo);
				}else{
					Serial.println("tekan 1-4");
				}

				Serial.print(serBuff);
				Serial.println(" OK");
				serBuff = "";
			}
			else
			{
				Serial.println("format salah");
			}
		}
		else
		{
			serBuff += c;
		}
	}
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
	delay(10);
	// init_socket();
	Serial.print("Mesin siap");
	servoInit();
	pompaInit();
	delay(1000);
	setPos(1, 0);
	setPos(2, 0);

	mqttInit();

	//socketInit();
}

void loop()
{
	// put your main code here, to run repeatedly:
	// socketIOLoop();
	servoLoop();
	mqttLoop();
	serialLoop();
	
}
