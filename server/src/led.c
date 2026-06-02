#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <softPwm.h>
// #include <syslog.h>

#define	LED	1

static void led_on();
static void led_off();
static void led_blink();
static void led_low();
static void led_mid();
static void led_high();

struct command_map {
	const char *command;
	void (*action)(void);
};

const static struct command_map cmd[] = {
	{"ON", led_on},
	{"OFF", led_off},
	{"BLINK", led_blink},
    {"LOW", led_low},
    {"MID", led_mid},
    {"HIGH", led_high},
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

void led_function(char* arg)
{
	if (wiringPiSetup() == -1) {
        // syslog(LOG_ERR, "LED Module: wiringPiSetup() failed");
        return;
    }
	pinMode(LED, OUTPUT);

    if (softPwmCreate(LED, 0, 100) != 0) {
        // syslog(LOG_ERR, "LED Module: softPwmCreate() failed");
        return;
    }

    if (arg == NULL) {
        // syslog(LOG_WARNING, "LED Module: argument is NULL");
        return;
    }

	for (int i = 0; i < cmd_size; i++) {
		if (strcmp(arg, cmd[i].command) == 0) {
            // syslog(LOG_INFO, "LED Module: executing command [%s]", arg);
			cmd[i].action();
			return;
		}
	}

    // syslog(LOG_WARNING, "LED Module: unknown command [%s]", arg);
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