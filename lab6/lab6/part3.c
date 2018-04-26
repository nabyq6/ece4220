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
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MSG_SIZE 40
#define CHAR_DEV "/dev/lab6"
void *readthread(void *check_this_bitch);

// alot of code if not most of it is copied and used from lab5 sockets
int sock;
int cdev_id;
struct sockaddr_in addr, from, shot;
int master = 0;
int information_error_check, number ;
struct sockaddr_in server;
char port; 
int flag;
int catch;

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
flag = 0;
catch = 0;
//int information_error_check, number ;
int master = 0;
unsigned int length;
socklen_t fromlen;
struct sockaddr_in server;
//struct sockaddr_in addr, from;
char buffer[MSG_SIZE];    // to store received messages or messages to be sent.
char my_message[MSG_SIZE];
char vote_string[MSG_SIZE];
char random_vote[2];
int vote_number;
int boolval = 1;            // for a socket option
int note;
char notes[MSG_SIZE];
int flag = 0;
//char bonus[MSG_SIZE];
int dummy; 
//int cdev_id;
    
    printf("start of progarm\n");
    if (argc != 2)
        {
            printf("usage: %s port\n", argv[0]);
            exit(-1);
        }
   printf("Argv\n"); 
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
    server.sin_port = htons(atoi(argv[1]));
	port = atoi(argv[1]);
    // going to set broadcasting address
    
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
        {
            printf("binding error dumby\n");
        }
    printf("socked bind\n");
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
        {
            printf("error setting socket option\n");
            exit(-1);
        }
    printf("setsockopt\n");
    if((cdev_id = open( CHAR_DEV, O_RDWR)) == -1)
    {
        printf("cant connect to device properly %s\n", CHAR_DEV);
	exit(-1);
    }
    fflush(stdout);
    printf("after the open\n"); 
    fromlen = sizeof(struct sockaddr_in);
    strcpy( my_message, "Nicholas");
    strcat( my_message, findIP());
    strcat( my_message, "is your master");
    printf("creating thread for bonus\n\n");
	pthread_t read;
	pthread_create(&read, NULL, &readthread, (void *)&master);
 //   printf("in the while loop\n");
    while(1)
    {
	//printf("seg fault\n");
        bzero(buffer, MSG_SIZE);
        information_error_check = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
        if( information_error_check < 0)
        {
            error("that was an error receiving infromation from the sock connection\n");
            exit( -1);
        }
        printf("Received where am i was:\n %s", buffer);
        
        
        if(strcmp(buffer, "WHOIS\n") == 0)
        {
            printf("Received WHOIS\n");
            
            if(master == 1)
            {
                printf("Master SENDTO\n");
                information_error_check = sendto(sock, my_message, MSG_SIZE, 0, ( struct sockaddr*)&addr, fromlen);
                if( information_error_check < 0 )
                {
                    printf("error sending the imformation to server\n");
                    exit(-1);
                }
                
            }
	}

//	printf("seg fault check number 2");
      if(strcmp(buffer, "VOTE\n") == 0)
	{
	printf("sent in vote\n");
	master = 1;
	number = rand() % 10 +1;
	snprintf(random_vote, 10, "%d", number);
	vote_string[0] = '\0';
	strcpy(vote_string, "# ");
	strcat(vote_string, findIP());
	strcat(vote_string, " ");
	strcat(vote_string, random_vote);
	
        addr.sin_addr.s_addr = inet_addr("128.206.19.255");
        printf("in vote: boardcast id set\n");// error check
	
        information_error_check = sendto( sock, vote_string, sizeof(vote_string), 0, (struct sockaddr *) &addr, sizeof(addr));
        if( information_error_check < 0)
        {
            printf("Error sending the information to server\n");
        }
        printf("Vote Sent: %s\n", vote_string);

	}

    if(buffer[0] == '#')
    {
        switch(buffer[16])
        {
            case  '1':
                if(buffer[17] == '0'){
                    vote_number = 10;
                }else{
                    vote_number = 1;
                }
                break;
            case '2':
                vote_number = 2;
                break;
            case '3':
                vote_number = 3;
                break;
            case '4':
                vote_number = 4;
                break;
            case '5':
                vote_number = 5;
                break;
            case '6':
                vote_number = 6;
                break;
            case '7':
                vote_number= 7;
                break;
            case '8':
                vote_number = 8;
                break;
            case '9':
                vote_number = 9;
                break;
        }
        if( number < vote_number)
        {
            master = 0;
        }
        else if(number == vote_number)
        {
            if(strncmp(vote_string,buffer,14) < 0)
            {
                master = 0;
            }
            else
            {
                master = 1;
            }
        }
    }


        
        if( buffer[0] == '@')
           {
               switch(buffer[1])
               {
                   case 'A':
                       note = 1;
                       break;
                
                   case 'B':
                       note = 2;
                       break;
                       
                   case 'C':
                       note = 3;
                       break;
                       
                   case 'D':
                       note = 4;
                       break;
                       
                   case 'E':
                       note = 5;
                   break;
                }
               if(master == 1 && buffer [0] == '@' && flag == 0 && catch == 0 )
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
}
           return 0;
	pthred_join( read, NULL);
}

void *readthread( void *check_that_bitch)
{
	int dummy;	
	
	char bonus[MSG_SIZE];
	shot.sin_family = AF_INET;
	shot.sin_addr.s_addr = INADDR_ANY;
	shot.sin_port = htons(4000);
	while(1)
	{
	bzero(bonus, MSG_SIZE);
	//dummy = read(cdev_id, bonus, sizeof(bonus));	
	if( read(cdev_id, bonus, sizeof(bonus)) == -1)
	{
	printf("error with the read");
	}
//	printf("%s\n", bonus);
	if( bonus[0] == '@')
	{
//	printf("master is: %d  bonus is: %s\n", master, bonus);

	if( *(int*)check_that_bitch == 1)
	{
	printf("master is: %d  bonus is: %s\n\n", master, bonus);
	shot.sin_addr.s_addr = inet_addr("128.206.19.255");
	information_error_check = sendto( sock, bonus, sizeof(bonus), 0, (struct sockaddr *) &shot, sizeof(shot));
               if( information_error_check < 0)
              	 {
                	   printf("Error sending the information to server\n");
             	  }
	catch = 1;

	}
    }
	usleep(800);
	bonus[0] = '\0';
	}
}
