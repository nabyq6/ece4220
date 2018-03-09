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
    int pipe_to_gps; //for pipe from GPS exe.
    unsigned char location_data;
    struct timeval data_time;
   // Realtime();
   
    
    if((pipe_to_gps = open("N_pipe1", O_RDONLY))< 0)
        {
            printf("Connection to GPS was not made - N_pipe1 error");
            exit(-1);
        }
    
    printf("Connection to GPS device was successful");//only if both progarms are running at
    
    while(1)
    {
        if( read(pipe_to_gps, &location_data, 1 ) < 0)
        {
            printf("error READING N_pipe1");
        }
        location_data = read(pipe_to_gps, &location_data, 1 ); // reading the pipe from GPS
        gettimeofday(&data_time, NULL);
        
        printf("location:%hhu\n time:%ld",location_data, data_time.tv_sec );//trying to test functioning
        
        gps_data_buffer.gps_data = location_data;
        gps_data_buffer.location_time = data_time;
    }
    
    
}

/*
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
 */
