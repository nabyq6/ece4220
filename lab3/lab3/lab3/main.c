//
//  main.c
//  lab3
//
//  Created by Nicholas Bouckaert on 2/20/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
 // libraries only on the pie

#include <wiringPi.h>
#include <sys/timerfd.h>
#include "ece4220lab3.h"

#define Red 8
#define Yellow 9 
#define Green 7
#define Button1 27
#define H HIGH
#define L LOW

#define MY_PRIORITY 51
void set_thread_priority( int change_priority );

int main ( void )
{
    // WILL NEED SUDO TO RUN WITH PRIORITY CHANGE
    set_thread_priority(0);
    
    //Setting up the wiring pi and setting pinModes to be used in execution. color names are defined above
    wiringPiSetup();
    pinMode( Red, OUTPUT);
    pinMode( Yellow, OUTPUT);
    pinMode( Green, OUTPUT);
    pinMode( Button1, INPUT); 
    pullUpDnControl( Button1, PUD_DOWN); // sett button press to zero
    
    digitalWrite( Red, LOW);// set lights all to off- blue is not off dont worry
    digitalWrite( Yellow, LOW);
    digitalWrite(Green, LOW);
	
	while( 1 ) 
	{
    		printf("Yellow\n");
                digitalWrite( Yellow, H);
	 	sleep(5);
		digitalWrite( Yellow, L);
	
		printf("Green\n");
		digitalWrite( Green, H);
		sleep(5);
		digitalWrite( Green, L);
	
		// check the button to see if it is being pressed. Could use check_button() to see if it was ever pressed instead 
		if( check_button() == 1) 
		{
		printf("Button has been pressed:red light on");
		digitalWrite(Red, H);
		sleep(5);
		clear_button();
		digitalWrite(Red, L);
		}

	}
    
    
    
    
    
    return 0;
}

void set_thread_priority( int change_priority)// will be helpfull when setting the  priority in lab 3 part 2 with # threads
{
    struct sched_param param;
    
        param.sched_priority = MY_PRIORITY + change_priority;
        if( sched_setscheduler(0, SCHED_FIFO, &param) < 0 )
        {
            printf("Real time is not accessed\n");
            exit(1);
        }
    
    
            
}
