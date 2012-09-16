******** leOS.h - little embedded Operating System *******

leOS (little embedded Operating System)

This is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule the execution of little routines in background, so that
you can forget about them. It's designed for Arduino and other common Atmel
microcontrollers (for the complete list please read below).

Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>


***********************
Version history

v. 0.1.3:  now a task can be added in "pause mode"; new example sketches
v. 0.1.2:  fixed a bug in the management of one-time pads
v. 0.1.1:  now the user can choose between 32-bit & 64-bit math
v. 0.1.0:  leOS now works correcty on Arduino Leonardo/Atmega32U4
v. 0.0.8:  now the user can modify running tasks
v. 0.0.7:  introduced one-time tasks
v. 0.0.6:  (preliminary) support for Arduino Leonardo/Atmega32U4
v. 0.0.5:  fixed some bugs and optimized code & memory consumption
v. 0.0.4:  use of a 64-bit counter so the scheduler will overflow after 584,942,417 years
v. 0.0.3:  added methods to pause and restart a task
v. 0.0.2:  user can now delete scheduled tasks
v. 0.0.1:  early release - user can only add tasks


***********************
How to use it - Methods

Unpack the library and copy it into your /libraries folder, that usually is in
your sketchs' folder. Then include the library and create a new istance of it
by adding the following code at the top of your sketch:

#include "leOS.h"
leOS myOS

Then you have to initialize the library in the setup() routine:
void setup() {
  myOS.begin();
  .....
}

Now you can add a task by simply call the method .addTask():
void setup() {
  myOS.begin();
  myOS.addTask(yourFunction, scheduleTime[, status]);
  ...
}

yourFunction must be a routine that is inside your sketch. scheduleTime is
the scheduled interval in milliseconds at which you want your routine
to be executed. Starting with version 0.1.3, now the user can choose the status
of the task when it adds it to the scheduler. status can be: PAUSED, for a task
that doesn't have to start immediately; SCHEDULED (default option), for a normal
task that has to start after its scheduling; ONETINE, for a task that has to
run only once.
An interesting feature is the ability to run one-time tasks. A one-time task
is a task that will be run only once: the scheduler, once it has run the task,
will remove it from the running tasks (it won't be paused, it will be permanently
deleted).


To pause a task, just call the following method:

myOS.pauseTask(yourFunction);


You can restart it with:

myOS.restartTask(yourFunction);


To remove a task from the scheduler call this method:

myOS.removeTask(yourFunction);


To modify a running task, simply call the modifyTask method with the new interval time 
and/or the kind of the status' task, i.e. a normal or a one-time task:

myOS.modifyTask(yourFunction, newInterval [, newTaskStatus]);

newTaskStatus can be ONETIME if you decide to transform a normal task into a
one-time task, or SCHEDULED if you want to transform a one-time task into
a normal task (the one-time has still to be executed).

It's the user that should check his code to avoid strange situations when he 
pauses a task. I.e.: if the task that must be paused alternates the output
of a pin, the user should check if the state of the pin after the pause is
compatible with his circuit.

The examples that comes with the library explain very well the simple use
of the OS.


***********************
32-/64-bit math

Starting with version 0.1.1, the user can choose between 32-bit and 64bit math.
Using 32bit math maximum interval that can be choosed is limited to 49.7 days,
while using 64-bit math the maximum that can be choosed is only limited by the
user's fantasy, due to the fact that the 64-bit counter will restart from 0
after 584,942,417 years (default maximum interval is 1 hour, but you can change this
value editing the leOS::addTask method inside the leOS.cpp file).

To switch between 32-bit and 64-bit math just comment/uncomment the line
#define SIXTYFOUR_MATH

that is present at the beginning of the code inside the leOS.h file.

***********************
How it works

The OS uses the timer 2, that has an 8-bit counter, to schedule the user's tasks.
The user must create simple tasks that:
1) don't use any other interrupts/timers (due to the fact that an interrupt
service routine (ISR) is atomic (this means that it cannot be halted or
interrupted by another interrupt)
2) don't use PWM functionalities over Arduino pins 3 & 11
3) don't keep too much running time


***********************
Supported microcontrollers

Actually the library has been successfully tested with Arduino UNO, Atmega328, 
Arduino MEGA2560 and Arduino Leonardo.
The library should compile and work versus the following:
- Attiny2313/4313
- Attiny24/44/84 & Attiny25/45/85
- Atmega644/1284
- Atmega8
- Atmega88/168/328 
- Atmega640/1280/1281/2560/2561
- Atmega32U4 (only at 16 MHz)

Supported clock are 1, 4, 8, and 16 MHz.

NOTE:
To use leOS on Attiny 24/44/84 microcontrollers you should modify a file from the Tiny 
core to move the millis() and delay() functions from timer 0 to timer 1.
To do that, open the file /arduino-your_version/hardware/tiny/cores/tiny/core_build_options.c
and look for the section "Build options for the ATtiny84 processor".
A couple of rows below, change this line
#define TIMER_TO_USE_FOR_MILLIS                   1
into
#define TIMER_TO_USE_FOR_MILLIS                   0
then save the file and reload the Arduino IDE.


***********************
Licence

This library is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public	License as published by the Free Software 
Foundation; either version 3.0 of the License, or (at your option) any later 
version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 


***********************
Document revision

12th revision: 2012/09/16
