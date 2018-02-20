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
/*
#include <wiringPi.h>
#include <sys/timer.h>
#include "ece4220lab3.c"

*/

#define MY_PRIORITY 51
void set_thread_priority( int change_priority );

int main ( void )
{
    // WILL NEED SUDO TO RUN WITH PRIORITY CHANGE
    set_thread_priority(0);
    
    //Setting up the wiring pi
    wiringPiSetupGpio();
    pinmode( 2, OUTPUT);
    pinmode( 2, OUTPUT);
    pinmode( 4, OUTPUT);
    pinmode( 5, OUTPUT);
    pinmode( 16, INPUT);
    pullUpDnControl( 16, PUD_DOWN); // sett button press to zero
    
    digitalWrite(2,0);// set lights to off
    digitalWrite(3,0);
    digitalWrite(4,0);
    digitalWrite(5,0);
    
    
    
    
    
    
    
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
