// Nicholas Bouckaert
// lab3 week2 

#include <stdio.h> 
#include <unistd.h> 
#include <sched.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <stdint.h> 
#include <time.h> 
#include <pthread.h> 
#include <wiringPi.h>
#include <sys/timerfd.h>
#include "ece4220lab3.h"
#include <semaphore.h>

#define Red 8
#define Yellow 9
#define Green 7
#define Button1 27
#define H HIGH 
#define L LOW
#define MY_PRIORITY 51

void set_thread_priority( int change_priority);
void *walksignal( void *change_priority);
void *yellowsignal(void *change_priority);
void *greensignal( void *change_priority);
	
sem_t sem; 

int main ( void) 
{
	
	//setting up the wiring pi portion 
	wiringPiSetup();
	pinMode( Red, OUTPUT);
	pinMode( Yellow, OUTPUT);
	pinMode(Green, OUTPUT);
	pinMode(Button1, INPUT);
	pullUpDnControl( Button1, PUD_DOWN);
		
	digitalWrite( Red, L);
	digitalWrite( Yellow, L);
	digitalWrite( Green, L);
	int green_priority, yellow_priority, walk_priority; 

	printf("Please enter a priority for the Following lights\n");
	printf("green:");
	scanf("%d", &green_priority);
	printf("\nyellow:");
	scanf("%d", &yellow_priority);
	printf("\nwalk:");
	scanf("%d", &walk_priority);

	pthread_t Greenlight, Yellowlight, Walklight;
	sem_init(&sem, 0, 1);
	
	pthread_create( &Greenlight, NULL, &greensignal, (void*)&green_priority);
	pthread_create( &Yellowlight, NULL, &yellowsignal, (void*)&yellow_priority);
	pthread_create( &Walklight, NULL, &walksignal, (void*)&walk_priority);

	pthread_join(Greenlight, NULL);
	pthread_join(Yellowlight, NULL);
	pthread_join(Walklight, NULL);

	sem_destroy(&sem);
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
void *yellowsignal ( void *change_priority)
{
	int set_priority = *((int*) change_priority);
	set_thread_priority(set_priority);

	while(1)//endless loop
	{
		sem_wait(&sem);
		digitalWrite( Yellow, H);
		sleep(2);
		digitalWrite( Yellow, L);
		sem_post(&sem);
		usleep(10);
	}

	pthread_exit(NULL);
	
}


void *greensignal ( void *change_priority)
{
	int set_priority = *((int*) change_priority);
	set_thread_priority(set_priority);

	while(1)//endless loop
	{
		sem_wait(&sem);
		digitalWrite( Green, H);
		sleep(2);
		digitalWrite( Green, L);
		sem_post(&sem);
		usleep(10);
	}

	pthread_exit(NULL);
	
}

void *walksignal ( void *change_priority)
{
	int set_priority = *((int*) change_priority);
	set_thread_priority(set_priority);

	while(1)//endless loop
	{
		if( check_button() == 1 ) // will only work if the button has been pushed
			{	
				sem_wait(&sem);
				digitalWrite( Red, H);
				clear_button();
				sleep(2);
				digitalWrite( Red, L);
				sem_post(&sem);
				usleep(10);
			}	
	
	}	
	pthread_exit(NULL);
	
}					



