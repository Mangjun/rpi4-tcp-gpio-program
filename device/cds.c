/*******************************************************************************
 * File Name    : <cds.c>
 * Description  : <조도 센서 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : 매개변수 device_state로 수정 (<김명준>)
 *******************************************************************************/

#include "state.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <syslog.h>

#define	LED	1

static int is_initialized; // 초기화 여부
static int fd;

void cds_function(struct device_state* const arg)
{
	if (!is_initialized) {
		if ((fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x48)) < 0) {
			syslog(LOG_ERR, "CDS Module: wiringPiI2CSetupInterface() failed");
        	return;
		}

		syslog(LOG_INFO, "CDS Module: Initialized successfully");
		is_initialized = 1;
	}

	if (arg == NULL) {
        syslog(LOG_WARNING, "CDS Module: argument is NULL");
        return;
    }

	while (arg->cds_on) {
		wiringPiI2CWrite(fd, 0x00);

		int val = wiringPiI2CRead(fd);
		
		update_cds_state(1, arg->cds_threshold, val);

		if (val < arg->cds_threshold) {
			softPwmWrite(LED, 0);
			update_led_state("OFF");
		}
		else {
			softPwmWrite(LED, 100);
			update_led_state("ON");
		}

		delay(500);
	}

	softPwmWrite(LED, 0);
	update_led_state("OFF");
}
