//
//  main.c
//  lab4
//
//  Created by Nicholas Bouckaert on 3/6/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
/* included and works only on the pi
#include <wiringPi.h>
#include <semaphore.h>
#include <sys/timerfd.h>
#include "serial_ece4220.h"
 */

#define MY_PRIORITY 51

void set_thread_priority( int change_priority);

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}

void set_thread_priority( int change_priority)//set priority for every
{
    struct sched_param param;
    param.sched_priority = MY_PRIORITY + change_priority;
    if( sched_setscheduler( 0, SCHED_FIFO, &param) < 0)
    {
        printf("Real Time is not accessed\n");
        exit(1);
    }
    
}
