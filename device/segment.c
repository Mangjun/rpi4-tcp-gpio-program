/*******************************************************************************
 * File Name    : <segment.c>
 * Description  : <7세그먼트 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : 매개변수 device_state로 수정 (<김명준>)
 *******************************************************************************/

#include "state.h"

#include <stdio.h>
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

void segment_function(struct device_state* const arg) {
    if (arg == NULL) {
        syslog(LOG_WARNING, "SEGMENT Module: argument is NULL");
        return;
    }

    if (!arg->segment_on) {
        return;
    }

    if (arg->segment < 0 || arg->segment > 9) {
        syslog(LOG_WARNING, "SEGMENT Module: Invalid timer value [%d]", arg->segment);
        return;
    }

    int current_count = arg->segment;

    clear_segment();

    for (; current_count >= 0; current_count--) {
        if (!arg->segment_on) {
            break;
        }

        update_segment_state(1, current_count);
        display_number(current_count);

        if (current_count == 0) {
            break;
        }

        delay(1000);
    }

    if (current_count == 0 && arg->segment_on) {
        softToneWrite(BUZZER, 1000);
        update_buzzer_state(1);
        delay(1000);
        softToneWrite(BUZZER, 0);
        update_buzzer_state(0);
    }

    clear_segment();

    update_segment_state(0, 0);
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