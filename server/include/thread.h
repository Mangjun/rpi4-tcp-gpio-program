/*******************************************************************************
 * File Name    : <thread.h>
 * Description  : <비동기 하드웨어 제어를 위한 워커 스레드>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   LED 워커 스레드 생성
 */
void* led_thread(void* arg);

/**
 * @brief   부저 워커 스레드 생성
 */
void* buzzer_thread(void* arg);

/**
 * @brief   조도 센서 워커 스레드 생성
 */
void* cds_thread(void* arg);

/**
 * @brief   7 세그먼트 워커 스레드 생성
 */
void* segment_thread(void* arg);