//
//  main.c
//  lab5
//
//  Created by Nicholas Bouckaert on 3/24/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

//compile using gcc historian.c -lpthread -lsqlite3

//#include <iostream>
//#include <fstream>
//#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <time.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sqlite3.h>
#include <pthread.h>
//#include <vector>
//#include <string> 
#include <unistd.h>
//#include <iomanip>

#define MSG_SIZE 100
#define port 4000
#define pi_address "128.206.19.255" //dont forget to change
#define historian_address "128.206.19.255" //dont forget to change 
#define database_location "/desktop/database.db"// location of the database on the local machine

//fuctions below main
void send_to_database_log( char message);
void send_to_database_error( char message);
void *send_command( void *not_used);
void *receive_update(void *not_used);
void database_connection();


//sending the variables are global for threads to beable to use them too
int sock, information_error_check, number ;
unsigned int length;
socklen_t fromlen;
struct sockaddr_in server, send_this, receive;
struct sockaddr_in addr, from;
char buffer[MSG_SIZE];

sqlite3 *dbfile;

char *findIP(void){
    int fd;                                                 //IP finding code sourced and modified from
    struct ifreq ifr;                                       //Filip Ekberg on Stack Overflow
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    
    ioctl(fd, SIOCGIFADDR, &ifr);
    
    close(fd);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

int main( int argc, char *argv[])
{
    // to store received messages or messages to be sent.
              
    int boolval = 1;            // for a socket option
    
    printf("usage: %s port\n", argv[0]);
         
    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
	{
        printf("socket error\n");
    	}
	
    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }
    length = sizeof(server); // determines lenght of the structure
    bzero(&server, length); // set all valus = 0
    server.sin_family = AF_INET; //constant for internet domain
    server.sin_addr.s_addr= INADDR_ANY; // MY IP address
    server.sin_port = htons(port);
    // going to set broadcasting address
    
    if (bind(sock, (struct sockaddr *)&server, length) < 0){
        printf("binding error dumby\n");
    }
    
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0){
        printf("error setting socket option\n");
        exit(-1);
    }   
	//reading and writing from the sockets 
	pthread_t read_sock, write_sock;
	
	pthread_create(&read_sock, NULL, &receive_update, NULL);
	pthread_create(&write_sock, NULL, &send_command, NULL); 

	//start accessing the database
//	database_connection();
	
	while(1)
	{
	usleep(2000);
	}
	pthread_join( read_sock, NULL);
	pthread_join( write_sock, NULL);
	
return 0;
}
void database_connection()
{
	int rc;

	rc = sqlite3_open( database_location, &dbfile);
	
	if( rc )
	{
		fprintf(stderr, "Could not open the database:%s\n", sqlite3_errmsg(dbfile));
		exit(-1);
	} 
	else
	{
	printf("opened the database successfully\n");
	}

	
}
void send_to_database_log( char message)
{	//database code used was from CMP3830
	sqlite3_stmt *statement;
	char *err_msg = 0;
	char insert_this;
	
	char cons[200] = "INSERT INTO Log VALUES";
	
	//insert_this = cons + to_string(messge);
	strcat( cons, &message);
	printf("%s\n", cons);

	char *str = cons;
	char *query = str;
	int res = 0;

	res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
	
	if (res != SQLITE_OK )
        {
            
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);
            exit(-1);
        }
	
	printf("\nEntered into the database successful");

}
void send_to_database_error( char message)
{
	sqlite3_stmt *statement;
	char *err_msg = 0;
	char  insert_this;
	
	char construct[200] = "INSERT INTO error VALUES";
	
	//insert_this = construct + to_string(messge);
	strcat( construct, &message );
	printf("%s\n", construct);
	
	char *str = construct;
	char *query = str;
	int res = 0;

	res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
	
	if (res != SQLITE_OK )
        {
            
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);
            exit(-1);
        }
	
	printf("\nEntered into the database successful");
	

}
void *send_command( void *not_used)
{
	char send[MSG_SIZE];
	char storage[MSG_SIZE];
	send_this.sin_family = AF_INET;
	send_this.sin_addr.s_addr = INADDR_ANY;
	send_this.sin_port = htons(port);
	
	while(1)
	{
	scanf("%s", storage);
	printf("\n%s", storage);
	if( send != '\0')
		{
		bzero(send, MSG_SIZE);
		strcpy(send, storage);
//		printf("this was stored:%s\n", storage);
		send_this.sin_addr.s_addr = inet_addr(pi_address);//change from boardcast address to pi address
		information_error_check = sendto( sock, send, sizeof(send), 0, (struct sockaddr *) &send_this, sizeof(addr));
               if( information_error_check < 0)
              	 {
                	   printf("Error sending the information to server\n");
         	  }
		
//		printf("message was sent: %s\n", send);//comment out once testing is done
		//comment this once testing is done and we are ready to test the database side
	//	send_to_database_log( send );
		}
	}
	pthread_exit(0);
}
void *receive_update(void *not_used)
{
	char buffer [MSG_SIZE];
	receive.sin_family = AF_INET;
	receive.sin_addr.s_addr = INADDR_ANY;
	receive.sin_port = htons(port);

    while(1)
  {
        bzero(buffer, MSG_SIZE);
//	shot.sin_addr.s_addr = inet_addr(historian_address);
        information_error_check = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&receive, &fromlen);
        if( information_error_check < 0)
            {
                printf("that was an error receiving infromation from the sock connection\n");
                exit( -1);
            }
//	printf("\nReceived was infomration was: %s\n", buffer);//comment out once testing is done 

   }
    pthread_exit(0);
}
