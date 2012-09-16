/*
This is a simple sketch to demonstrate the use of leOS, a
<l>ittle <e>mbedded <O>perating <S>ystem" 

leOS is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule little works in background, so that you can forget about
them.

This sketch demonstrates the blink of 1 LED without using millis() nor delay().
Simply upload the sketch and look at the LED on pin 13.

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
const byte LED1 = 13;

//program setup
void setup() {
    myOS.begin(); //initialize the scheduler
    pinMode(LED1, OUTPUT);
    
    //add the tasks at the scheduler
    myOS.addTask(flashLed1, 1000);
}


//main loop
void loop() {
    //empty
}


//flashing task
void flashLed1() {
    led1Status^=1;
    digitalWrite(LED1, led1Status);
}

