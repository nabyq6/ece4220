//Nicholas Bouckaert
//Labg 5

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
#include <sys/type.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MSG_SIZE 40
#define CHAR_DEV "/dev/lab6"

// alot of code if not most of it is copied and used from lab5 sockets


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
int sock, information_error_check, number ;
int master = 0;
unsigned int length;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in addr, from;
char buffer[MSG_SIZE];    // to store received messages or messages to be sent.
char my_message[MSG_SIZE];
char vote_string[MSG_SIZE];
char random_vote[2];
int vote_number;
int boolval = 1;            // for a socket option
int note;
    
    
    if (argc != 2)
        {
            printf("usage: %s port\n", argv[0]);
            exit(1);
        }
    
    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        {
            error("socket");
        }
    
    
    length = sizeof(server); // determines lenght of the structure
    bzero(&server, length); // set all valus = 0
    server.sin_family = AF_INET; //constant for internet domain
    server.sin_addr.s_addr= INADDR_ANY; // MY IP address
    server.sin_port = htons(atoi(argv[1]));
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
    
    if((cdev_id = open( CHAR_DEV, O_WRONLY)) == -1)
        {
            printf("cant connect to device properly");
            exit(-1);
        }
    
    fromlen = sizeof(struct sockaddr_in);
    strcpy( my_message, "Nicholas");
    strcat( my_message, findIP());
    strcat( my_message, "is your master");
    
    while(1)
    {
        bzero(buffer, MSG_SIZE);
        information_error_check = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
        if( information_error_check < 0)
        {
            error("that was an error receiving infromation from the sock connection\n");
            exit( -1);
        }
        printf("Received buffer was:\n %s", buffer);
        
        
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
        
        ddr.sin_addr.s_addr = inet_addr("128.206.19.255");
        printf("boardcast id set\n");// error check
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
        
        if( buffer[0] == '@)
           {
               switch(bufffer[1])
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
                       note = 4;
                   break;
                }
               if(master == 1)
               {
               ddr.sin_addr.s_addr = inet_addr("128.206.19.255");
               printf("boardcast id set\n");// error check
               information_error_check = sendto( sock, vote_string, sizeof(vote_string), 0, (struct sockaddr *) &addr, sizeof(addr));
               if( information_error_check < 0)
               {
                   printf("Error sending the information to server\n");
               }
               if(write( cdev_id, buffer, sizeof(buffer)) != sizeof(buffer))
               {
                   printf("error during note changing\n");
                   exit(-1);
               }
           }
        
    }
}
           return 0;
}
