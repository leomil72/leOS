/*
This is a simple sketch to demonstrate the use of leOS, a
<l>ittle <e>mbedded <O>perating <S>ystem" 

leOS is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule little works in background, so that you can forget about
them.

This sketch tests the use of 1 task that runs in background.
To try this sketch, create a circuit on a protoboard connecting 2 LEDs
(and their resistors) at Arduino pins 7, and 8.

More info on the schedulers and the methods can be found into the README file.

Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	
This code and the leOS library are free software; you can redistribute 
and/or modify them under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3.0 of the License,
or (at your option) any later version.

The leOS library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

*/
//include the OS
#include "leOS.h"

leOS myOS; //create a new istance of the class leOS

//variables to control the LEDs
byte led1Status = 0;
byte led2Status = 0;
const byte LED1 = 7;
const byte LED2 = 8;


//program setup
void setup() {
    myOS.begin(); //initialize the scheduler
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    
    //add the tasks at the scheduler
    myOS.addTask(flashLed1, 600);
}


//main loop
void loop() {
    digitalWrite(LED2, led2Status);
    led2Status ^= 1;    
    delay(1000);
}


//first task
void flashLed1() {
    led1Status^=1;
    digitalWrite(LED1, led1Status);
}

