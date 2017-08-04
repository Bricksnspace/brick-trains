#include "Arduino.h"


#define TRAIN_NUM 4

// statuses
#define ST_STOP 1
#define ST_RUN  2
#define ST_WAIT 3


// timings in millisec
#define TIME_STOP	80000	// 80 sec
#define TIME_RUN	20000	// 20 sec
#define TIME_DELTA	20000	// 20 sec
#define TIME_MAX	120000	// 120 sec maximum run time without hit a sensor


// output pins
int outpins[] = { 2,3,4,5 };
int sensors[] = { 14,15,16,17 }; // analog in 0,1,2,3

// status of single train
int status[TRAIN_NUM];

// recorded time for wait start
unsigned long times[TRAIN_NUM];

// how millisecs must wait
unsigned long waits[TRAIN_NUM];



//The setup function is called once at startup of the sketch
void setup()
{
	// init random sequence
	randomSeed(analogRead(3));

	// initial values setup
	for (int i=0; i<TRAIN_NUM; i++) {
		pinMode(outpins[i], OUTPUT);
		digitalWrite(outpins[i],LOW);
		pinMode(sensors[i],INPUT_PULLUP);
		status[i] = ST_STOP;
		times[i] = millis();
		// random value between 10 and 100 sec
		waits[i] = random(10000,100000);
	}
	pinMode(13,OUTPUT);
	digitalWrite(13,LOW);
}

// The loop function is called in an endless loop
void loop()
{
	digitalWrite(13,HIGH);
	unsigned long time = millis();

	for (int i=0; i<TRAIN_NUM; i++) {
		switch (status[i]) {
		case ST_STOP:
			if ((time-times[i]) > waits[i]) {
				status[i] = ST_RUN;
				times[i] = millis();
				waits[i] = TIME_RUN + random(TIME_DELTA);
				digitalWrite(outpins[i],HIGH);
			}
			break;
		case ST_RUN:
			if ((time-times[i]) > waits[i]) {
				status[i] = ST_WAIT;
				times[i] = millis();	// guard time
			}
			break;
		case ST_WAIT:
			if (digitalRead(sensors[i]) == LOW || (time - times[i])>TIME_MAX) {
				// train hit sensor
				status[i] = ST_STOP;
				times[i] = millis();
				waits[i] = TIME_STOP + random(TIME_DELTA);
				digitalWrite(outpins[i],LOW);
			}
			break;
		}
	}
	delay(10);
	digitalWrite(13,LOW);
	delay(90);
}
