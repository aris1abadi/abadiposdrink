
#include <Arduino.h>
// #include "serverHandle.h"
#include "mainServoHandle.h"

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
				switch (n_servo)
				{
				case 0:
					setPos(1, 0);
					setPos(2, 0);
					break;
				case 1:
					setPos(1, 45);
					setPos(2, 45);
					break;
				case 2:
					setPos(1, 90);
					setPos(2, 90);
					break;
				case 3:
					setPos(1, 135);
					setPos(2, 135);
					break;
				case 4:
					setPos(1, 180);
					setPos(2, 180);
					break;
				case 5:
					isReady = true;
					break;
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
	delay(5000);
	setPos(1, 0);
	setPos(2, 0);
}

void loop()
{
	// put your main code here, to run repeatedly:
	// socketIOLoop();
	servoLoop();
	serialLoop();
	if (isReady)
	{
		isReady = false;
		bikinEsTeh();
	}
}
