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
#include <wiringPiSPI.h> 

#define MSG_SIZE 40
#define CHAR_DEV "/dev/project"
#define SPI_SPEED 2000
#define SPI_CHANNEL 0 // could be 0 or 1
//Defining led pins for easier reading
#define BLUE 5
#define YELLOW 3
#define GREEN 4
#define port 4000 //hard corded port number 
#define ADC_CHANNEL 2

//max and minimum for the ADC
#define MIN 450
#define MAX 790
uint16_t get_ADC(int channel);

pthread_mutex_t lock;//lock during critical section 

void *readthread(void *check_this_bitch);//thread to read from character device
void *commandleds(void *not_gonna_be_used);//command to turn the leds on or off
void set_thread_priority( int change_priority);//set thread priority 
void *time_update( void * not_used);//send a status update
void *ADC_read( void*  ADC_chan); 

// alot of code if not most of it is copied and used from lab5 sockets
int sock;
int cdev_id;
struct sockaddr_in addr, from;
int master = 0;
int information_error_check, number;
struct sockaddr_in update;
int filter_boardcast;


struct event
	{
		int yellow_status;
		int blue_status;
		int green_status;
		int first_switch;
		int second_switch;
		int button_one;
		int button_two;
		int no_power;
		int line_overload;
	}event;
	

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
	printf("\n%d", event.first_switch);

	//creating the pthread to read from the leds	
	pthread_t lighting;
	pthread_create(&lighting, NULL, &commandleds, NULL);

	//creating the thread for creating update string for data base
	pthread_t update;
	pthread_create(&update, NULL, &time_update, NULL); 
	
	//create thread to continously read the adc
	pthread_t ADC;
	pthread_create( &ADC, NULL, &ADC_read, NULL);

	while(1)
	{
		usleep(1000);
	}
	

           
	pthread_join( read, NULL);
	pthread_join( lighting, NULL);
	pthread_join( update, NULL);
	pthread_join( ADC, NULL);
	close(sock);
	
	return 0;
}

uint16_t get_ADC( int ADC_chan)
{
	uint8_t spiData[3];
		usleep(1700);
		spiData[0] = 0b00000001;
		spiData[1] = 0b10000000 | (ADC_chan << 4);
	
		spiData[2] = 0;
		wiringPiSPIDataRW( SPI_CHANNEL, spiData, 3);
			
	return (( spiData[1] << 8 ) | spiData[2]); 

}
void *ADC_read( void *ADC_chan)
{
	set_thread_priority(10);

	uint16_t ADCvalue;
	int sample_value;
	int value[5];
	int i = 0;

	while(1)
	{
		ADCvalue = get_ADC( ADC_CHANNEL);
		value[i] = ADCvalue;
	//	printf("ADC Value: %d\n", sample_value);
		fflush(stdout);
	
	if( MAX < ADCvalue || MIN > ADCvalue)
	{
		event.line_overload = 1;
	//	printf("\nevent line overload");	
	}
	//value[i] = sample_value;
	i++;	

	if( value[0] = value[1] = value[2] = value[3] = value[4] )
	{
		event.no_power = 1;
	//	printf("\n event no power"); 
	}
			
	if(i == 4)
	{
	 i = 0; 
	}

	}

	
	pthread_exit(0); 
} 
void * time_update( void* not_used)
{
	set_thread_priority(10);
	int i = 0;
	char update_buffer[50]; 

	//set up for the boardcast
	update.sin_family = AF_INET;
	update.sin_addr.s_addr = INADDR_ANY;
	update.sin_port = htons(4000);
	update.sin_addr.s_addr = inet_addr("128.206.19.117");
	

	struct timespec time;
	//create timing struct 
	struct itimerspec period;
		period.it_interval.tv_sec = 1;
		period.it_interval.tv_nsec = 0;
		period.it_value.tv_sec = 1;
		period.it_value.tv_nsec = 0;
	
	uint64_t num_periods = 0;
	
	int time_fd = timerfd_create( CLOCK_MONOTONIC, 0);
		
	clock_gettime(CLOCK_REALTIME, &time);
	
	if( timerfd_settime( time_fd, 0, &period, NULL) == -1)
	{	
		printf("timer failed to create\n");
		exit(-1);
	}
	
	while(1)
	{
	//	filter_boardcast = 1;
		bzero(update_buffer, 50);				
		read(time_fd, &num_periods, sizeof(num_periods));
		if( num_periods > 1)
		{
			printf("Window was missed for sending data\n");
			exit(-1);
		}
		sprintf(update_buffer, "(?, 1, %d, %d, %d, %d, %d, %d, %d, %d, %d , time());"
			, event.blue_status
			, event.yellow_status
			, event.green_status
			, event.first_switch
			, event.second_switch
			, event.button_one	
			, event.button_two	
			, event.no_power
			, event.line_overload
			);	
	
		printf("\nsending to desktop: %s", update_buffer);
		information_error_check = sendto( sock, update_buffer, sizeof(update_buffer), 0, (struct sockaddr *) &update, sizeof(update));
		if(information_error_check < 0)
		{
			printf("Error sending the information to server\n");
		}
		filter_boardcast = 0; 
		update_buffer == '\0';
			
	}	
	
	
	pthread_exit(0);

}

void *readthread( void *check_that_bitch)
{	
	set_thread_priority(10);
	//may need to add semphore to bc of the read function
	int dummy;	
	event.first_switch = 0;
	event.second_switch = 0;
	event.button_one = 0;
	event.button_two = 0;

	char bonus[MSG_SIZE];
	while(1)
	{
		//set the status of buttons to zero
	//	event.button_one = 0;
	//	event.button_two = 0;
	//	pthread_mutex_lock(&lock);
		bzero(bonus, MSG_SIZE);
		if( read(cdev_id, bonus, sizeof(bonus)) == -1)
		{
			printf("error with the read");
		}
		fflush(stdout);
	//	printf("bonus is:%s\n", bonus);
		if( bonus[0] == '!')
		{
//			pthread_mutex_lock(&lock);
	//		printf("\nread from kernel module: %s", bonus);
				switch(bonus[1]) 
				{
				case '4':
				//	printf("\nupdate status:%s", bonus);
					if(event.first_switch == 0)
					{		
					event.first_switch = 1;
					}
					else 
					{
					event.first_switch  = 0;
					}
					break; 
				case '3':
				//	printf("\nupdate status:%s", bonus);
					if(event.second_switch == 0)
					{		
					event.second_switch = 1;
					}
					else 
					{
					event.second_switch  = 0;
					}
					
					break;
				case '2':
				//	printf("\nupdate status:  %s", bonus);
					if( event.button_one == 0)
					{
					event.button_one = 1;
					}
					else 
					{
					event.button_one = 0;
					}
					break;
				case '1':
				//	printf("\nupdate status:  %s", bonus);
					if( event.button_two == 0)
					{
					event.button_two = 1;
					}
					else 
					{
					event.button_two = 0;
					}
							
					break; 

				}
		bonus[0] = '\0';
	//	pthread_mutex_unlock(&lock);
		}
	//	bonus[0] = '\0';

    	}	
   	pthread_exit(NULL);

}
void *commandleds(void *not_gonna_be_used)
{
	//gonna have to figure out how to implement data base then come back and add that implementation
	char led[MSG_SIZE];
	set_thread_priority(10);
	event.yellow_status = 0;
	event.green_status = 0;
	event.blue_status = 0;	
	while(1)
	{
		pthread_mutex_lock(&lock);
	//	printf("inside loop\n");
		if( filter_boardcast != 1)
		{
		if( read(sock, &led, sizeof(led)) < 0)
		{
	  	 	printf("error receiving led command in commandled function\n");
		}
		printf("\nreceived command is:%s", led); 
		if( led[0] == '@')
		{
			switch(led[1])
			{
			case 'B':
				printf("command to turn on blue light\n");
				digitalWrite( BLUE, HIGH);
				event.blue_status = 1;
				break;
			case 'G':
				printf("command to turn on green light\n");
				digitalWrite( GREEN, HIGH);
				event.green_status = 1;
				break;
			case 'Y':
				printf("command to turn on yellow light\n");
				digitalWrite( YELLOW, HIGH);
				event.yellow_status = 1;
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
				event.blue_status = 0;
				break;
			case 'G':
				printf("command to turn off green light\n");
				digitalWrite( GREEN, LOW);
				event.green_status = 0;
				break;
			case 'Y':
				printf("command to turn off yellow light\n");
				digitalWrite( YELLOW, LOW);
				event.yellow_status = 0;
				break;

			}
		}
		}
	//	else
	pthread_mutex_unlock(&lock);
	 }
	pthread_exit(NULL);
}
