//
//  main.c
//  lab2-week2
//
//  Created by Nicholas Bouckaert on 2/13/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h> //library only exsist on the PI
#include <time.h>

#define first "first.txt"
#define second "second.txt"
#define combined "combined.txt"

char buffer[100];//buffer for text to be stored in 
char song[20][100];//buffer for text to be writi too.

#define MY_PRIORITY 49;

void *odd_from_file( void *file);
void *even_from_file( void *file);
void *write_to_file( void *file);

int main (void)
{
    
    FILE *firstfile;
    firstfile = fopen(first, "r");//abiltiy to read from file 
    
    FILE *secondfile;
    secondfile = fopen(second, "r");//ability to read from file 
    
    FILE *combinedfile;
    combinedfile = fopen( combined , "w+");//ability to write to file
    
    pthread_t readodd, readeven, writetofile;//create threads

    pthread_create(&readodd, NULL, odd_from_file,(void*) firstfile);
    pthread_create(&writetofile, NULL, write_to_file, (void*) combinedfile);
    pthread_create(&readeven, NULL, even_from_file, (void*) secondfile );
   // pthread_create(&writetofile, NULL, write_to_file, (void*)combinedfile);
    
    pthread_join(readeven, NULL);
    pthread_join(readodd, NULL);
    pthread_join(writetofile, NULL);

	int i; 
	for( i = 0; i < 20 ; i++)
	{
	puts(song[i]);
	}

    fclose(firstfile);
    fclose(secondfile);
    fclose(combinedfile);
    return 0;
}
int set_for_Realtime( void )//set priority for execution 
{
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;

    if(sched_setscheduler( 0, SCHED_FIFO, &param) != 0)
    {
    printf("Real time is not accessable\n");
    }
    return 0;
}
void *odd_from_file(void *file)// set to read from first.txt
{
     set_for_Realtime();
    
   int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;//set timer and interval 
        itval.it_interval.tv_sec = 0;//Peroid_second;
        itval.it_interval.tv_nsec = 4000000;//Peroid_nsecond;
        itval.it_value.tv_sec = 0;//inittime_second;
        itval.it_value.tv_nsec = 7000000;//initime_nsecond;
    
    if(-1 ==  timerfd_settime (timer_fd, 0, &itval, NULL))
	{
		printf("timer failed to create on odd");
	}
    
    uint64_t num_periods = 0;
    int i;
       for( i  = 0; i < 10; i++)
    {	
	if( i > 2)
		{
		printf("Line Content: odd\n");	
       		 buffer[0] == '\0'; 
        	fgets(buffer, 100, file);
        //	printf("%s\n", buffer); // testing the buffer to insure something is being read in
		}
         read(timer_fd, &num_periods, sizeof(num_periods));
        if(num_periods > 1)
        	{
        	    puts("MISSED WINDOW IN ODD READ");
       		 }			
    }
    return NULL;
    
}

void *even_from_file(void *file)//reading from the second.txt
{
    set_for_Realtime();
    
    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    struct itimerspec itval;//set time to begin and interval
   
        itval.it_interval.tv_sec = 0;//Peroid_second;
        itval.it_interval.tv_nsec = 4000000;//Peroid_nsecond;
        
        itval.it_value.tv_sec = 0;//inittime_second;
       itval.it_value.tv_nsec = 9000000;//initime_nsecond;
    
       if(-1 == timerfd_settime (timer_fd, 0, &itval, NULL))
	{
		printf("timer failed to create in even");
	}

    uint64_t num_periods = 0;
    int i = 0;
    for( i  = 0; i < 10; i++)
    {
	if( i > 2)
	{
		printf("Line Content : Even\n");
      		buffer[0] == '\0';
     		fgets(buffer, 100, file);	 
		// printf("%s\n", buffer); // testing the buffer to insure something is being read in
        }
       		 read(timer_fd, &num_periods, sizeof(num_periods));
       	 	if(num_periods > 1)
            	{
                	puts("MISSED WINDOW IN EVEN READ");
                	exit(1);
           	 }
    	}
    return NULL;
}

void *write_to_file( void* file )// writing to file and buffer at this point 
{
    set_for_Realtime();	

	pthread_mutex_t mutex;

    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;//set timer and interval 
        itval.it_interval.tv_sec = 0;//Peroid_second;
        itval.it_interval.tv_nsec = 2000000;//Peroid_nsecond;
        
        itval.it_value.tv_sec = 0;//inittime_second;
        itval.it_value.tv_nsec = 6000000; //initime_nsecond;
    
    if( -1 == timerfd_settime (timer_fd, 0, &itval, NULL))
	{
		printf("timer failed to create");
	
	}
    uint64_t num_periods = 0;
    int i;
    printf("******************write to file begins************\n\n\n");
     for( i  = 0; i < 20; i++)
    {
	if( i > 2)
	{
	pthread_mutex_lock(&mutex);
	strcpy( song[i], buffer);
      // printf("1 %s\n", buffer); // testing the buffer to insure something is being read in
        printf("writin to buffer: %d\n", i); 
      	fprintf(file, "%s\n", buffer);
        }
	read(timer_fd, &num_periods, sizeof(num_periods));
        if(num_periods > 1)
        	{
            	   puts("MISSED WINDOW IN WRITING TO FILE");
         	   exit(1);
        	}
	pthread_mutex_unlock(&mutex);
    	}
   return NULL;
}
