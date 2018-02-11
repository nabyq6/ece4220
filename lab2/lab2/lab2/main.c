
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
    int data[100][100];
    int find;
    int result;
    
}info;

void
int main(void)
{
    pthread_t thread1; //thread2, thread3;
    struct info file1, file2, file3;
    void *result;
   // info *fileI = &file1;
    //info *fileII= &file2;
   // info *fileIII = &file3;

    float time, time_end, total_time;

    
    import_info("2x100.txt", &file1);
    import_info("15x15.txt", &file2);
    import_info("20x10.txt", &file3);
    
    printf("Enter the number you would like to search the files for: ");
    scanf("%d", &file1.find);
    file2.find = file3.find = file1.find;
    file2.result = file3.result = file1.result;
    
    //start the pthreading
    
    //thread one will do the whole matrix
    time = clock();
    pthread_create( &thread1, NULL, running_thread1, (void *)&file2);
    pthread_join( thread1, &result);
    file1.result = (int) result;
    time_end = clock();
    printf("Single thread: ");
    printf("Find: %d  columns in file %d \n", file2.find, file2.result);
    printf("Time to complete was: %fs\n",time_end -time);
    
    
    
    
    
    return 0;
    
    
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
int main(void)
{
    pthread_t thread1; //thread2, thread3;
    struct info file1, file2, file3;
    void *result;
   // info *fileI = &file1;
    //info *fileII= &file2;
   // info *fileIII = &file3;

    float time, time_end, total_time;

    
    import_info("2x100.txt", &file1);
    import_info("15x15.txt", &file2);
    import_info("20x10.txt", &file3);
    
    printf("Enter the number you would like to search the files for: ");
    scanf("%d", &file1.find);
    file2.find = file3.find = file1.find;
    file2.result = file3.result = file1.result;
    
    //start the pthreading
    
    //thread one will do the whole matrix
    time = clock();
    pthread_create( &thread1, NULL, running_thread1, (void *)&file1);
    pthread_join( thread1, &re   
    
    
    return 0;
    
    
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

