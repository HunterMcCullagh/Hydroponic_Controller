#include "string_functions.h"
#include "ESP32_hydroponic_control.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int process_input(char* input)
{
    int value;

    

    if (strncmp(input, "heater+p+", 9) == 0) {
        if (sscanf(input + 9, "%d", &value) == 1) {
            heater_p = value;
            return 1;
        } 
    } 
    else if (strncmp(input, "heater+i+", 9) == 0) {
        if (sscanf(input + 9, "%d", &value) == 1) {
            heater_i = value;
            return 1;
        }
    } 
    else if (strncmp(input, "heater+d+", 9) == 0) {
        if (sscanf(input + 9, "%d", &value) == 1) {
            heater_d = value;
            return 1;
        } 
    } 
    else {
        //printf("Unknown command\n");
    }
    return 0;
     
}