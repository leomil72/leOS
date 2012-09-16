/*
This is a simple sketch to demonstrate the use of leOS, a
<l>ittle <e>mbedded <O>perating <S>ystem" 

leOS is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule little works in background, so that you can forget about
them.

This sketch shows how to manage a task from within another task. The first task
will blink a LED while the second task will pause and restart the other one.
Just put 1 LED (with an appropriate resistor) on digital pin 9.

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
const byte LED = 9;
volatile byte flashValue = 0;
volatile char flashIncrement = 1;
volatile byte ledStatus = 1;

//program setup
void setup() {
    myOS.begin(); //initialize the scheduler
    
    //add the tasks at the scheduler
    myOS.addTask(flashLed, 2);
    myOS.addTask(pauseLed, 5000); 
}


//main loop
void loop() {
    //it's empty because all of the running code is inside the tasks :-)
}  
    

//this task will blink the led
void flashLed() {
    flashValue += flashIncrement;
    //check if the value has reached the bottom or top value, in case
    //the direction of the blinking will be changed
    if ((flashValue == 255) || (flashValue == 0)) {
        flashIncrement *= -1;
    }
    analogWrite(LED, flashValue);
}


//every 5 seconds this task will pause/restart the other task
void pauseLed() {
    //change the status of the first task using a XOR operation (remember: 1^1=0 and 0^1=1)
    ledStatus ^= 1;
    //check if the blinking task must be paused or restarted
    if (ledStatus == 0) {
        myOS.pauseTask(flashLed);
        analogWrite(LED, 0);
    } else {
        myOS.restartTask(flashLed);
    }
}
