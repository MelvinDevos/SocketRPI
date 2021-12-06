#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include "ud_ucase.h"

// A function that terminates when enter key is pressed
void fun()
{
    printf("fun() starts \n");
    printf("Press enter to stop fun \n");
    while (1)
    {
        if (getchar())
            break;
    }
    printf("fun() ends \n");
}

// The main program calls fun() and measures time taken by fun()
int main()
{
    wiringPiSetupGpio();
    pinMode(27, OUTPUT) ;
    // pinMode(24, INPUT) ;
    // pinMode(25, INPUT) ;
    // Calculate the time taken by fun()
    int t;
    t = millis();
    fun();
    t = millis() - t;

    digitalWrite(27, 0);
    printf("DR: %d\n", digitalRead(27));
    printf("DR: %d\n", digitalRead(24));
    printf("DR: %d\n", digitalRead(25));



    printf("fun() took %d millis to execute \n", t);
    return 0;
}