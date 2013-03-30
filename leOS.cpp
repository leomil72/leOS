/* This file is part of leOS library.
   Please check the README file and the notes
   inside the leOS.h file
*/

//include required libraries
#include "leOS.h"
#include <avr/interrupt.h>


//global settings - modify them to change the leOS characteristics
const uint8_t MAX_TASKS = 9; //max allowed tasks -1 (i.e.: 9 = 10-1)
#ifdef SIXTYFOUR_MATH
volatile unsigned long long _counterMs = 0; //use a 64-bit counter so it will overflow after 584,942,417 years!
#else
volatile unsigned long _counterMs = 0; //use a 32bit counter, so max intervals cannot exceed 49.7 days
#endif
//set your max interval here (max 2^32-1) - default 3600000 (1 hour)
#define MAX_TASK_INTERVAL 3600000UL


//global variables
#if defined(ATMEGAxU)
volatile unsigned int _starter = 0;
#else
volatile uint8_t _starter = 0;
#endif
uint8_t _initialized;


//tasks variables
struct leOS_core {
    void (*taskPointer)(void); //used to store the pointers to user's tasks
    volatile unsigned long userTasksInterval; //used to store the interval between each task's run
//used to store the next time a task will have to be executed
#ifdef SIXTYFOUR_MATH
    volatile unsigned long long plannedTask;
#else
    volatile unsigned long plannedTask;
#endif
    volatile uint8_t taskIsActive; //used to store the status of the tasks
};
leOS_core tasks[MAX_TASKS];
volatile uint8_t _numTasks; //the number of current running tasks


//class constructor
leOS::leOS(void) {
	_initialized = 0;
}


//class initialization
void leOS::begin(void) {
    setTimer();
	_initialized = 1;
	_numTasks = 0;
}


//halt the scheduler
void leOS::haltScheduler(void) {
	SREG &= ~(1<<SREG_I);
#if defined (ATMEGAx8) || defined (ATMEGAx4) || defined (ATMEGAx0)
    TIMSK2 &= ~(1<<TOIE2);
#elif defined (ATMEGA8)
    TIMSK &= ~(1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
    TIMSK &= ~(1<<TOIE0);
#elif defined (ATTINYx4)
    TIMSK0 &= ~(1<<TOIE0);
#elif defined (ATMEGAxU)
	TIMSK3 &= ~(1<<TOIE3);
#endif
    SREG |= (1<<SREG_I);
}


//restart the scheduler
void leOS::restartScheduler(void) {
	if (_initialized) {
		SREG &= ~(1<<SREG_I);
#if defined (ATMEGAx8) || defined (ATMEGAx4) || defined (ATMEGAx0)
		TIMSK2 |= (1<<TOIE2);
#elif defined (ATMEGA8)
		TIMSK |= (1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
		TIMSK |= (1<<TOIE0);
#elif defined (ATTINYx4)
		TIMSK0 |= (1<<TOIE0);
#elif defined (ATMEGAxU)
		TIMSK3 |= (1<<TOIE3);
#endif
		SREG |= (1<<SREG_I);
	}
}


//add a task to the scheduler
uint8_t leOS::addTask(void (*userTask)(void), unsigned long taskInterval, uint8_t taskStatus) {
	if ((_initialized == 0) || (_numTasks == MAX_TASKS)) { //max number of allowed tasks reached
		return 1; 
	}

	if ((taskInterval < 1) || (taskInterval > MAX_TASK_INTERVAL)) { 
		taskInterval = 50; //50 ms by default
	}
    //check if taskStatus is valid
    if (taskStatus > SCHEDULED_IMMEDIATESTART) { 
        taskStatus = SCHEDULED;
    }
    //add the task to the scheduler
    SREG &= ~(1<<SREG_I); //halt the scheduler
	tasks[_numTasks].taskPointer = *userTask;
	tasks[_numTasks].taskIsActive = taskStatus & 0b00000011; //I get only the first 2 bits - I don't need the IMMEDIATESTART bit
	tasks[_numTasks].userTasksInterval = taskInterval;
	//no wait if the user wants the task up and running once added...
    //...otherwise we wait for the interval before to run the task
	tasks[_numTasks].plannedTask = _counterMs + ((taskStatus & 0b00000100)? 0 : taskInterval);
	_numTasks++;
    SREG |= (1<<SREG_I); //restart the scheduler
    return 0;
}


//pause a specific task
uint8_t leOS::pauseTask(void (*userTask)(void)) {
    return (setTask(userTask, 0));
}            
	

//restart a specific task
uint8_t leOS::restartTask(void (*userTask)(void)) {
    return (setTask(userTask, 1));
}


//modify an existing task
uint8_t leOS::modifyTask(void (*userTask)(void), unsigned long taskInterval, uint8_t oneTimeTask) {
    if ((oneTimeTask < SCHEDULED) && (oneTimeTask > ONETIME)) {
        oneTimeTask = NULL;
    }

    //modify the task into the scheduler
    SREG &= ~(1<<SREG_I); //halt the scheduler
    uint8_t tempI = 0;
    uint8_t _done = 1;
	do {
		if (tasks[tempI].taskPointer == *userTask) { //task found
            tasks[tempI].userTasksInterval = taskInterval;
            if (oneTimeTask != NULL) {
                tasks[tempI].taskIsActive = oneTimeTask;
            }
            tasks[tempI].plannedTask = _counterMs + taskInterval;
            _numTasks++;
            break;
            _done = 0;
        }
        tempI++;
    } while (tempI < _numTasks);
    SREG |= (1<<SREG_I); //restart the scheduler
    
	return _done;
	
}


//manage the tasks' status
uint8_t leOS::setTask(void (*userTask)(void), uint8_t tempStatus, unsigned long taskInterval) {
    if ((_initialized == 0) || (_numTasks == 0)) {
		return 1;
	}
    
    SREG &= ~(1<<SREG_I); //halt the scheduler
	uint8_t tempI = 0;
	do {
        if (tasks[tempI].taskPointer == *userTask) {
            tasks[tempI].taskIsActive = tempStatus;
            if (tempStatus == SCHEDULED) { 
				if (taskInterval == NULL) {
					tasks[_numTasks].plannedTask = _counterMs + tasks[tempI].userTasksInterval;
				} else {
					tasks[_numTasks].plannedTask = _counterMs + taskInterval;
				}
			}
            break;
        } else {
            tempI++;
    }
	} while (tempI < _numTasks);
    SREG |= (1<<SREG_I); //restart the scheduler
    return 0;
}    


//remove a task from the scheduler
uint8_t leOS::removeTask(void (*userTask)(void)) {
	if ((_initialized == 0) || (_numTasks == 0)) {
		return 1;
	}
    
    SREG &= ~(1<<SREG_I); //halt the scheduler
	uint8_t tempI = 0;
	do {
		if (tasks[tempI].taskPointer == *userTask) {
            if ((tempI + 1) == _numTasks) { 
                _numTasks--;
            } else if (_numTasks > 1) {
                for (uint8_t tempJ = tempI; tempJ < _numTasks; tempJ++) {
                    tasks[tempJ].taskPointer = tasks[tempJ + 1].taskPointer;
                    tasks[tempJ].taskIsActive = tasks[tempJ + 1].taskIsActive;
                    tasks[tempJ].userTasksInterval = tasks[tempJ + 1].userTasksInterval;
                    tasks[tempJ].plannedTask = tasks[tempJ + 1].plannedTask;
                }
                _numTasks -= 1;
            } else {
                _numTasks = 0;
            }
			break;
		} else {
			tempI++;
		}
	} while (tempI < _numTasks);
    SREG |= (1<<SREG_I); //restart the scheduler
    return 0;
}


//check if a task is running
uint8_t leOS::getTaskStatus(void (*userTask)(void)) {
	if ((_initialized == 0) || (_numTasks == 0)) {
		return -1;
	}
    
    uint8_t tempJ = 255; //return 255 if the task was not found (almost impossible)
    SREG &= ~(1<<SREG_I); //halt the scheduler
	uint8_t tempI = 0;
    //look for the task
	do {
		if (tasks[tempI].taskPointer == *userTask) {
            //return its current status
            tempJ = tasks[tempI].taskIsActive; 
            break;
        }
        tempI++;
    } while (tempI < _numTasks);
    SREG |= (1<<SREG_I); //restart the scheduler
    return tempJ; //return the task status
}


/*
    **************************************************************
    * WARNING - The following code contains the core of leOS:    *
    * do not modify it unless you exactly know what you're doing *
    **************************************************************
*/

//ISR (Interrupt Service Routine) called by the timer's overflow:
//interrupt-driven routine to run the tasks
#if defined (ATMEGAx8) || defined (ATMEGA8) || defined (ATMEGAx4) || defined (ATMEGAx0)
ISR(TIMER2_OVF_vect) {
    TCNT2 = _starter;
#elif defined (ATTINYx313)
ISR(TIMER0_OVF_vect) {
    TCNT0 = _starter;
#elif defined (ATTINYx4) || defined (ATTINYx5)
ISR (TIM0_OVF_vect) {
    TCNT0 = _starter;
#elif defined (ATMEGAxU)
ISR (TIMER3_OVF_vect) {
    TCNT3 = _starter;
#endif

    _counterMs++; //increment the ms counter

    //this is the scheduler - it checks if a task has to be executed
	uint8_t tempI = 0;	
	do {
		if (tasks[tempI].taskIsActive > 0 ) { //the task is running  
            //check if it's time to execute the task
#ifdef SIXTYFOUR_MATH
			if (_counterMs > tasks[tempI].plannedTask) { 
#else
            if ((long)(_counterMs - tasks[tempI].plannedTask) >=0) { //this trick overrun the overflow of _counterMs
#endif
				tasks[tempI].taskPointer(); //call the task
                //if it's a one-time task, than it has to be removed after running
                if (tasks[tempI].taskIsActive == ONETIME) { 
                    if ((tempI + 1) == _numTasks) { 
                        _numTasks--;
                    } else if (_numTasks > 1) {
                        for (uint8_t tempJ = tempI; tempJ < _numTasks; tempJ++) {
                            tasks[tempJ].taskPointer = tasks[tempJ + 1].taskPointer;
                            tasks[tempJ].taskIsActive = tasks[tempJ + 1].taskIsActive;
                            tasks[tempJ].userTasksInterval = tasks[tempJ + 1].userTasksInterval;
                            tasks[tempJ].plannedTask = tasks[tempJ + 1].plannedTask;
                        }
                        _numTasks -= 1;
                    } else {
                        _numTasks = 0;
                    }
                } else {
                    //let's schedule next start
                    tasks[tempI].plannedTask = _counterMs + tasks[tempI].userTasksInterval;
                }
			}
		}
	tempI++;
	} while (tempI < _numTasks);
}


//
//private methods
//

void leOS::setTimer() {
    float prescaler = 0.0;

    //halt all the interrupts
    SREG &= ~(1<<SREG_I);
#if defined (ATMEGAx8) || defined (ATMEGAx4) || defined (ATMEGAx0)
    //during setup, disable all the interrupts based on timer
    TIMSK2 &= ~((1<<TOIE2) | (1<<OCIE2A) | (1<<OCIE2B));
    //prescaler source clock set to internal Atmega clock (asynch mode)
    ASSR &= ~(1<<AS2);
    //this sets the timer to increment the counter until overflow
    TCCR2A &= ~((1<<WGM21) | (1<<WGM20)); 
    TCCR2B &= ~(1<<WGM22);
    //the following code sets the prescaler depending on the system clock
    if (F_CPU == 16000000UL) {   // prescaler set to 64
        TCCR2B |= (1<<CS22);
        TCCR2B &= ~((1<<CS21) | (1<<CS20));
        prescaler = 64.0;
    } else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) { // prescaler set to 32
        TCCR2B &= ~(1<<CS22); 
        TCCR2B |= ((1<<CS21) | (1<<CS20)); 
        prescaler = 32.0;
    } else if (F_CPU == 1000000UL) { // prescaler set to 8
        TCCR2B &= ~((1<<CS22) | (1<<CS20));
        TCCR2B |= (1<<CS21);
        prescaler = 8.0;
    }
#elif defined (ATTINYx5) || defined (ATTINYx313)
    //during setup, disable all the interrupts based on timer 0
    TIMSK &= ~((1<<TOIE0) | (1<<OCIE0A) | (1<<OCIE0B));
    //normal mode: counter not connected to external pins
    TCCR0A &= ~((1<<COM0A0) | (1<<COM0A1));
    //this sets the timer to increment the counter until overflow
    TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
    TCCR0B &= ~(1<<WGM02);
    //the following code sets the prescaler depending on the system clock
    if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
        TCCR0B &= ~(1<<CS02);
        TCCR0B |= ((1<<CS01) | (1<<CS00));
        prescaler = 64.0;
    } else if (F_CPU == 1000000UL) { // prescaler set to 8
        TCCR0B &= ~((1<<CS02) | (1<<CS00));
        TCCR0B |= (1<<CS01);
        prescaler = 8.0;
    }
#elif defined (ATTINYx4)
    //on Attinyx4 we must use the timer 0 because timer1 is a 16 bit counter
	
    //during setup, disable all the interrupts based on timer 0
    TIMSK0 &= ~((1<<TOIE0) | (1<<OCIE0A) | (1<<OCIE0B));
    //normal mode: increment counter until overflow & disconnect timer from pins
    TCCR0B &= ~(1<<WGM02);
    TCCR0A &= ~((1<<WGM01) | (1<<WGM00) | (1<<COM0A0) | (1<<COM0A1));
    //the following code sets the prescaler depending on the system clock
    if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
        TCCR0B &= ~(1<<CS02);
        TCCR0B |= ((1<<CS01) | (1<<CS00));
        prescaler = 64.0;
    } else if (F_CPU == 1000000UL) { // prescaler set to 8
        TCCR0B &= ~((1<<CS02) | (1<<CS00));
        TCCR0B |= (1<<CS01);
        prescaler = 8.0;
    }
#elif defined (ATMEGA8)
    //during setup, disable all the interrupts based on timer2
    TIMSK &= ~((1<<TOIE2) | (1<<OCIE2));
    //normal mode: counter incremented until overflow
    TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
    //prescaler source clock set to internal Atmega clock (synch mode)
    ASSR &= ~(1<<AS2);
	
    if (F_CPU == 1600000UL) {	// prescaler set to 64
        TCCR2 |= (1<<CS22);
        TCCR2 &= ~((1<<CS21) | (1<<CS20));
        prescaler = 64.0;
    } else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) {	// prescaler set to 32
        TCCR2 &= ~(1<<CS22);
        TCCR2 |= ((1<<CS21) | (1<<CS20));
        prescaler = 32.0;
    } else if (F_CPU == 1000000L) { // prescaler set to 8
        TCCR2 |= (1<<CS21);
        TCCR2 &= ~((1<<CS22) | (1<<CS20));
        prescaler = 8.0;
    }
#elif defined (ATMEGAxU)
    //during setup, disable all the interrupts based on timer3
    TIMSK3 &= ~((1<<TOIE3) | (1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C) | (1<<ICIE3));
    //normal mode: counter incremented until overflow, prescaler set to /1
    TCCR3A &= ~((1<<WGM31) | (1<<WGM30));
    TCCR3B &= ~((1<<WGM33) | (1<<WGM32) | (1<<CS32) | (1<<CS31));
    TCCR3B |= (1<<CS30);
    //TCCR3B = 1;
    prescaler = 1.0;
#endif

    //set the initial value of the counter depending on the prescaler
#if defined (ATMEGAxU) 
	_starter = 65536 - (uint16_t)((float)F_CPU * 0.001 / prescaler); //for 16 MHz: 49536
#else
    _starter = 256 - (int)((float)F_CPU * 0.001 / prescaler); //for 16 MHz: 6
#endif

    //start the counter
#if defined (ATMEGAx8) || defined (ATMEGAx4) || defined (ATMEGAx0)
    TCNT2 = _starter;
    TIMSK2 |= (1<<TOIE2);
#elif defined (ATMEGA8)
    TCNT2 = _starter;
    TIMSK |= (1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
    TCNT0 = _starter;
    TIMSK |= (1<<TOIE0);
#elif defined (ATTINYx4)
    TCNT0 = _starter;
    TIMSK0 |= (1<<TOIE0);
#elif defined (ATMEGAxU)
    TCNT3 = _starter;
	TIMSK3 |= (1<<TOIE3);
#endif
    SREG |= (1<<SREG_I);
}

