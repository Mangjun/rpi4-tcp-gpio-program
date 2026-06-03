/*******************************************************************************
 * File Name    : <buzzer.c>
 * Description  : <부저 하드웨어 제어 라이브러리>
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
#define TOTAL 32

/**
 * @brief   악보
 */
static const int notes[TOTAL] = {
    391, 391, 440, 391, 391, 330, 330, 330, 
    391, 391, 330, 330, 294, 294, 294, 0, 
    391, 391, 440, 440, 391, 391, 330, 330, 
    391, 330, 294, 330, 262, 262, 262, 0
};

/**
 * @brief   음악 켜기
 */
static void music_on(struct device_state * const arg);

/**
 * @brief   음악 끄기
 */
static void music_off(struct device_state * const arg);

void buzzer_function(struct device_state * const arg) {
    if (arg == NULL) {
        syslog(LOG_WARNING, "BUZZER Module: argument is NULL");
        return;
    }

	if (arg->buzzer_on == 1) {
        syslog(LOG_INFO, "BUZZER Module: Music ON");
        music_on(arg);
    }
    else {
        syslog(LOG_INFO, "BUZZER Module: Music OFF");
        music_off(arg);
    }
}

static void music_on(struct device_state * const arg)
{
    int i;

    for (i = 0; i < TOTAL; i++) {
        if (!arg->buzzer_on) break;
        softToneWrite(BUZZER, notes[i]);
        delay(280);
    }

    softToneWrite(BUZZER, 0);
    arg->buzzer_on = 0;
}

static void music_off(struct device_state * const arg)
{
    softToneWrite(BUZZER, 0); 
}