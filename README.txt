******** leOS.h - little embedded Operating System *******

leOS (little embedded Operating System)

This is not a complete OS nor an RTOS as you usually know but it's a simple
scheduler to schedule the execution of little routines in background, so that
you can forget about them. It's designed for Arduino and other common Atmel
microcontrollers (for the complete list please read below).

Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>


***********************
Version history

v. 1.1.1:  minor changes
v. 1.1.0:  new methods to stop/restart the scheduler
v. 1.0.3:  now a task can be started once it has been added to the scheduler
v. 1.0.2:  code cleaning
v. 1.0.1a: function taskIsRunning renamed as getTaskStatus
v. 1.0.1:  code cleaning and 32-bit overflow fixing
v. 1.0.0:  added a method to check if a task is running - first stable release
v. 0.1.5:  added support for Atmega344
v. 0.1.4:  core code rewriting (now it uses Structs)
v. 0.1.3:  now a task can be added in "paused mode"; new example sketches
v. 0.1.2:  fixed a bug in the management of one-time pads
v. 0.1.1:  now the user can choose between 32-bits & 64-bits math
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
to be executed. Starting from version 0.1.3, now the user can choose the status
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


To check if a task is running, you have to use the taskIsRunning() method:
myOS.getTaskStatus(yourFunction);

This will return 255 if there was an error (task not found) or a value for the
current status: 
PAUSED - task is paused/not running
SCHEDULED - task is running 
ONETIME - task is scheduled to run in a near future.
SCHEDULED_IMMEDIATESTART or simply IMMEDIATESTART, for a task that has to
be executed once it has been added to the scheduler.

Introduced with leOS 1.1.0 there are 2 new methods that permit to stop/restart
the scheduler, useful if you need to stop all the running tasks at the 
same time:

myOS.haltScheduler();
stops the scheduler and freezes all the tasks preserving their current
intervals;

myOS.restartScheduler();
restarts the scheduler resuming all the tasks that were running.


*Be careful*: the user is asked to check his code to avoid strange situations when 
he pauses a task. I.e.: if the task that has been paused alternated the output of 
a pin and that pin drove an external circuit, the user should check if the status 
of the pin after the task has been paused is safe and compatible with his needs.


***********************
32-/64-bit math

Starting with version 0.1.1, the user can choose between 32-bit and 64-bit math.
Using 32-bit math, the maximum interval that can be choosed is limited to 49.7 days;
the overflow of the counter has been fixed since versione 1.0.1.

While using 64-bit math the maximum that can be choosen is only limited by the
user's fantasy, due to the fact that the 64-bit counter will overflow after 
584,942,417 years (default maximum interval is 1 hour, but you can change this
value editing the leOS::addTask method inside the leOS.cpp file).

To switch between 32-bit and 64-bit math just comment/uncomment the line
#define SIXTYFOUR_MATH

that is present at the beginning of the code inside the leOS.h file.

***********************
How it works

The scheduler makes use of a 8-bit timer of the microcontroller to schedule the user's tasks,
so you'll loose PWM functionalities on the related pins. This is a list of the
timers used on each microcontroller and the pins that you cannot use as PWM
pins anymore:
1) Atmega48/88/168/328 (Arduino UNO/2009): timer 2 - pins 5 & 17 (pins D3 & D11 on Arduino UNO)
2) Atmega344/644/1284: timer 2 - pins 20 & 21 (pins D14 & D15 on Maniacbug core)
3) Atmega640/1280/1281/2560/2561: timer 2 - pins 18 & 23 (pins D9 & D10 an Arduino MEGA)
4) Attiny25/45/85: timer 0 - pins 5 & 6 (pins D0 & D1 on Tiny core)
5) Attiny2313/4313: timer 0 - pins 9 & 14 (pins D7 & D11 on Tiny core)
6) Attiny24/44/84: timer 0 - pins 5 & 6 (pins D2 & D3 on Tiny core)
7) Atmega8: timer 2 - pins 17 (pin D11 on older Arduino boards)
8) Atmega32U4 (Arduino Leonardo): timer 3 - pin 32 (pin D5 on Arduino Leonardo)

The user has to remember that he has to create simple tasks that:
1) don't use any other interrupts/timers inside them (due to the fact that an interrupt
service routine, ISR, is atomic - this means that it cannot be halted or
interrupted by another interrupt);
2) don't use PWM functionalities on the pins listed above;
3) don't keep too much running time - this is important because a task that
runs for a long time will freeze the execution of the other tasks and the
main loop.


***********************
Supported microcontrollers

Actually the library has been successfully tested with Arduino UNO, Atmega328, 
Arduino MEGA2560 and Arduino Leonardo.
The library should compile and work versus the following:
- Attiny2313/4313
- Attiny24/44/84
- Attiny25/45/85
- Atmega344/644/1284/P
- Atmega8/A
- Atmega48/88/168/328/P
- Atmega640/1280/1281/2560/2561
- Atmega32U4 (only at 16 MHz)

Supported clocks are 1, 4, 8, and 16 MHz.

NOTE:
To use leOS on Attiny 24/44/84 microcontrollers you have to modify a file of the Tiny 
core to move the millis() and delay() functions from timer 0 to timer 1.
To do that, open the file /arduino-your_version/hardware/tiny/cores/tiny/core_build_options.c
and look for the section "Build options for the ATtiny84 processor".
A couple of rows below, change this line
#define TIMER_TO_USE_FOR_MILLIS                   1

to
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

20th revision: 2013/03/30
