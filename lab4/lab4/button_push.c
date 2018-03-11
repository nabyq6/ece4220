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
//GPIO ports set 
#define Button1 27

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

int main(void) 
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
    
	 timerfd_settime( timer_fd, 0, &itval, NULL);

       //setting up to read from the first button
       wiringPiSetup();
       pinMode( Button1, INPUT);
       pullUpDnControl( Button1, PUD_DOWN);
       
       uint64_t num_peroids = 0;
       read( timer_fd, &num_peroids, sizeof( num_peroids));
		printf("at the fork\n");

	pipe_setup = system("mkfifo N_pipe2");
	 printf("got to opening pipe\n");
	if(( time_collection_pipe = open("N_pipe2", O_WRONLY)) < 0)
		{
			printf("error opening pipe in button_push_collect_time thread\n");
      			      exit(-1);
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
     return 0;
}
