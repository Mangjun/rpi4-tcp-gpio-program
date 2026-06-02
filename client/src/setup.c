/*******************************************************************************
 * File Name    : <setup.c>
 * Description  : <클라이언트 설정>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/
#include "setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void setup_signal()
{
	sigset_t set;

	sigfillset(&set);
	sigdelset(&set, SIGINT);

	sigprocmask(SIG_SETMASK, &set, NULL);
}

int setup_server_socket(int* const sockfd, const char* ip, const char* port)
{
	struct sockaddr_in server_addr;
	
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port)); // 문자열이므로 숫자로 변환해줘야 함
    server_addr.sin_addr.s_addr = inet_addr(ip);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(*sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        close(*sockfd);
        return 1;
    }

	return 0;
}