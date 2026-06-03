/*******************************************************************************
 * File Name    : <state.c>
 * Description  : <전역 하드웨어 상태 저장 구조체 정의>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include "state.h"

struct device_state g_state = {
    .led            = "OFF",
    .buzzer_on      = 0,
    .cds_on         = 0,
    .cds_threshold  = -1,
    .cds_current    = 0,
    .segment_on     = 0,
    .segment        = 0,
};