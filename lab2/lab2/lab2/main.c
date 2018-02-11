
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

void import_info( char filename[], info *file);
void *running_thread1(void *file);
void *running_thread2(void *file);
void *running_thread3(void *file);
void *running_thread4(void *file);

void testing_case1(info *file);
void testing_case2(info *file);
void testing_case3(info *file);
void testing_case4(info *file);

int main(void)
{
    struct info file1, file2, file3;
    
    import_info("2x100.txt", &file1);
    import_info("15x15.txt", &file2);
    import_info("20x10.txt", &file3);
    
    printf("Enter the number you would like to search the files for: ");
    scanf("%d", &file1.find);
    file2.find = file3.find = file1.find;
    file2.result = file3.result = file1.result;
    
    //start the pthreading
    
    //thread one will do the whole matrix
    /*time = clock();

    pthread_create( &thread1, NULL, running_thread1, (void *)&file1);
    pthread_join( thread1, &result);
    file1.result = (int) result;
    time_end = clock();
    printf("Single thread: ");
    printf("Find: %d  columns in file %d \n", file1.find, file1.result);
     printf("Time to complete was: %fs\n",time_end -time);
    
   */
    printf("\n\n2x100.txt\n");
    testing_case1(&file1);
    testing_case2(&file1);
    testing_case3(&file1);
    testing_case4(&file1);
    
    printf("\n\n\n\n15x15.txt\n");
    testing_case1(&file2);
    testing_case2(&file2);
    testing_case3(&file2);
    testing_case4(&file2);
    
    printf("\n\n\n20x10.txt\n");
    testing_case1(&file3);
    testing_case2(&file3);
    testing_case3(&file3);
    testing_case4(&file3);
    return 0;
    
    
}
void testing_case4(info *file)
{
    float time = 0, time_end;
    void *result;
    pthread_t thread1[100][100];
    int i = 0, j  = 0, k = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)
    {
        for( k = 0; k < file->row; k++)
        {
            file->row_to_scan = k;
            for( j = 0; j < file->columns; j++)
            {
                file->columns_to_scan = j;
                pthread_create( &thread1[k][j], NULL, running_thread4, (void *)file);
            }
        }
        for( k = 0; k < file->row; k++)
        {
                for( j = 0; j < file->columns; j++)
                {
                    pthread_join( thread1[k][j], &result);
                    file->result += (int) result;
                }
        }
    }
    time_end = clock();
    printf("%d threads: \n", file->columns * file->row);
    printf("Find: %d  how many where found: %d \n", file->find, (file->result/10) );
    printf("Time to complete was: %fs\n",((time_end - time) /10));
}

void *running_thread4(void *file)
{
    struct info running = *(struct info*)file;
    int result = 0;
    int i , j;
    j = running.row_to_scan;
    i = running.columns_to_scan;
    
        if(running.data[j][i] == running.find)
        {
            ++result;
        }
    
    return (void *)result;
}
void testing_case3(info *file)
{
    float time = 0, time_end;
    void *result;
    pthread_t thread1[100][100];
    int i = 0, j  = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)
    {
        for( j = 0; j < file->columns; j++)
        {
            file->columns_to_scan = j;
            pthread_create( &thread1[0][j], NULL, running_thread3, (void *)file);
        }
        for( j = 0; j < file->columns; j++)
        {
            pthread_join( thread1[0][j], &result);
            file->result += (int) result;
        }
    }
    time_end = clock();
    printf("%d threads: \n", file->columns);
    printf("Find: %d  how many where found: %d \n", file->find, (file->result/10) );
    printf("Time to complete was: %fs\n",((time_end - time) /10));
}

void *running_thread3(void *file)
{
    int j;
    struct info running = *(struct info*)file;
    int result = 0;
    
    for( j = 0; j < running.row; j++)
    {
        if(running.data[j][running.columns_to_scan] == running.find)
        {
            ++result;
        }
    }
    return (void *)result;
}

void testing_case2(info *file)
{
    float time = 0, time_end;
    void *result = 0x0;
    pthread_t thread1[100][100];
    int i = 0, j  = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)
    {
        for( j = 0; j < file->row; j++)
        {
        file->row_to_scan = j;
        pthread_create( &thread1[j][0], NULL, running_thread2, (void *)file);
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
    printf("Time to complete was: %fs\n",((time_end - time) /10));
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


void testing_case1(info *file)
{
    float time = 0, time_end;
    void *result;
    pthread_t thread1[0][0];
    int i = 0;
    
    file->result = 0;
    time = clock();
    for(i = 0; i < 10 ; i++)
    {
    pthread_create( &thread1[0][0], NULL, running_thread1, (void *)file);
    pthread_join( thread1[0][0], &result);
    file->result = (int) result;
    }
    time_end = clock();
    printf("Single thread: ");
    printf("Find: %d  how mauny where found: %d \n", file->find, file->result );
    printf("Time to complete was: %fs\n",(time_end -time) /10);
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
    fscanf(fp,"%d", &file->row);
    fscanf(fp,"%d", &file->columns);
    //printf("rows:%d , columns: %d", file->row, file->columns);
    
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
    
    //printf(" %d \n", file->data[0][5]);
    fclose(fp);
};
