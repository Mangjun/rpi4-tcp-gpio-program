/*******************************************************************************
 * File Name    : <state.c>
 * Description  : <전역 하드웨어 상태 저장 구조체 정의>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 * - <2026.06.04> : 하드웨어 상태 업데이트 캡슐화 및 MUTEX와 브로드캐스트 추가 (<김명준>)
 *******************************************************************************/

#include "state.h"
#include "core.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct device_state g_state = {
    .led            = "OFF",
    .buzzer_on      = 0,
    .cds_on         = 0,
    .cds_threshold  = -1,
    .cds_current    = 0,
    .segment_on     = 0,
    .segment        = 0,
};

void update_led_state(const char *new_state)
{
    int changed = 0;

    pthread_mutex_lock(&mutex);
    if (strcasecmp(g_state.led, new_state)) {
        snprintf(g_state.led, sizeof(g_state.led), "%s", new_state);
        changed = 1;
    }
    pthread_mutex_unlock(&mutex);

    if (changed) {
        broadcast_sse();
    }
}

void update_buzzer_state(const int new_state)
{
    int changed = 0;

    pthread_mutex_lock(&mutex);
    if (g_state.buzzer_on != new_state) {
        g_state.buzzer_on = new_state;
        changed = 1;
    }
    pthread_mutex_unlock(&mutex);


    if (changed) {
        broadcast_sse();
    }
}

void update_cds_state(const int cds_on_state, const int cds_threshold, const int cds_current)
{
    int changed = 0;

    pthread_mutex_lock(&mutex);
    if (g_state.cds_on != cds_on_state || g_state.cds_threshold != cds_threshold || g_state.cds_current != cds_current) {
        g_state.cds_on = cds_on_state;
        g_state.cds_threshold = cds_threshold;
        g_state.cds_current = cds_current;
        changed = 1;
    }
    pthread_mutex_unlock(&mutex);

    if (changed) {
        broadcast_sse();
    }
}

void update_segment_state(const int segment_on, const int segment)
{
    int changed = 0;

    pthread_mutex_lock(&mutex);
    if (g_state.segment_on != segment_on || g_state.segment != segment) {
        g_state.segment_on = segment_on;
        g_state.segment = segment;
        changed = 1;
    }
    pthread_mutex_unlock(&mutex);

    if (changed) {
        broadcast_sse();
    }
}