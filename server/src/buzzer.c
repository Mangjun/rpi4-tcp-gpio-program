#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <softTone.h>
// #include <syslog.h>

#define BUZZER 6
#define TOTAL 32

static const int notes[TOTAL] = {
    391, 391, 440, 391, 391, 330, 330, 330, 
    391, 391, 330, 330, 294, 294, 294, 0, 
    391, 391, 440, 440, 391, 391, 330, 330, 
    391, 330, 294, 330, 262, 262, 262, 0
};

static void music_on();
static void music_off();

struct command_map {
	const char *command;
	void (*action)(void);
};

const static struct command_map cmd[] = {
	{"ON", music_on},
	{"OFF", music_off},
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

static int is_initialized = 0;

volatile int keep_playing = 0;

void buzzer_function(char* arg) {
    if (!is_initialized) {
        if (wiringPiSetup() == -1) {
            // syslog(LOG_ERR, "BUZZER Module: wiringPiSetup() failed");
            return;
        }

        if (softToneCreate(BUZZER) == -1) {
            // syslog(LOG_ERR, "BUZZER Module: softToneCreate() failed");
            return;
        }

        is_initialized = 1;
    }
    
    if (arg == NULL) {
        // syslog(LOG_WARNING, "BUZZER Module: argument is NULL");
        return;
    }

	for (int i = 0; i < cmd_size; i++) {
		if (strcmp(arg, cmd[i].command) == 0) {
            // syslog(LOG_INFO, "BUZZER Module: executing command [%s]", arg);
			cmd[i].action();
			return;
		}
	}

    // syslog(LOG_WARNING, "BUZZER Module: unknown command [%s]", arg);
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