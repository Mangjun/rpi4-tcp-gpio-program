/*******************************************************************************
 * File Name    : <handler.c>
 * Description  : <작업 스레드 디스패처 구현부>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : 모든 device의 상태를 저장하는 구조체 만들어서 처리하도록 수정 (<김명준>)
 *******************************************************************************/

#include "handler.h"
#include "thread.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <syslog.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <softTone.h>

#define	LED	1
#define BUZZER 6
static const int segment_pins[7] = { 29, 28, 27, 25, 24, 23, 22 };

extern struct device_state g_state;

static const char * led_status[] = { "ON", "OFF", "BLINK", "LOW", "MID", "HIGH" };
static const int led_status_count = sizeof(led_status) / sizeof(led_status[0]);

pthread_mutex_t cds_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t segment_mutex = PTHREAD_MUTEX_INITIALIZER;

int handler_init(void)
{
    if (wiringPiSetup() == -1) {
        syslog(LOG_ERR, "wiringPiSetup() failed");
        return -1;
    }

    if (softToneCreate(BUZZER) == -1) {
        syslog(LOG_ERR, "softToneCreate() failed");
        return -1;
    }

    if (softPwmCreate(LED, 0, 100) != 0) {
        syslog(LOG_ERR, "softPwmCreate() failed");
        return -1;
    }

    for (int i = 0; i < 7; i++) {
        pinMode(segment_pins[i], OUTPUT);
    }

    syslog(LOG_INFO, "GPIO Initialized successfully");
    return 0;
}

void handle_led(const char *action)
{
    pthread_t tid;
    
    int i;
    for (i = 0; i < led_status_count; i++) {
        if (!strcasecmp(action, led_status[i])) {
            snprintf(g_state.led, sizeof(g_state.led), "%s", action);
            break;
        }
    }

    if (pthread_create(&tid, NULL, led_thread, (void *)&g_state)) {
        syslog(LOG_ERR, "LED pthread_create");
        return;
    }
    
    pthread_detach(tid);
}

void handle_buzzer(const char *action)
{
    pthread_t tid;

    if (!strcasecmp(action, "ON")) {
        g_state.buzzer_on = 1;
    }
    else if (!strcasecmp(action, "OFF")) {
        g_state.buzzer_on = 0;
    }

    if (pthread_create(&tid, NULL, buzzer_thread, (void *)&g_state)) {
        syslog(LOG_ERR, "BUZZER pthread_create");
        return;
    }

    pthread_detach(tid);
}

void handle_cds(const char *action)
{
    pthread_t tid;

    if (!strcasecmp(action, "OFF")) {
        g_state.cds_threshold = -1;
        g_state.cds_on = 0;
    }
    else {
        int val = atoi(action);
        if (val <= 0 && strcmp(action, "0")) {
            val = 0;
        }

        pthread_mutex_lock(&cds_mutex);
        g_state.cds_threshold = val;
        pthread_mutex_unlock(&cds_mutex);

        if (!g_state.cds_on) {
            g_state.cds_on = 1;
            if (pthread_create(&tid, NULL, cds_thread, (void *)&g_state)) {
                syslog(LOG_ERR, "CDS pthread_create");
                return;
            }

            pthread_detach(tid);
        }
    }
}

void handle_segment(const char *action)
{
    pthread_t tid;
    int num = atoi(action);

    if (num < 0 || num > 9) {
        return;
    }

    pthread_mutex_lock(&segment_mutex);
    if (g_state.segment_on) {
        pthread_mutex_unlock(&segment_mutex);
        return;
    }

    g_state.segment = num;
    g_state.segment_on = 1;
    pthread_mutex_unlock(&segment_mutex);

    if (pthread_create(&tid, NULL, segment_thread, (void *)&g_state)) {
        syslog(LOG_ERR, "SEGMENT pthread_create");
        pthread_mutex_lock(&segment_mutex);
        g_state.segment_on = 0;
        pthread_mutex_unlock(&segment_mutex);
        return;
    }

    pthread_detach(tid);
}