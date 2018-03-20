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

	struct loaded{
	    unsigned char gps_data;
	    struct timeval location_time;
	}loaded;

	struct event_buffer
	{
		double location_before;
		double location_of_event;
		double location_after;
		double time_before;
		double time_of_event;
		double time_after;
	}event_buffer;

int pipe_for_bonus[2];


	void set_thread_priority( int change_priority);
	//void *button_push_collect_time( void * input );
	void *collect_button_information(void * inputs);// tired to runs as a sperate process by using a fork
	void *calculate_event( void * event);//thread for getting each event data
	void *print_through_pipe( void *current_event);// receives the information through a pipe
	//define GPIO ports
	#define Button1 27

	int main(int argc, const char * argv[]) {
	    int pipe_to_gps; //for pipe from GPS exec.
		struct loaded;
	  //  unsigned char location_data;
	   // struct timeval data_time;
	   // Realtime();
	    pthread_t check_button_press, pipe_print, read_time_information;
	    set_thread_priority(15);
	    if((pipe_to_gps = open("/tmp/N_pipe1", O_RDONLY))< 0)
		{
		    printf("Connection to GPS was not made - N_pipe1 error\n");
		    exit(-1);
		}
        ---------- trying to get bonus-----
        if(pipe(pipe_for_bonus)<0){
            printf("Pipe Creation Error\n");
            exit(-1);
        }
-------------
	    printf("Connection to GPS device was successful\n");//only if both progarms are running at
	    
	 //   	pthread_create(&check_button_press, NULL, &button_push_collect_time, NULL);
		pthread_create(&read_time_information, NULL, &collect_button_information, NULL);
		pthread_create(&pipe_print, NULL, &print_through_pipe, NULL);
	    
		 while(1)
		 {
			if( read(pipe_to_gps, &loaded.gps_data, 1 ) < 0)
			{
			  printf("error READING N_pipe1");
			}
			// read(pipe_to_gps, &loaded.gps_data, 1 ); // reading the pipe from GPS
			gettimeofday(&loaded.location_time, NULL);
		
			// printf("location:%hhu \n",(int) loaded.gps_data );//trying to test functioning
		
			gps_data_buffer.gps_data = loaded.gps_data;
			gps_data_buffer.location_time = loaded.location_time;
		}
	   
	//    	pthread_join( check_button_press, NULL);
		pthread_join( read_time_information, NULL);
		pthread_join( pipe_print, NULL);
	   
	    return 0;
	    
	}

	/*void *button_push_collect_time( void * input )
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
	      //fork may need to go before pipe always we will be waiting for the other end of the pipe
		  pid_t pid;
		//forking to run a seperate proccess to have the buttons press piped to it
		if( pid = fork() < 0 )
		{	
			printf("Fork FAILED\n");
			exit(-1);
		}
		else
		{
		    printf("fork work\n");
		     collect_button_information();

		}

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
	       pthread_exit(NULL);
	}
	*/      
	void *collect_button_information( void * input)
	    {
	       // set_thread_priority(5);
		int collect_button_information;//= *((int*)input);
		//set_thread_priority(10); should need to set a thread priority 
		struct timeval collected;
		//if you really forgot a while loop your a dumby 
		pthread_t event[10], print_via_pipe;
		int i;

		 printf("Reading from the button press setup\n");
		
		if((collect_button_information = open("N_pipe2", O_RDONLY)) < 0)
				{
					printf("error opening pipe in collect_ button_information\n");
			exit(-1);
				}

	while (1)
	{
		if( read(collect_button_information, &collected, sizeof(collected)) < 0)
        		{
           		 printf("Error reading the information from the button press\n");
                 exit(-1);
        		}
	//dynamicall creating a thread 
	pthread_create(&event[i], NULL, &calculate_event,(void*)&collected);
    pthread_create(&print_via_pipe, NULL, &print_through_pipe, NULL);//***** bonus
	pthread_detach(event[i]);	
	if( i == 9)
	{
	i = 0;
	}
//	printf("time of the button push: %lf: i value is:%d\n ", (double)collected.tv_usec, i); was used for error checking
        printf("reading collected_button_information occured\n");
        }

        pthread_exit(NULL);
    }

void *calculate_event( void * event )
{	// one confusing thing to follow 
	set_thread_priority(5);
	struct gps_data_buffer before_event = gps_data_buffer;
	struct timeval event_time = *(struct timeval*) event;
	struct gps_data_buffer after_event;
	struct event_buffer current_event;
	//int pipe_for_bonus;

//	printf("data locaiton %d\n ", (int) gps_data_buffer.gps_data); used for error checking the double increment 
	while( before_event.gps_data  == gps_data_buffer.gps_data)
	{
	usleep(10);//sleeping to allow it to wait short bit trying to fix the error of double press crashing
	} 
//	printf("data locaiton %d\n ", (int) loaded.gps_data); used for error checking the double increment 
		
	printf("calculating the event data\n\n\n");
	after_event.gps_data = (double) gps_data_buffer.gps_data;
	after_event.location_time = gps_data_buffer.location_time;

	current_event.time_before = (double) before_event.location_time.tv_sec + ((double) before_event.location_time.tv_usec/1000000);
	current_event.time_of_event = (double) event_time.tv_sec + ((double) event_time.tv_usec/1000000);
//	printf("time of even %f\n", current_event.time_of_event);
	current_event.time_after = (double) after_event.location_time.tv_sec + ((double) after_event.location_time.tv_usec/1000000);
	
	current_event.location_before = (double) before_event.gps_data;
	current_event.location_after = (double) after_event.gps_data;
	current_event.location_of_event = (double) (((current_event.time_of_event - current_event.time_before)/(current_event.time_after - current_event.time_before))*(current_event.location_after - current_event.location_before)) + current_event.location_before;
/*
	printf("location_before: %f time of event:%f\n ", current_event.location_before, current_event.time_before);
	printf("location of event %f, time of event%f \n" , current_event.location_of_event, current_event.time_of_event);
	printf("Location after: %f,  time of event %f \n", current_event.location_after, current_event.time_after);
*/	
	/*if((pipe_for_bonus= open("N_pipe3", O_WRONLY)) < 0)
				{
					printf("error opening pipe in collect_ button_information\n");
			exit(-1);
	*/			}


		
		if( write(pipe_for_bonus[1], &current_event, sizeof(current_event)) < 0)
        		{
           		 printf("Error reading the information from the button press\n");
                 exit(-1);
        		}


	pthread_exit(NULL);

}

//Adding the bonus here 
void *print_through_pipe( void *current_event)
{
    struct event_buffer current_event;
	/*if((pipe_for_bonus = open("N_pipe3", O_RDONLY)) < 0)
				{
					printf("error opening pipe in collect_ button_information\n");
					exit(-1);
				}
     */
    printf("****pipe connection for printing setup****\n");
	while(1)
    {
		if( read( pipe_for_bonus[0], &current_event, sizeof(current_event)) < 0)
        		{
           		 printf("Error reading the information from the button press\n");
               		  exit(-1);
        		}
	printf("printing from the piped connect:\n");
        printf("location_before: %lf , time of event:%lf\n ", current_event.location_before, current_event.time_before);
        printf("location of event %lf , time of event: %lf \n" , current_event.location_of_event, current_event.time_of_event);
        printf("Location after: %lf,  time of event: %lf \n", current_event.location_after, current_event.time_after);
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
       


