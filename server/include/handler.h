/*******************************************************************************
 * File Name    : <handler.h>
 * Description  : <작업 스레드 디스패처>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : HW 초기화 함수 추가 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   하드웨어 초기 설정
 */
int handler_init(void);

/**
 * @brief   LED 워커 스레드 호출
 */
void handle_led(const char *action);

/**
 * @brief   부저 워커 스레드 호출
 */
void handle_buzzer(const char *action);

/**
 * @brief   조도 센서 워커 스레드 호출
 */
void handle_cds(const char *action);

/**
 * @brief   7 세그먼트 워커 스레드 호출
 */
void handle_segment(const char *action);