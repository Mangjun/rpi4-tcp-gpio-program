#include "thread.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
// #include <syslog.h>

#define	LED	1

static int is_initialized;
static int fd;

void cds_function(int* threshold)
{
	if (!is_initialized) {
		if ((fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x48)) < 0) {
			// syslog(LOG_ERR, "CDS Module: wiringPiI2CSetupInterface() failed");
        	return;
		}

		if (wiringPiSetup() == -1) {
            // syslog(LOG_ERR, "CDS Module: wiringPiSetup() failed");
            return;
        }

		pinMode(LED, OUTPUT);

		is_initialized = 1;
	}
	
	while (*threshold >= 0) {
		wiringPiI2CWrite(fd, 0x00);

		int val = wiringPiI2CRead(fd);

		if (val < *threshold) {
			digitalWrite(LED, HIGH);
		}
		else {
			digitalWrite(LED, LOW);
		}

		delay(500);
	}
}
