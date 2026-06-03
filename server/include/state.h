/*******************************************************************************
 * File Name    : <state.h>
 * Description  : <하드웨어 상태 저장 구조체 정의>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   하드웨어 상태 저장 구조체
 */
struct device_state {
    char led[16];       /**< LED 상태 */
    char buzzer_on;     /**< 부저 상태 */
    int cds_on;         /**< 조도 센서 상태 */
    int cds_threshold;  /**< 조도 센서 threshold 값 */
    int cds_current;    /**< 조도 센서 현재 감지 값 */
    int segment_on;     /**< 7 세그먼트 상태 */
    int segment;        /**< 7 세그먼트 현재 값 */
};

extern struct device_state g_state;