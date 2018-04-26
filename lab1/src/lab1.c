/*
 ============================================================================
 Name        : lab1.c
 Author      : Nicholas Bouckaert
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

#define LED1 8 //red led
#define LED2 9 // yellow led
#define speaker 22
#define button1 27
#define button2 0
#define button3 1
#define button4 24
#define button5 28
int main(void) {

    wiringPiSetup();
    int time = 1;
    int button = 1;
	//seting up the pins for input and output
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(speaker, OUTPUT);
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);
    pinMode(button5, INPUT);
	// putting position of the pin
    pullUpDnControl(button1,PUD_DOWN);
    pullUpDnControl(button2,PUD_DOWN);
    pullUpDnControl(button3,PUD_DOWN);
    pullUpDnControl(button4,PUD_DOWN);
    pullUpDnControl(button5,PUD_DOWN);

    while( time < 5 )
    {   
	
        digitalWrite( LED1, HIGH);// red on
        digitalWrite( LED2, LOW); // yellow off
        sleep(2);
        digitalWrite( LED1, LOW); //red off
	      digitalWrite( LED2, HIGH);//yellow on
        sleep(2);	

    }


	//what button the user wants to be able to push to activate the speaker
    do
    {
    printf("\n\n\n Enter a button number 1-5 to use to play speaker");
    scanf("%d", &button);
        if(button < 1 && button > 5)
        {
            printf("Invalid choice please try again\n");
        }
    }while(button < 1 && button > 5);
	// while loop in every case statement that will endlessly loop until the user cltr c or quites
    switch(button){

    case 1:
        printf("button one ");
        while ( time == 1 )
            {
                if( digitalRead(button1))
                {
                    digitalWrite(speaker, HIGH);
                    usleep(400);
                    digitalWrite(speaker, LOW);
                    usleep(400);
                }
            }
        break;
    case 2:
        printf("button two ");
         while(time == 1)
	{
	       if( digitalRead(button2))
                    {
                        digitalWrite(speaker, HIGH);
                        usleep(400);
                        digitalWrite(speaker, LOW);
                        usleep(400);
                    }
        }
	        break;

    case 3:
        printf("button three ");
         while(time == 1)
	{
		if( digitalRead(button3))
            {
                digitalWrite(speaker, HIGH);
                usleep(400);
                digitalWrite(speaker, LOW);
                usleep(400);
            }
	}
            break;
    case 4:
        printf("button four ");
         while(time == 1)
	{
	       if( digitalRead(button4))
                    {
                        digitalWrite(speaker, HIGH);
                        usleep(400);
                        digitalWrite(speaker, LOW);
                        usleep(400);

                    }
        }
	       break;
    case 5:
        printf("button 5");
        while( time == 1)
	{
	     if( digitalRead(button5))
                    {
                        digitalWrite(speaker, HIGH);
                        usleep(400);
                        digitalWrite(speaker, LOW);
                        usleep(400);

                    }
	}
                break;
    }
    return 0;
}
