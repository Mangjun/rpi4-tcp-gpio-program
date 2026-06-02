#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <softTone.h>
// #include <syslog.h>

#define BUZZER 6

static void display_number(int num);
static void clear_segment();

static const int segment_pins[7] = { 29, 28, 27, 25, 24, 23, 22 };

static const int num_patterns[10][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0}, // 2
    {0, 0, 0, 0, 1, 1, 0}, // 3
    {1, 0, 0, 1, 1, 0, 0}, // 4
    {0, 1, 0, 0, 1, 0, 0}, // 5
    {0, 1, 0, 0, 0, 0, 0}, // 6
    {0, 0, 0, 1, 1, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0}, // 8
    {0, 0, 0, 0, 1, 0, 0}  // 9
};

static int is_initialized = 0;

void segment_function(int *arg) {
    if (!is_initialized) {
        if (wiringPiSetup() == -1) {
            // syslog(LOG_ERR, "SEGMENT Module: wiringPiSetup() failed");
            return;
        }

        for (int i = 0; i < 7; i++) {
            pinMode(segment_pins[i], OUTPUT);
        }

        if (softToneCreate(BUZZER) == -1) {
            // syslog(LOG_ERR, "SEGMENT Module: softToneCreate() failed");
            return;
        }

        is_initialized = 1;
    }
    
    if (arg == NULL) {
        // syslog(LOG_WARNING, "SEGMENT Module: argument is NULL");
        return;
    }

    clear_segment();

    int i;
    for (i = *arg; i >= 0; i--) {
        display_number(i);

        if (i == 0) {
            break;   
        }

        delay(1000);
    }

    softToneWrite(BUZZER, 1000);
    delay(1000);
    softToneWrite(BUZZER, 0);
    
    clear_segment();
}

static void display_number(int num)
{
    for (int i = 0; i < 7; i++) {
        digitalWrite(segment_pins[i], num_patterns[num][i]);
    }
}

static void clear_segment()
{
    for (int i = 0; i < 7; i++) {
        digitalWrite(segment_pins[i], 1); 
    }
}