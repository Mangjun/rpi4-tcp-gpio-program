/*******************************************************************************
 * File Name    : <server.c>
 * Description  : <서버 메인 루프 및 클라이언트 세션 관리>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.03> : 분할하여 코드 간결화 (<김명준>)
 *******************************************************************************/

#include "handler.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#define TCP_PORT 60000
#define HTTP_PORT 8080

int main(void)
{
    int tcp_sockfd, http_sockfd;
    openlog("server", LOG_PID | LOG_CONS, LOG_USER);

    daemonize();

    if (handler_init()) {
        syslog(LOG_ERR, "Hardware Init Fail...");
        exit(EXIT_FAILURE);
    }

    tcp_sockfd = init_server_socket(TCP_PORT);

    if (tcp_sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    http_sockfd = init_server_socket(HTTP_PORT);

    if (http_sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    run(tcp_sockfd, http_sockfd);

    closelog();
    return 0;
}