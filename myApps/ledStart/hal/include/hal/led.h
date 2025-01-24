// Sample button module 
// Part of the Hardware Abstraction Layer (HAL) 

#ifndef _LED_H
#define _LED_H

#define LED_TRIGGER_FILE_NAME "/sys/class/leds/ACT/trigger"
#define LED_BRIGHTNESS_FILE_NAME "/sys/class/leds/ACT/brightness"
void turnOnTrigger(char* input);
void setBrightness(char* flag);

#endif
