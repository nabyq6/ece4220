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
#include <sys/time.h>
#include <pthread.h>
/* included and works only on the pi
#include <wiringPi.h>
#include <semaphore.h>
#include <sys/timerfd.h>
#include "serial_ece4220.h"
 */

#define MY_PRIORITY 51

struct gps_data_buffer{
    unsigned char gps_data;
    struct timeval location_time;
}gps_data_buffer;




void set_thread_priority( int change_priority);

int main(int argc, const char * argv[]) {
    unsigned char location_data;
    struct timeval data_time;
    Realtime();
    
    if(pipe(N_pipe1) < 0)
        {
            printf("Connection to GPS was not made - N_pipe1 error");
            exit(-1);
        }
    
    printf("Connection to GPS device was successful");//only if both progarms are running at
    
    while(1)
    {
        if( read(N_pipe1, &location_data, 1 ) < 0)
        {
            printf("error READING N_pipe1");
        }
        location_data = read(N_pipe1, &location_data, 1 ); // reading the pipe from GPS
        gettimeofday(&data_time, NULL);
        
        printf("location:%d\n time:%d", data_time.time_t );//trying to test functioning
        
        gps_data_buffer.gps_data = location_data;
        gps_data_buffer.location_time = data_time;
    }
    
    
}

void set_thread_priority( int change_priority)//set priority for every thread - reused from lab 3
{
    struct sched_param param;
    param.sched_priority = MY_PRIORITY + change_priority;
    if( sched_setscheduler( 0, SCHED_FIFO, &param) < 0)
    {
        printf("Real Time is not accessed\n");
        exit(1);
    }
    
}
