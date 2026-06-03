/*******************************************************************************
 * File Name    : <segment.c>
 * Description  : <7세그먼트 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <softTone.h>
#include <syslog.h>

#define BUZZER 6

/**
 * @brief   화면에 숫자 출력하는 헬퍼 함수
 * @param   num 숫자
 */
static void display_number(const int num);

/**
 * @brief   화면 지우는 헬퍼 함수
 */
static void clear_segment();

// GPIO A ~ G 핀
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

static int is_initialized = 0; // 초기화 여부

void segment_function(int *arg) {
    if (!is_initialized) {
        if (wiringPiSetup() == -1) {
            syslog(LOG_ERR, "SEGMENT Module: wiringPiSetup() failed");
            return;
        }

        for (int i = 0; i < 7; i++) {
            pinMode(segment_pins[i], OUTPUT);
        }

        if (softToneCreate(BUZZER) == -1) {
            syslog(LOG_ERR, "SEGMENT Module: softToneCreate() failed");
            return;
        }

        is_initialized = 1;
        syslog(LOG_INFO, "SEGMENT Module: Initialized successfully");
    }
    
    if (arg == NULL) {
        syslog(LOG_WARNING, "SEGMENT Module: argument is NULL");
        return;
    }

    if (*arg < 0 || *arg > 9) {
        syslog(LOG_WARNING, "SEGMENT Module: Invalid timer value [%d]", *arg);
        return;
    }

    clear_segment();

    syslog(LOG_INFO, "SEGMENT Module: Timer value [%d]", *arg);
    int i;
    for (i = *arg; i >= 0; i--) {
        display_number(i);

        if (i == 0) {
            break;   
        }

        delay(1000);
    }

    syslog(LOG_INFO, "SEGMENT Module: Timer 0");
    softToneWrite(BUZZER, 1000);
    delay(1000);
    softToneWrite(BUZZER, 0);
    
    clear_segment();
}

static void display_number(const int num)
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