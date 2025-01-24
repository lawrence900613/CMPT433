// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include "hal/led.h"
int main()
{
    // printf("try to set brightess!\n");
    // setBrightness("1");
    printf("try to set heartbeat!\n");
    // setBrightness("1");
    turnOnTrigger("heartbeat");
}