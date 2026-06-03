/*******************************************************************************
 * File Name    : <buzzer.c>
 * Description  : <부저 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
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
static void music_on();

/**
 * @brief   음악 끄기
 */
static void music_off();

/**
 * @brief   명령어 테이블
 */
struct command_map {
	const char *command;    /**< 명령어 */
	void (*action)(void);   /**< 수행할 함수 */
};

/**
 * @brief   명령어 바인딩
 */
const static struct command_map cmd[] = {
	{"ON", music_on},
	{"OFF", music_off},
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

static int is_initialized = 0; // 초기화 여부

volatile int keep_playing = 0; // 음악 수행 여부

void buzzer_function(char* arg) {
    if (!is_initialized) {
        if (wiringPiSetup() == -1) {
            syslog(LOG_ERR, "BUZZER Module: wiringPiSetup() failed");
            return;
        }

        if (softToneCreate(BUZZER) == -1) {
            syslog(LOG_ERR, "BUZZER Module: softToneCreate() failed");
            return;
        }

        syslog(LOG_INFO, "BUZZER Module: Initialized successfully");
        is_initialized = 1;
    }
    
    if (arg == NULL) {
        syslog(LOG_WARNING, "BUZZER Module: argument is NULL");
        return;
    }

	for (int i = 0; i < cmd_size; i++) {
		if (!strcasecmp(arg, cmd[i].command)) {
            syslog(LOG_INFO, "BUZZER Module: executing command [%s]", arg);
			cmd[i].action();
			return;
		}
	}

    syslog(LOG_WARNING, "BUZZER Module: unknown command [%s]", arg);
}

static void music_on()
{
    int i;

    if (keep_playing == 1) {
        return; 
    }

    keep_playing = 1;
    for (i = 0; i < TOTAL; i++) {
        if (!keep_playing) break;
        softToneWrite(BUZZER, notes[i]);
        delay(280);
    }

    softToneWrite(BUZZER, 0);
    keep_playing = 0;
}

static void music_off()
{
    keep_playing = 0;
    softToneWrite(BUZZER, 0); 
}