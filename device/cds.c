/*******************************************************************************
 * File Name    : <cds.c>
 * Description  : <조도 센서 하드웨어 제어 라이브러리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <syslog.h>

#define	LED	1

static int is_initialized; // 초기화 여부
static int fd;

void cds_function(int* threshold)
{
	if (!is_initialized) {
		if ((fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x48)) < 0) {
			syslog(LOG_ERR, "CDS Module: wiringPiI2CSetupInterface() failed");
        	return;
		}

		if (wiringPiSetup() == -1) {
            syslog(LOG_ERR, "CDS Module: wiringPiSetup() failed");
            return;
        }

		pinMode(LED, OUTPUT);

		syslog(LOG_INFO, "CDS Module: Initialized successfully");
		is_initialized = 1;
	}

	if (threshold == NULL) {
        syslog(LOG_WARNING, "CDS Module: argument is NULL");
        return;
    }

	volatile int* live_threshold = (volatile int*)threshold;
	
	while (*live_threshold >= 0) {
		wiringPiI2CWrite(fd, 0x00);

		int val = wiringPiI2CRead(fd);

		if (val < *live_threshold) {
			digitalWrite(LED, LOW);
		}
		else {
			digitalWrite(LED, HIGH);
		}

		delay(500);
	}

	digitalWrite(LED, LOW);
}
