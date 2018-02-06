//Nicholas Bouckaert 
// lab2 week 1
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct info
{
	int row;
	int columns;
	int *array;
}info;

void import_info( char filename[], struct info);


int main(void)
{
	//	pthread_t thread1, thread2, thread3;
	struct info file1;
	char input[] = "2x100.txt";
	 import_info(input, file1);
	








	return 0;


}

void import_info( char filename[], struct info info)//function will open and read in every file and store in the stuct
{
	int j, i;
	FILE *fp = fopen( filename, "r");
	fscanf(fp,"%d",&info.row);
	fscanf(fp,"%d", &info.columns);
	printf("rows:%d , columns: %d", info.row, info.columns);
	
	info * array =(info*)malloc(sizeof(int)*info.columns*info.row);
	
	for( i = 0; i <= info.row; i++)
	{
		for( j = 0; j <= info.columns; j++)
		{
			fscanf(fp, "%d", &info.array[i][j]);
			printf("  %d  ", info.array[i][j]);
		}
		printf("\n");
	}
	

	
	fclose(fp);
};

