/*******************************************************************************
 * File Name    : <led.c>
 * Description  : <LED 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : 매개변수 device_state로 수정 (<김명준>)
 *******************************************************************************/

#include "state.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <syslog.h>

#define	LED	1

/**
 * @brief   LED 켜기
 */
static void led_on(struct device_state* const arg);

/**
 * @brief   LED 끄기
 */
static void led_off(struct device_state* const arg);

/**
 * @brief   LED 점멸
 */
static void led_blink(struct device_state* const arg);

/**
 * @brief   LED 밝기 최소(20%)
 */
static void led_low(struct device_state* const arg);

/**
 * @brief   LED 밝기 중간(60%)
 */
static void led_mid(struct device_state* const arg);

/**
 * @brief   LED 밝기 최대(100%)
 */
static void led_high(struct device_state* const arg);

/**
 * @brief   명령어 테이블
 */
struct command_map {
	const char *command;                          /**< 명령어 */
	void (*action)(struct device_state* const);   /**< 수행할 함수 */
};

/**
 * @brief   명령어 바인딩
 */
const static struct command_map cmd[] = {
	{"ON", led_on},
	{"OFF", led_off},
	{"BLINK", led_blink},
    {"LOW", led_low},
    {"MID", led_mid},
    {"HIGH", led_high},
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

void led_function(struct device_state* const arg)
{
    if (arg == NULL) {
        syslog(LOG_WARNING, "LED Module: argument is NULL");
        return;
    }

	for (int i = 0; i < cmd_size; i++) {
		if (!strcasecmp(arg->led, cmd[i].command)) {
            syslog(LOG_INFO, "LED Module: executing command [%s]", arg->led);
			cmd[i].action(arg);
			return;
		}
	}

    syslog(LOG_WARNING, "LED Module: unknown command [%s]", arg->led);
}

static void led_on(struct device_state* const arg)
{
    softPwmWrite(LED, 100);
}

static void led_off(struct device_state* const arg)
{
    softPwmWrite(LED, 0);
}

static void led_blink(struct device_state* const arg)
{
    while (!strcasecmp(arg->led, "BLINK")) {
        softPwmWrite(LED, 100);
        delay(500);

        if (strcasecmp(arg->led, "BLINK")) break; // 중간에 값 바뀔 시 대비

        softPwmWrite(LED, 0);
        delay(500);
    }
}

static void led_low(struct device_state* const arg)
{
    softPwmWrite(LED, 20);
}

static void led_mid(struct device_state* const arg)
{
    softPwmWrite(LED, 60);
}

static void led_high(struct device_state* const arg)
{
    softPwmWrite(LED, 100);
}