/*
This is a simple sketch to demonstrate the use of leOS, a
<l>ittle <e>mbedded <O>perating <S>ystem

leOS is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule little works in background, so that you can forget about
them.

This sketch demonstrates the ability to modify a running tast varying the 
interval of a flashing LED.
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

//variables to control the LED
byte led1Status = 0;
const byte LED1 = 13;
unsigned long taskInterval = 500;


//program setup
void setup() {
    myOS.begin(); //initialize the scheduler
    pinMode(LED1, OUTPUT);
    
    //add the tasks at the scheduler
    myOS.addTask(flashLed1, taskInterval);
}


//main loop
void loop() {
    delay(10000); //wait 10 secs.
    
    //change the flashing interval
    if (taskInterval == 500) {
        taskInterval = 2000;
    } else {
        taskInterval = 500;
    }
    //modify the task
    myOS.modifyTask(flashLed1, taskInterval);
}


//flashing task
void flashLed1() {
    led1Status^=1;
    digitalWrite(LED1, led1Status);
}
