#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hal/led.h"
#include <string.h>

void turnOnTrigger(char* input){
    FILE *pLedTriggerFile = fopen(LED_TRIGGER_FILE_NAME, "w");
    if (pLedTriggerFile == NULL) {
        perror("Error opening LED trigger file");
        exit(EXIT_FAILURE);
    }
    if (!strcmp(input, "none") && !strcmp(input, "heartbeat")) {
        perror("Wrong command to set Trigger");
        exit(EXIT_FAILURE);
    }
    int charWritten = fprintf(pLedTriggerFile, input);
    if (charWritten <= 0) {
        perror("Error writing data to LED file");
        exit(EXIT_FAILURE);
    }
    fclose(pLedTriggerFile);
    // Sleep 100 milliseconds
    long seconds = 0;
    long nanoseconds = 100000000;  // 100 milliseconds = 100,000,000 nanoseconds
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
    printf("set trigger!\n");
}

void setBrightness(char* flag){
    FILE *pledBrightnessFile = fopen(LED_BRIGHTNESS_FILE_NAME, "w");
    if (pledBrightnessFile == NULL) {
        perror("Error opening LED trigger file");
        exit(EXIT_FAILURE);
    }
    int charWritten = fprintf(pledBrightnessFile, flag);
    if (charWritten <= 0) {
        perror("Error writing data to LED file");
        exit(EXIT_FAILURE);
    }
    fclose(pledBrightnessFile);
}