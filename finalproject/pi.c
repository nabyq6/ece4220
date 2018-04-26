//Nicholas Bouckaert
//Lab 5

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
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <wiringPi.h>
#include <sched.h>
#include <sys/timerfd.h>

#define MSG_SIZE 40
#define CHAR_DEV "/dev/project"
#define SPI_SPEED 2000000
#define SPI_CHANNEL 0 // could be 0 or 1
//Defining led pins for easier reading
#define BLUE 5
#define YELLOW 3
#define GREEN 4
#define port 5000 //hard corded port number 

pthread_mutex_t lock;

void *readthread(void *check_this_bitch);//thread to read from character device
void *commandleds(void *not_gonna_be_used);//command to turn the leds on or off
void set_thread_priority( int change_priority);


// alot of code if not most of it is copied and used from lab5 sockets
int sock;
int cdev_id;
struct sockaddr_in addr, from;
int master = 0;
int information_error_check, number ;

void set_thread_priority( int change_priority)
{
	struct sched_param param;
		
	param.sched_priority = 50 + change_priority;
	
	if( sched_setscheduler(0, SCHED_FIFO, &param) != 0)
	{
	printf("real time is not accessed\n");
	exit(-1);
	}

}

char *findIP(void){//taken exactly from lab 5
    int fd;                                                 //IP finding code sourced and modified from
    struct ifreq ifr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    
    ioctl(fd, SIOCGIFADDR, &ifr);
    
    close(fd);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

int main ( int argc, char *argv[])
{
//int information_error_check, number ;
int master = 0;
unsigned int length;
socklen_t fromlen;
struct sockaddr_in server;
//struct sockaddr_in addr, from;
char buffer[MSG_SIZE];    // to store received messages or messages to be sent.
char my_message[MSG_SIZE];
char vote_string[MSG_SIZE];
int boolval = 1;            // for a socket option
int note;
char notes[MSG_SIZE];
int flag = 0;
//char bonus[MSG_SIZE];
int dummy; 
//int cdev_id;
    
    	if (argc != 1)
        	{
           		 printf("usage: %s port\n", argv[0]);
           		 exit(-1);
       		 }
    
	sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    	if (sock < 0)
        	{
            		error("socket");
        	}
    
    //printf("socked made\n");
    length = sizeof(server); // determines lenght of the structure
    bzero(&server, length); // set all valus = 0
    server.sin_family = AF_INET; //constant for internet domain
    server.sin_addr.s_addr= INADDR_ANY; // MY IP address
    server.sin_port = htons(port);
    // going to set broadcasting address
    
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
        {
            printf("binding error dumby\n");
        }
 
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
        {
            printf("error setting socket option\n");
            exit(-1);
        }

    if((cdev_id = open( CHAR_DEV, O_RDWR)) == -1)
    	{
        	printf("cant connect to device properly %s\n", CHAR_DEV);
		exit(-1);
    	}

    fflush(stdout);
    printf("after the open\n"); 
    fromlen = sizeof(struct sockaddr_in);
    strcpy( my_message, "connection");
  
//seting up the lighting on the the board
	wiringPiSetupGpio();
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED)< 0)
		{
			printf("failed to in wiringpi when setting up the leds\n");
			exit(-1);
		}
	pinMode(BLUE, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(YELLOW, OUTPUT);

	digitalWrite(BLUE, LOW);
	digitalWrite(GREEN, LOW);
	digitalWrite(YELLOW, LOW);

        printf("creating threads\n\n");	
	
	//creating the thread to read from the kernel module 
	pthread_t read;
	pthread_create(&read, NULL, &readthread, NULL);

	//creating the pthread to read from the leds	
	pthread_t lighting;
	pthread_create(&lighting, NULL, &commandleds, NULL);

	while(1)
	{
	usleep(100);
	}
	
/*    while(1)
    {
	//printf("seg fault\n");
        bzero(buffer, MSG_SIZE);
        information_error_check = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
        if( information_error_check < 0)
        {
            error("that was an error receiving infromation from the sock connection\n");
            exit( -1);
        }        

         if(master == 1 && buffer [0] == '@' && flag == 0 )
          {
               addr.sin_addr.s_addr = inet_addr("128.206.19.255");
               //printf("boardcast id set\n");// error check
	//	buffer[0] = '\0';
		strcpy( notes, buffer);
		//strcat(notes, buffer[1]);
		usleep(100);
               information_error_check = sendto( sock, notes , sizeof(notes), 0, (struct sockaddr *) &addr, sizeof(addr));
               		if( information_error_check < 0)
               		{
                   		printf("Error sending the information to server\n");
               		}
		
		flag = 1;
               		
			if(write(cdev_id , buffer, sizeof(buffer)) != sizeof(buffer))
               		{
                	   	printf("error during note changing\n");
                   		exit(-1);
               		}
	
        	}
		else 
		{
			flag = 0;
		}
        
    }
*/
           
	pthread_join( read, NULL);
	pthread_join( lighting, NULL);

	return 0;
}

void *readthread( void *check_that_bitch)
{	
	set_thread_priority(10);
	//may need to add semphore to bc of the read function
	int dummy;	
	
	char bonus[MSG_SIZE];
	while(1)
	{

		bzero(bonus, MSG_SIZE);
		if( read(cdev_id, bonus, sizeof(bonus)) == -1)
		{
			//printf("error with the read");
		}
		//	printf("%s\n", bonus);
		if( bonus[0] == '!')
		{
			pthread_mutex_lock(&lock);
			printf("\nread from kernel module: %s", bonus);
		//	information_error_check = sendto( sock, bonus, sizeof(bonus), 0, (struct sockaddr *) &addr, sizeof(addr));
               		if( information_error_check < 0)
              	 	{
                		   printf("Error sending the information to server\n");
             		}

		pthread_mutex_unlock(&lock);
		}
			usleep(800);
		bonus[0] = '\0';

    	}	
   	pthread_exit(NULL);
}
void *commandleds(void *not_gonna_be_used)
{
	//gonna have to figure out how to implement data base then come back and add that implementation
	char led[MSG_SIZE];
	int blue_status = 0;
	int green_status = 0;
	int yellow_status = 0;
	
	while(1)
	{
		if( read(sock, &led, sizeof(led)) < 0)
		{
	  	 	printf("error receiving led command in commandled function\n");
		}
		else
		{
			printf("received command is:%s", led); 
		}
		if( led[0] == '@')
		{
			switch(led[1])
			{
			case 'B':
				printf("command to turn on blue light\n");
				digitalWrite( BLUE, HIGH);
				blue_status = 1;
				break;
			case 'G':
				printf("command to turn on green light\n");
				digitalWrite( GREEN, HIGH);
				green_status = 1;
				break;
			case 'Y':
				printf("command to turn on yellow light\n");
				digitalWrite( YELLOW, HIGH);
				yellow_status = 1;
				break;

			}
		}
		if( led[0]  == '#')
		{
			switch(led[1])
			{
			case 'B':
				printf("command to turn off blue light\n");
				digitalWrite( BLUE, LOW);
				blue_status = 0;
				break;
			case 'G':
				printf("command to turn off green light\n");
				digitalWrite( GREEN, LOW);
				green_status = 0;
				break;
			case 'Y':
				printf("command to turn off yellow light\n");
				digitalWrite( YELLOW, LOW);
				yellow_status = 0;
				break;

			}
		}

	 }
	pthread_exit(NULL);
}
