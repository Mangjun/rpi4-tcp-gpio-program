/*******************************************************************************
 * File Name    : <menu.c>
 * Description  : <리눅스 클라이언트 메뉴 인터페이스 및 서버 통신>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.02> : 함수 주석 및 safe 함수로 변환 ex) scanf -> fgets, 대소문자 구분 X strcmp -> strcasecmp (<김명준>)
 *******************************************************************************/

#include "menu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAX_BUF_SIZE 1024

static const char* menu_name[] = { "LED", "BUZZER", "CDS", "SEGMENT", "EXIT" };

/**
 * @brief   TCP send 함수
 * @details /target/action 형식으로 보내기로 했음 ex) /led/on
 * @param   target  특정 하드웨어 (LED, 부저, 조도 센서, 7세그먼트)
 * @param   action  수행할 명령 (ON, OFF 등)
 */
static void send_command(const int sockfd, const char *target, const char *action) {
    char buf[MAX_BUF_SIZE] = {0};
    snprintf(buf, MAX_BUF_SIZE, "/%s/%s", target, action);
    
    if (send(sockfd, buf, strlen(buf), 0) == -1) {
        perror("send error");
    }
}

/**
 * @brief   사용자 문자열 입력 받는 함수
 * @param   msg 화면에 출력할 메시지
 * @param   buf 입력받을 공간
 */
static inline void input_option(const char * const msg, char * const buf)
{
    printf("%s", msg);
    memset(buf, 0, MAX_BUF_SIZE);
    fgets(buf, MAX_BUF_SIZE, stdin);

    // MAX_BUF_SIZE보다 더 많이 입력했을 시
    if (!strchr(buf, '\n')) {
        while (getchar() != '\n'); // 버퍼 비우기
    }
    else {
        buf[strcspn(buf, "\n")] = '\0'; // fgets는 \n까지 포함하므로 제거해줘야 함
    }
}

/**
 * @brief   사용자 숫자 입력 받는 함수
 * @param   msg 화면에 출력할 메시지
 */
static inline int input_num(const char * const msg)
{
    char buf[33] = {0};
    printf("%s", msg);
    fgets(buf, sizeof(buf), stdin);

    if (!strchr(buf, '\n')) {
        while (getchar() != '\n'); 
    }

    return atoi(buf);
}

void print_menu(void)
{
    int i;
    printf("\n===============================\n");
    for (i = 0; i < 5; i++) {
        printf("%d. %s\n", i + 1, menu_name[i]);
    }
    printf("===============================\n");
    printf("Input > ");
}

void led_menu(int sockfd)
{
    char buf[MAX_BUF_SIZE];

    while (1) {
        printf("Usage: [ON | OFF | BLINK | LOW | MID | HIGH | Q]\n");
        input_option("LED> ", buf);

        if (!strcasecmp(buf, "Q")) {
            break;
        }

        send_command(sockfd, "led", buf);
    }
}

void buzzer_menu(int sockfd)
{
    char buf[MAX_BUF_SIZE];

    while (1) {
        printf("Usage: [ON | OFF | Q]\n");
        input_option("BUZZER> ", buf);

        if (!strcasecmp(buf, "Q")) {
            break;
        }

        send_command(sockfd, "buzzer", buf);
    }
}

void cds_menu(int sockfd)
{
    char buf[MAX_BUF_SIZE];

    while (1) {
        printf("Usage: [ON | OFF | Q]\n");
        input_option("CDS> ", buf);

        if (!strcasecmp(buf, "OFF")) {
            send_command(sockfd, "cds", "OFF");
        }
        else if (!strcasecmp(buf, "ON")) {
            int threshold;
            char arg[33]; // int형 크기 (4 * 8 = 32bit + \0 1bit)

            threshold = input_num("input threshold: ");

            if (threshold < 0) {
                threshold = 0;
            }
            
            snprintf(arg, sizeof(arg), "%d", threshold);
            send_command(sockfd, "cds", arg);
        }
        else if (!strcasecmp(buf, "Q")) {
            break;
        }
    }
}

void segment_menu(int sockfd)
{
    int num;

    num = input_num("input Num: ");

    if (num < 0 || num > 9) {
        printf("Input 0 ~ 9\n");
        return;
    }
    else {
        char arg[16];
        snprintf(arg, sizeof(arg), "%d", num);
        send_command(sockfd, "segment", arg);
    }
}

void exit_menu(int sockfd)
{
    printf("\nExit...\n");
    close(sockfd);
    exit(0);
}