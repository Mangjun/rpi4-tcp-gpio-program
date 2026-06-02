#include "handler.h"
#include "thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <syslog.h>

static int g_threshold = -1;
static int cds_on = 0;
static pthread_t tid_cds;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_led(const char *action) {
    pthread_t tid;
    char *arg_copy = strdup(action);
    if (pthread_create(&tid, NULL, led_thread, (void *)arg_copy)) {
        free(arg_copy);
        return;
    }
    
    pthread_detach(tid);
}

void handle_buzzer(const char *action) {
    pthread_t tid;
    char *arg_copy = strdup(action);
    if (pthread_create(&tid, NULL, buzzer_thread, (void *)arg_copy)) {
        free(arg_copy);
        return;
    }

    pthread_detach(tid);
}

void handle_cds(const char *action) {
    if (strcmp(action, "OFF") == 0 || strcmp(action, "off") == 0) {
        g_threshold = -1;
        cds_on = 0;
    }
    else {
        int val = atoi(action);
        if (val <= 0 && strcmp(action, "0") != 0) val = 0;

        pthread_mutex_lock(&mutex);
        g_threshold = val;
        pthread_mutex_unlock(&mutex);

        if (!cds_on) {
            if (pthread_create(&tid_cds, NULL, cds_thread, (void *)&g_threshold)) {
                return;
            }
            pthread_detach(tid_cds);
            cds_on = 1;
        } else {

        }
    }
}

void handle_segment(const char *action) {
    int num = atoi(action);
    if (num < 0 || num > 9) {
        return;
    }

    pthread_t tid;
    int *arg = (int *)malloc(sizeof(int));
    *arg = num;

    if (pthread_create(&tid, NULL, segment_thread, (void *)arg)) {
        syslog(LOG_ERR, "SEGMENT pthread_create failed");
        free(arg);
        return;
    }
    pthread_detach(tid);
}