/*******************************************************************************
 * File Name    : <led.c>
 * Description  : <LED 하드웨어 제어 라이브러리>
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
#include <softPwm.h>
#include <syslog.h>

#define	LED	1

/**
 * @brief   LED 켜기
 */
static void led_on();

/**
 * @brief   LED 끄기
 */
static void led_off();

/**
 * @brief   LED 점멸
 */
static void led_blink();

/**
 * @brief   LED 밝기 최소(20%)
 */
static void led_low();

/**
 * @brief   LED 밝기 중간(60%)
 */
static void led_mid();

/**
 * @brief   LED 밝기 최대(100%)
 */
static void led_high();

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
	{"ON", led_on},
	{"OFF", led_off},
	{"BLINK", led_blink},
    {"LOW", led_low},
    {"MID", led_mid},
    {"HIGH", led_high},
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

static int is_initialized = 0; // 초기화 여부

void led_function(char* arg)
{
    if (!is_initialized) {
        if (wiringPiSetup() == -1) {
            syslog(LOG_ERR, "LED Module: wiringPiSetup() failed");
            return;
        }

        pinMode(LED, OUTPUT);

        if (softPwmCreate(LED, 0, 100) != 0) {
            syslog(LOG_ERR, "LED Module: softPwmCreate() failed");
            return;
        }

        syslog(LOG_INFO, "LED Module: Initialized successfully");
        is_initialized = 1;
    }
	
    if (arg == NULL) {
        syslog(LOG_WARNING, "LED Module: argument is NULL");
        return;
    }

	for (int i = 0; i < cmd_size; i++) {
		if (!strcasecmp(arg, cmd[i].command)) {
            syslog(LOG_INFO, "LED Module: executing command [%s]", arg);
			cmd[i].action();
			return;
		}
	}

    syslog(LOG_WARNING, "LED Module: unknown command [%s]", arg);
}

static void led_on()
{
    softPwmWrite(LED, 100);
}

static void led_off()
{
    softPwmWrite(LED, 0);
}

static void led_blink()
{
	softPwmWrite(LED, 100);
	delay(500);
	softPwmWrite(LED, 0);
	delay(500);
}

static void led_low()
{
    softPwmWrite(LED, 20);
}

static void led_mid()
{
    softPwmWrite(LED, 60);
}

static void led_high()
{
    softPwmWrite(LED, 100);
}