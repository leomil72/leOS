/*
	leOS.h - <l>ittle <e>mbedded <O>perating <S>ystem 
	This is not a complete OS nor an RTOS as you usually know but
	it's a simple scheduler to schedule little works in background, so that 
	you	can forget about them. It's designed for Arduino and other common
	Atmel microcontroller (for the complete list of the supported MCUs
    please refer to the README.txt file).

	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
    
    The latest version of this library can be found at:
    http://www.leonardomiliani.com/
    	
	Current version: 1.1.0 - 2013/03/15
    (for a complete history of the previous versions, see the README file)
    
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


//library version
#define leOS_VERSION 111


//this library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


//uncomment this line if you want to use 64-bit math (more Flash consumption) - default is 32-bit math
//(read the README.txt file for more info about the use of 32- vs. 64-bit math)
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
#elif defined (__AVR_ATmega344__) || defined (__AVR_ATmega344P__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
#define ATMEGAx4
#elif defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny4313__)
#define ATTINYx313
#elif defined (__AVR_ATmega32U4__)
#define ATMEGAxU
#if (F_CPU!=16000000UL)
#error Sorry, Atmega32U4 is supported only at 16 MHz 
#endif
#else
#error Sorry, microcontroller not supported!
#endif

//check if the frequency is supported
#if ((F_CPU!=1000000UL) && (F_CPU!=4000000UL) && (F_CPU!=8000000UL) && (F_CPU!=16000000UL))
#error Sorry, clock frequency not supported!
#endif

const uint8_t PAUSED = 0;
const uint8_t SCHEDULED = 1; //0b00000001
const uint8_t SCHEDULED_IMMEDIATESTART = 5; //0b00000101
const uint8_t IMMEDIATESTART = SCHEDULED_IMMEDIATESTART; //alias for previous
const uint8_t ONETIME = 2;

class leOS {
	public: 
		//public methods
		leOS();
        void begin(void);
		uint8_t addTask(void (*)(void), unsigned long, uint8_t taskStatus=1);
		uint8_t removeTask(void (*)(void));
		uint8_t pauseTask(void (*)(void));
        uint8_t restartTask(void (*)(void));
		uint8_t modifyTask(void (*)(void), unsigned long, uint8_t oneTimeTask=NULL);
		uint8_t getTaskStatus(void (*)(void));
		void haltScheduler(void);
		void restartScheduler(void);
	private:
        //private methods
        void setTimer();
        uint8_t setTask(void (*)(void), uint8_t, unsigned long taskInterval=NULL);
};

#endif
