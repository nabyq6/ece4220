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
// included and works only on the pi
#include <wiringPi.h>
#include <semaphore.h>
#include <sys/timerfd.h>
#include "ece4220lab3.h"
 

#define MY_PRIORITY 51

struct gps_data_buffer{
    unsigned char gps_data;
    struct timeval location_time;
}gps_data_buffer;




void set_thread_priority( int change_priority);
void *button_push_collect_time( void * input );
void collect_button_information();// tired to runs as a sperate process by using a fork

//define GPIO ports
#define Button1 27

int main(int argc, const char * argv[]) {
    int pipe_to_gps; //for pipe from GPS exe.
    unsigned char location_data;
    struct timeval data_time;
   // Realtime();
    pthread_t check_button_press, read_time_information;
    
    if((pipe_to_gps = open("/tmp/N_pipe1", O_RDONLY))< 0)
        {
            printf("Connection to GPS was not made - N_pipe1 error\n");
            exit(-1);
        }
    
    printf("Connection to GPS device was successful\n");//only if both progarms are running at
    
    	pthread_create(&check_button_press, NULL, &button_push_collect_time, NULL);
//	pthread_create(&read_time_information, NULL, &collect_button_information, NULL);
    
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
   
    	pthread_join( check_button_press, NULL);
//	pthread_join( read_time_information, NULL);
   
    return 0;
    
}

void *button_push_collect_time( void * input )
{
    set_thread_priority(5);//hard cording thread prioirty to be 55
    struct timeval Button_time;
    int time_collection_pipe, pipe_setup;
    
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;// setting timer to run thread every 75ms
        itval.it_interval.tv_sec = 1; //Period_seconds
        itval.it_interval.tv_nsec = 0;//peroid in nano
        itval.it_value.tv_sec = 0;// init time in seconds
        itval.it_value.tv_nsec = 75000000;// init time in nano
    
    if( -1 == timerfd_settime( timer_fd, 0, &itval, NULL))
        {
            printf("timer failed to create for the button push\n");
            exit(-1);
        }
       //setting up to read from the first button
       wiringPiSetup();
       pinMode( Button1, INPUT);
       pullUpDnControl( Button1, PUD_DOWN);
       
       uint64_t num_peroids = 0;
       read( timer_fd, &num_peroids, sizeof( num_peroids));
	
	pipe_setup = system("mkfifo /tmp/N_pipe2");
	
	if(( time_collection_pipe = open("/tmp/N_pipe2", O_WRONLY)) < 0)
		{
			printf("error opening pipe in button_push_collect_time thread\n");
            exit(-1);
		}
    
        pid_t pid = fork();//forking to run a seperate proccess to have the buttons press piped to it
        if( pid == 0 )
        {
            collect_button_information();
        	printf("Fork worked\n");
        }
    
    while(1)
    {
        if(check_button() == 1)
            {
                gettimeofday(&Button_time, NULL);
                printf("Button Pressed\n");
                fflush(stdout);
                if(write(time_collection_pipe, &Button_time, sizeof(Button_time)) < 0)
                {
                    printf("Error writing time to pipe in button_push_collect_time thread \n");
                    exit(-1);
                }
                clear_button();
            }
        //uint64_t num_periods = 0;                    //Wait one period
       // read(timer_fd, &num_periods, sizeof(num_periods));
        
    }
       pthread_exit(NULL);
}
       
void collect_button_information( void )
    {
        int collect_button_information;//= *((int*)input);
        //set_thread_priority(10); should need to set a thread priority 
        struct timespec collected;
        printf("Reading from the button press\n");
       
		 if((collect_button_information = open("/tmp/N_pip2", O_RDONLY)) < 0)
			{
				printf("error opening pipe in collect_ button_information\n");
                exit(-1);
			}

		if( read(collect_button_information, &collected, sizeof(collect_button_information)))
        		{
           		 printf("Error reading the information from the button press\n");
                 exit(-1);
        		}

        printf("reading collected_button_information occured\n");
        
        pthread_exit(NULL);
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
       


