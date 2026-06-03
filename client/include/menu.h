/*******************************************************************************
 * File Name    : <menu.c>
 * Description  : <리눅스 클라이언트 메뉴 함수 선언부>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.02> : 함수 주석 (<김명준>)
 * - <2026.06.03> : main.c에서 input_num 사용하도록 수정 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   사용자 문자열 입력 받는 함수
 * @param   sockfd 서버 소켓
 * @param   msg 화면에 출력할 메시지
 * @param   buf 입력받을 공간
 */
void input_option(const int sockfd, const char * const msg, char * const buf);

/**
 * @brief   사용자 숫자 입력 받는 함수
 * @param   sockfd 서버 소켓
 * @param   msg 화면에 출력할 메시지
 */
int input_num(const int sockfd, const char * const msg);

/**
 * @brief   화면에 메뉴(이벤트) 출력
 */
void print_menu(void);

/**
 * @brief   LED 메뉴
 * @details
 *          ON: LED 켜기
 *          OFF: LED 끄기
 *          BLINK: LED 점멸
 *          LOW: 밝기 20퍼
 *          MID: 밝기 60퍼
 *          HIGH: 밝기 100퍼
 *          Q: 나가기
 * @param   sockfd  서버 소켓 디스크립터
 */
void led_menu(const int);

/**
 * @brief   부저 메뉴
 * @details
 *          ON: 음악 켜기
 *          OFF: 음악 끄기
 *          Q: 나가기
 * @param   sockfd  서버 소켓 디스크립터
 */
void buzzer_menu(const int);

/**
 * @brief   조도 센서 메뉴
 * @details
 *          ON: 임계값 설정 후 빛 감지 시작 -> 어두우면 LED 켜기, 밝으면 LED 끄기
 *          OFF: 감지 끝
 *          Q: 나가기
 * @param   sockfd  서버 소켓 디스크립터
 */
void cds_menu(const int);

/**
 * @brief   7 세그먼트 메뉴
 * @details 0 ~ 9 사이의 숫자 입력
 * @param   sockfd  서버 소켓 디스크립터
 */
void segment_menu(const int);

/**
 * @brief   클라이언트 종료
 * @param   sockfd  서버 소켓 디스크립터
 */
void exit_menu(const int);