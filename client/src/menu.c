#include "menu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAX_BUF_SIZE 80

static void send_command(int sockfd, const char *target, const char *action) {
    char buffer[128] = {0};
    snprintf(buffer, sizeof(buffer), "/%s/%s", target, action);
    
    if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
        perror("send error");
    }
}

static inline void input_option(const char * const msg, char * const buf)
{
    printf("%s", msg);
    memset(buf, 0, MAX_BUF_SIZE);
    scanf("%s", buf);
}

void print_menu(void)
{
    printf("\n===============================\n");
    printf("1. LED\n");
    printf("2. BUZZER\n");
    printf("3. CDS\n");
    printf("4. SEGMENT\n");
    printf("5. EXIT\n");
    printf("===============================\n");
    printf("Input > ");
}

void led_menu(int sockfd)
{
    char buf[MAX_BUF_SIZE];

    while (1) {
        printf("Usage: [ON | OFF | BLINK | LOW | MID | HIGH | Q]\n");
        input_option("LED> ", buf);

        if (strcmp(buf, "Q") == 0 || strcmp(buf, "q") == 0) {
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

        if (strcmp(buf, "Q") == 0 || strcmp(buf, "q") == 0) {
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

        if (strcmp(buf, "OFF") == 0 || strcmp(buf, "off") == 0) {
            send_command(sockfd, "cds", "OFF");
        }
        else if (strcmp(buf, "ON") == 0 || strcmp(buf, "on") == 0) {
            int threshold;
            printf("input threshold: ");
            scanf("%d", &threshold);

            if (threshold < 0) {
                threshold = 0;
            }
            
            char arg[32];
            sprintf(arg, "%d", threshold);
            send_command(sockfd, "cds", arg);
        }
        else if (strcmp(buf, "Q") == 0 || strcmp(buf, "q") == 0) {
            break;
        }
    }
}

void segment_menu(int sockfd)
{
    int num;

    printf("input Num: ");
    scanf("%d", &num);

    if (num < 0 || num > 9) {
        printf("Input 0 ~ 9\n");
        return;
    }
    else {
        char arg[16];
        sprintf(arg, "%d", num);
        send_command(sockfd, "segment", arg);
    }
}

void exit_menu(int sockfd)
{
    printf("Exit...\n");
    close(sockfd);
    exit(0);
}