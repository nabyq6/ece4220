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
//#include <sys/timerfd.h> library may only exsist on the PI
#include <time.h>

#define first "first.txt"
#define second "second.txt"
#define combined "combined.txt"

char buffer[100];
char file[20][100];

#define MY_PRIORITY 51;

void *odd_from_file( void *file);
void *even_from_file( void *file);
void *write_to_file( void *file);

int main (void)
{
    
    FILE *firstfile;
    firstfile = fopen(first, "r");
    
    FILE *secondfile;
    secondfile = fopen(second, "r");
    
    FILE *combinedfile;
    combinedfile = fopen( combined , "r");
    
    pthread_t readodd, readeven, writetofile;
    
    pthread_create(&readodd, NULL, odd_from_file, (void *)firstfile);
    pthread_create(&readeven, NULL, even_from_file, (void *)secondfile);
    pthread_create(&writetofile, NULL, write_to_file, (void *)combinedfile);
    
    
    pthread_join(readeven, NULL);
    pthread_join(readodd, NULL);
    pthread_join(writetofile, NULL);
    
    


    fclose(firstfile);
    fclose(secondfile);
    fclose(combinedfile);
    
}
int set_for_Realtime( void)
{
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;

    if(sched_setscheduler( 0, SCHED_FIFO, &param) < 0)
    {
    printf("Real time is not accessable");
    }
    
}
void *odd_from_file(void *file)
{
    set_for_Realtime();
    
    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;
    {
    itval.it_interval.tv_sec = //Peroid_second;
    itval.it_interval.tv_nsec = //Peroid_nsecond;
    
    itval.it_value.tv_sec = //inittime_second;
    itval.it_value.tv_nsec = //initime_nsecond;
    }itval;
    timerfd_settime (timer_fd, 0, &itval, NULL);
    uint64_t num_periods = 0;
    printf("LineContent:");
    for( i  = 0; i < 10; i++)
    {
        buffer[0] = '\0';
        fgets(buffer, 100, file);
        printf("1 %s\n", buffer); // testing the buffer to insure something is being read in
        
        read(timer_fd, &num_periods, sizeof(num_periods));
        if(num_periods > 1)
        {
            puts("MISSED WINDOW IN ODD READ");
        }
    }
    return NULL;
    
}

void *even_from_file(void *file)
{
    set_for_Realtime();
    
    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);

    struct itimerspec itval;
    {
        itval.it_interval.tv_sec = //Peroid_second;
        itval.it_interval.tv_nsec = //Peroid_nsecond;
        
        itval.it_value.tv_sec = //inittime_second;
        itval.it_value.tv_nsec = //initime_nsecond;
    }itval;
    timerfd_settime (timer_fd, 0, &itval, NULL);
    uint64_t num_periods = 0;
    int i = 0;
    printf("Line content:");
    for( i  = 0; i < 10; i++)
    {
        buffer[0] = '\0';
        fgets(buffer, 100, file);
        printf("1 %s\n", buffer); // testing the buffer to insure something is being read in
        
        read(timer_fd, &num_periods, sizeof(num_periods));
        if(num_periods > 1)
        {
            puts("MISSED WINDOW IN EVEN READ");
            exit(1);
        }
    }
    return NULL;
}

void *write_to_file( void *file )
{
    set_for_Realtime();
    
    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;
    {
        itval.it_interval.tv_sec = //Peroid_second;
        itval.it_interval.tv_nsec = //Peroid_nsecond;
        
        itval.it_value.tv_sec = //inittime_second;
        itval.it_value.tv_nsec = //initime_nsecond;
    }itval;
    timerfd_settime (timer_fd, 0, &itval, NULL);
    uint64_t num_periods = 0;
    printf("Line content:");
    for( i  = 0; i < 20; i++)
    {
        strcpy(arr[i], buffer);
        printf("1 %s\n", buffer); // testing the buffer to insure something is being read in
        
        read(timer_fd, &num_periods, sizeof(num_periods));
        if(num_periods > 1)
        {
            puts("MISSED WINDOW IN WRITING TO FILE");
            exit(1);
        }
    }
    return NULL;
}
