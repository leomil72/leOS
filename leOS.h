/*
	leOS.h - <l>ittle <e>mbedded <O>perating <S>ystem 
	This is not a complete OS nor an RTOS as you usually know but
	it's a simple scheduler to schedule little works in background, so that 
	you	can forget about them. It's designed for Arduino and other common
	Atmel microcontroller (for the complete list refer to the README file).

	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	
	History:
    v. 0.1.3:  now a task can be added in "pause mode" - a new example sketch
    v. 0.1.2:  fixed a bug in the management of one-time pads
    v. 0.1.1:  now the user can choose between 32-bit & 64-bit math
    v. 0.1.0:  leOS now works correcty on Arduino Leonardo/Atmega32U4
    v. 0.0.8:  now the user can modify running tasks    
    v. 0.0.7:  introduced one-time tasks
	v. 0.0.6:  support for Arduino Leonardo/Atmega32U4
    v. 0.0.5:  fixed some bugs and optimized code & memory consumption
	v. 0.0.4:  use of a 64-bit counter so the scheduler will overflow after 584942417 years
    v. 0.0.3:  added methods to pause and restart a task
	v. 0.0.2:  user can now delete scheduled tasks
	v. 0.0.1:  early release - user can only add tasks

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 3.0 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

*/


#ifndef leOS_H
#define leOS_H

//Library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


//uncomment this line if you want to use 64-bit math (more Flash consumption) - default is 32-bit math
//#define SIXTYFOUR_MATH

//check if the micro is supported
#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega168P__) || defined (__AVR_ATmega328P__)
#define ATMEGAx8
#elif defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
#define ATTINYx5
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
#define ATMEGA8
#elif defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny84__)
#define ATTINYx4
#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)
#define ATMEGAx0 
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
#define ATMEGA644
#elif defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny4313__)
#define ATTINYx313
#elif defined (__AVR_ATmega32U4__)
#define ATMEGAxU
#if (F_CPU!=16000000UL)
#error Sorry, on Atmega32U4 only 16 MHz are supported
#endif
#else
#error Sorry, microcontroller not supported!
#endif

//check if the frequency is supported
#if ((F_CPU!=1000000UL) && (F_CPU!=4000000UL) && (F_CPU!=8000000UL) && (F_CPU!=16000000UL))
#error Sorry, clock frequency not supported!
#endif

const uint8_t PAUSED = 0;
const uint8_t SCHEDULED = 1;
const uint8_t ONETIME = 2;

class leOS {
	public: 
		//public methods
		leOS();
        void begin(void);
        void (*voidFuncPtr)(void);
		uint8_t addTask(void (*)(void), unsigned long, uint8_t taskStatus=1);
		uint8_t removeTask(void (*)(void));
		uint8_t pauseTask(void (*)(void));
        uint8_t restartTask(void (*)(void));
		uint8_t modifyTask(void (*)(void), unsigned long, uint8_t oneTimeTask=NULL);
		
	private:
        //private methods
        void setTimer();
        uint8_t setTask(void (*)(void), uint8_t, unsigned long taskInterval=NULL);
};

#endif
