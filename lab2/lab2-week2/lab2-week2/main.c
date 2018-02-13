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
    
    
    
    
    


    fclose(firstfile);
    fclose(secondfile);
    fclose(combinedfile);
    
}
void *odd_from_file(void *file)
{
    int i = 0;
    struct sched_param param;
    {
        param.sched_priority = MY_PRIORITY + 3;
    }param;
    
    sched_setscheduler( 0, SCHED_FIFO, &param);
    
    int timer_fd = timerfd_create( CLOCK_MONOTONIC, 0);
    
    struct itimerspec itval;
    {
    itval.it_interval.tv_sec = //Peroid_second;
    itval.it_interval.tv_nsec = //Peroid_nsecond;
    
    itval.it_value.tv_sec = //inittime_second;
    itval.it_value.tv_nsec = //initime_nsecond;
    }itval;
    
        uint64_t num_periods = 0;
    read(timer_fd, &num_periods, sizeof(num_periods));
            if(num_periods > 1)
            {
                puts("MISSED WINDOW IN ODD READ");
            }
    
    char OddContent[100];
    printf("LineContent:");
    for( i  = 0; i < 20; i++)
    {
        scanf("%99[^\n]", &OddContent);
        getchar();
        printf("%s", OddContent);
        
    }
    
    
    
    
    
    
    return 0;
    
}

void *even_from_file(void *file)
{
    return 0;
}

void *write_to_file( void *file )
{
    

    return 0;
}
