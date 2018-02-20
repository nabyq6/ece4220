
//
//  main.c
//  lab2
//
//  Created by Nicholas Bouckaert on 2/10/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

//Nicholas Bouckaert
// lab2 week1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct info
{
    int row;
    int columns;
    int row_to_scan;
    int columns_to_scan;
    int data[100][100];
    int find;
    int result;
}info;

typedef struct thread_specific
{
    info * info;
    int row_to_scan;
    int column_to_scan;
} thread_specific;

void import_info( char filename[], info *file);
void *running_thread1(void *file);//testing using one thread
void *running_thread2(void *file);//testing using number of rows = #threads
void *running_thread3(void *file);//testing using number oc columns = # threads
void *running_thread4(void *file);//Each individually element as a thread

void testing_case1(info *file);
void testing_case2(info *file);
void testing_case3(info *file);
void testing_case4(info *file);


int main(void)
{
    struct info file1, file2, file3;
    
    //void *result;
    // info *fileI = &file1;
    //info *fileII= &file2;
    // info *fileIII = &file3;
    
    //float time, time_end, total_time;
    
    import_info("2x100.txt", &file1);
    import_info("15x15.txt", &file2);
    import_info("20x10.txt", &file3);
    
    printf("Enter the number you would like to search the files for: ");
    scanf("%d", &file1.find);
    file2.find = file3.find = file1.find;
    file2.result = file3.result = file1.result = 0;
    
    //start the pthreading
    
    
    printf("\n\n2x100.txt\n");//testing this file in each type of search
    testing_case1(&file1);
    testing_case2(&file1);
    testing_case3(&file1);
    testing_case4(&file1);
    
    printf("\n\n\n\n15x15.txt\n");//testing this file in each type of search
    testing_case1(&file2);
    testing_case2(&file2);
    testing_case3(&file2);
    testing_case4(&file2);
   
    printf("\n\n\n20x10.txt\n");//testing this file in each type of search
    testing_case1(&file3);
    testing_case2(&file3);
    testing_case3(&file3);
    testing_case4(&file3);
     
    return 0;
    
    
    
}
void testing_case4(info *file)//each element as a thread
{
    float time = 0, time_end;
    pthread_t ** thread1;
    thread_specific **perThreadJob;
    //pthread_t thread1[100][100]; // gee, I hope that's enough!
    //thread_specific perThreadJob[100][100];
    int i = 0, j  = 0, k = 0;
    
    // Init thread1 and perThreadJob
    thread1 = (pthread_t**) alloca(sizeof(pthread_t*) * file->row);
    perThreadJob = (thread_specific**) alloca(sizeof(thread_specific*) * file->row);
    for (i=0; i<file->row; i++) {
        thread1[i] = (pthread_t*) alloca(sizeof(pthread_t) * file->columns);
        perThreadJob[i] = (thread_specific*) alloca(sizeof(thread_specific) * file->columns);
    }
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)//running each search ten times to get the average
    {
        for( k = 0; k < file->row; k++)
        {
            for( j = 0; j < file->columns; j++)
            {
                perThreadJob[k][j].info = file; // only copying the pointer
                perThreadJob[k][j].row_to_scan = k;
                perThreadJob[k][j].column_to_scan = j;
                pthread_create( &thread1[k][j], NULL, running_thread4, (void *)&perThreadJob[k][j]);
            }
        }
        for( k = 0; k < file->row; k++)
        {
            for( j = 0; j < file->columns; j++)
            {
                void *result;
                pthread_join( thread1[k][j], &result);
                file->result += (int)(intptr_t) result;
            }
        }
    }
    time_end = clock();
    printf("%d threads: \n", file->columns * file->row);
    printf("Find: %d  how many where found: %d \n", file->find, (file->result/10) );
    printf("Time to complete was: %fs\n",((double)(time_end - time) /10/ CLOCKS_PER_SEC));
}

void *running_thread4(void *file)
{
    thread_specific *job = (thread_specific*)file; // not making a full copy, just converting the reference
    info *running = job->info;
    int j = job->row_to_scan;
    int i = job->column_to_scan;
    
    //printf("i:%d j:%d\n", i, j);
    if(running->data[j][i] == running->find)
    {
        return (void *)(intptr_t)1;
    }
    
    return (void *)(intptr_t)0;
}

void testing_case3(info *file)// each column as a thread
{
    float time = 0, time_end;
    void *result;
    pthread_t *thread1;
    thread_specific *perThreadJob1;
   // pthread_t thread1[100][100];
    int i = 0, j  = 0;
    
    thread1 = (pthread_t*) malloc(sizeof(pthread_t*) * file->columns);
    perThreadJob1 = (thread_specific*) malloc(sizeof(thread_specific*) * file->columns);
//    thread1[i] = *(pthread_t*) alloca(sizeof(pthread_t));
  //  perThreadJob1[i] = *(thread_specific*)alloca(sizeof(thread_specific));
    
    file->result = 0;
    time = clock();
    //for(i = 0; i < 10 ; i++)//running each search ten times to get the average
    {
        //printf("%d\n", file->row);
        for( j = 0; j < file->columns; j++)
        {
            
            perThreadJob1[j].info = file; // only copying the pointer
            perThreadJob1[j].column_to_scan = j ;
            //printf("*%d\n", j);
            pthread_create( &thread1[j], NULL, running_thread3, (void *)&perThreadJob1[j]);
            
            //pthread_create( &thread1[j], NULL, running_thread3, (void *)file);
           //usleep(100);//sleeping to allow time for excution of the thread
        }
        for( j = 0; j < file->columns; j++)
        {
            pthread_join( thread1[j], &result);
            file->result += (int)(intptr_t)(result);
        }
    }
    time_end = clock();
    printf("%d threads: \n", file->columns);
    printf("Find: %d  how many where found: %d \n", file->find, (file->result/10) );
    printf("Time to complete was: %fs\n",((double) (time_end - time) /10 / CLOCKS_PER_SEC));
}

void *running_thread3(void *file)
{
    int i = 0;
    int j = 0;
    thread_specific *job = (thread_specific*)file; // not making a full copy, just converting the reference
    info *running = job->info;
    
    j = job->column_to_scan;
    void * result = 0;
    //printf("--%d\n", job->columns_to_scan);
    //printf("--%d\n", j);
    for( i = 0; i < running->row; i++)
    {
        //printf("ti:%d j:%d\n", i, j);
    if(running->data[i][j] == running->find)
        {
            ++result;// get around this bitch with a simple mutex lock:
        }
    }
   // printf("%d" ,result);
    pthread_exit ((void*)(intptr_t)result);
}


void testing_case2(info *file)//each row as a thead
{
    float time = 0, time_end;
    void *result = 0x0;
    pthread_t thread1[100][100];
    int i = 0, j  = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)//running each search ten times to get the average
    {
        for( j = 0; j < file->row; j++)
        {
            file->row_to_scan = j;
            pthread_create( &thread1[j][0], NULL, running_thread2, (void *)file);
            usleep(100);//sleeping to allow time for excution of the thread
        }
        for( j = 0; j < file->row; j++)
        {
            pthread_join( thread1[j][0], &result);
            file->result += (int) result;
        }
    }
    time_end = clock();
    printf("%d threads: \n", file-> row);
    printf("Find: %d  how many where found: %d \n", file->find, (file->result/10));
    printf("Time to complete was: %fs\n",((double)(time_end - time) /10/ CLOCKS_PER_SEC));
}

void *running_thread2(void *file)
{
    int j;
    struct info running = *(struct info*)file;
    int result = 0;
    
    for( j = 0; j < running.columns; j++)
    {
        if(running.data[running.row_to_scan][j] == running.find)
        {
            ++result;
        }
    }
    return (void *)result;
}


void testing_case1(info *file)//one thread to scan the whole file
{
    float time = 0, time_end;
    void *result;
    pthread_t thread1[0][0];
    int i = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)//running each search ten times to get the average
    {
        pthread_create( &thread1[0][0], NULL, running_thread1, (void *)file);//creation of thread
        pthread_join( thread1[0][0], &result);
        file->result = (int) result;
    }
    time_end = clock();
    printf("Single thread: ");
    printf("Find: %d  how mauny where found: %d \n", file->find, file->result );
    printf("Time to complete was: %fs\n",(double)(time_end -time) /10/ CLOCKS_PER_SEC);
}

void *running_thread1(void *file)
{
    int i, j;
    struct info running = *(struct info*)file;
    int result = 0;
    
    for( i = 0; i < running.row; i++ )
    {
        for( j = 0; j < running.columns; j++)
        {
            if(running.data[i][j] == running.find)
            {
                ++result;
            }
        }
        
    }
    return (void *)result;
}



void import_info( char filename[],  info *file)//function will open and read in every file and store in the stuct
{
    int j, i;
    FILE *fp = fopen( filename, "r");
    if( fp == NULL)
    {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }
    fscanf(fp,"%d", &file->row);
    fscanf(fp,"%d", &file->columns);
   // printf("rows:%d , columns: %d", file->row, file->columns);
    
   // scanf("%d" , i);
    
    //int array[file->row][file->columns];
    //int array= *(int*)malloc(sizeof(int)*file.columns*file.row);
    
    for( i = 0; i < file->row; i++)
    {
        for( j = 0; j < file->columns; j++)
        {
            fscanf(fp, "%d", &file->data[i][j]);
            //printf("  %d  ", file->data[i][j]);
        }
        
    }
    
    //printf(" %d \n", file->data[0][5]); test 5 item to insure proper storage
    fclose(fp);//file is closed
};

