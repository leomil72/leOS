/*
This is a simple sketch to demonstrate the use of leOS, a
<l>ittle <e>mbedded <O>perating <S>ystem" 

leOS is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule little works in background, so that you can forget about
them.

This sketch shows how to add a task into the scheduler that doesn't
have to start immediately after adding. 

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

//include the leOS
#include "leOS.h"

leOS myOS; //create a new istance of the class leOS

//variables to control the LED
const byte LED = 13;
volatile byte ledStatus = 1;

//program setup
void setup() {
    pinMode(LED, OUTPUT);
    myOS.begin(); //initialize the scheduler
    
    //add the tasks at the scheduler
    myOS.addTask(flashLed, 500, PAUSED);
    delay(5000); //wait for a while...
    myOS.restartTask(flashLed); //than it starts the task
}


//main loop
void loop() {
    //empty
}  
    

//this task will blink the led
void flashLed() {
    digitalWrite(LED, ledStatus);
    ledStatus ^= 1;
}
