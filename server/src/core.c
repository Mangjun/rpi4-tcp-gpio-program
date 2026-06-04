/*******************************************************************************
 * File Name    : <core.c>
 * Description  : <서버 초기화 및 코어 동작 구현>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 * - <2026.06.04> : HTTP 요청 추가 (<김명준>)
 *******************************************************************************/

#include "core.h"
#include "list.h"
#include "handler.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <syslog.h>

#define MAX_EVENTS 64
#define BACKLOG 20
#define TCP_PORT 60000
#define HTTP_PORT 8080
#define MAX_BUF_SIZE 1024

struct command_map {
	const char *command;
	void (*action)(const char *);
};

const static struct command_map cmd[] = {
	{"led", handle_led},
    {"buzzer", handle_buzzer},
    {"cds", handle_cds},
    {"segment", handle_segment}
};

/**
 * @brief   TCP인지 HTTP인지 구분하기 위한 세션 구조체
 */
struct session {
    int fd;                  /**< 파일 디스크립터 */
    struct node* list_node;  /**< 리스트의 위치 (TCP는 NULL) */
};

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

struct list* client_list;

/**
 * @brief   수신된 명령어 파싱하는 헬퍼 함수
 */
static int parse_command(const char *input, char *target, char *action) {
    if (input == NULL || target == NULL || action == NULL) {
        return -1;
    }

    if (sscanf(input, "/%63[^/]/%63s", target, action) == 2) {
        return 0;
    }

    return -1;
}

/**
 * @brief   html 파일 보내는 헬퍼 함수
 */
static void send_html(int client_fd, const char *filename) {
    char header[] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: close\r\n\r\n";
    
    send(client_fd, header, strlen(header), 0);

    int fd = open(filename, O_RDONLY);
    if (fd != -1) {
        char file_buf[MAX_BUF_SIZE];
        int len;
        while ((len = read(fd, file_buf, sizeof(file_buf))) > 0) {
            send(client_fd, file_buf, len, 0);
        }
        close(fd);
    } else {
        const char *err = "<h1>404 Not Found</h1>";
        send(client_fd, err, strlen(err), 0);
    }
}

/**
 * @brief   각 웹 클라이언트에게 SSE 응답 보내는 헬퍼 함수
 */
static int send_sse_data_cb(const int client_fd, void *arg) {
    const char *sse_msg = (const char *)arg;
    int len = strlen(sse_msg);

    int ret = send(client_fd, sse_msg, len, MSG_NOSIGNAL);

    if (ret < 0) {
        syslog(LOG_INFO, "SSE Stream Disconnected [FD: %d]", client_fd);
        close(client_fd);
        return -1;
    }
    
    return 0;
}

void broadcast_sse() {
    if (!client_list || !client_list->pimpl) return;

    extern struct device_state g_state;
    char json_buf[256];
    
    // JSON 형식
    snprintf(json_buf, sizeof(json_buf),
        "{\"led\":\"%s\", \"buzzer_on\":%d, \"cds_on\":%d, \"cds_threshold\":%d, \"cds_current\":%d, \"segment_on\":%d, \"segment\":%d}",
        g_state.led, g_state.buzzer_on, g_state.cds_on, g_state.cds_threshold, 
        g_state.cds_current, g_state.segment_on, g_state.segment);

    char sse_msg[MAX_BUF_SIZE];
    snprintf(sse_msg, sizeof(sse_msg), "data: %s\n\n", json_buf);

    client_list->ops->for_each(client_list, send_sse_data_cb, (void *)sse_msg);
}

void daemonize()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);

    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    int fd = open("/dev/null", O_RDWR);
    /* 컴파일 경고 지우기 용도 */
    if (fd != -1) {
        if (dup2(fd, STDIN_FILENO) == -1) exit(EXIT_FAILURE);
        if (dup2(fd, STDOUT_FILENO) == -1) exit(EXIT_FAILURE);
        if (dup2(fd, STDERR_FILENO) == -1) exit(EXIT_FAILURE);
        
        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }
}

int init_server_socket(const int port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        syslog(LOG_ERR, "Socket Create Fail...");
        return -1;
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        syslog(LOG_ERR, "Bind Fail...");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, BACKLOG) == -1) {
        syslog(LOG_ERR, "Listen Fail...");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void run(const int tcp_sockfd, const int http_sockfd)
{
    int new_fd;
    struct sockaddr_in client_addr;
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    int event_count, num_bytes;
    int i;

    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((epoll_fd = epoll_create(MAX_EVENTS)) == -1) {
        syslog(LOG_ERR, "Epoll Create Fail...");
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLIN;
    event.data.fd = tcp_sockfd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_sockfd, &event) == -1) {
        syslog(LOG_ERR, "Epoll Add TCP Socket Fail...");
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLIN;
    event.data.fd = http_sockfd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, http_sockfd, &event) == -1) {
        syslog(LOG_ERR, "Epoll Add HTTP Socket Fail...");
        exit(EXIT_FAILURE);
    }

    client_list = list_create();

    if (!client_list) {
        syslog(LOG_ERR, "Client List Init Fail...");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Server Run...");

    while (1) {
        if ((event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1) {
            syslog(LOG_ERR, "Epoll Wait Fail...");
            continue;
        }

        for (i = 0; i < event_count; i++) {
            if (events[i].data.fd == tcp_sockfd) {
                if ((new_fd = accept(tcp_sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
                    syslog(LOG_ERR, "Accept Fail...");
                    continue;
                }

                struct session *client = malloc(sizeof(struct session));

                if (!client) {
                    syslog(LOG_ERR, "TCP Session malloc Fail...");
                    continue;
                }

                client->fd = new_fd;
                client->list_node = NULL;

                event.events = EPOLLIN;
                event.data.ptr = client;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
                    syslog(LOG_ERR, "Epoll Add Client Fail...");
                    close(new_fd);
                    free(client);
                    continue;
                }

                syslog(LOG_INFO, "New TCP Client Connected! [FD: %d]", new_fd);
            }
            else if (events[i].data.fd == http_sockfd) {
                if ((new_fd = accept(http_sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
                    syslog(LOG_ERR, "Accept Fail...");
                    continue;
                }

                struct session *client = (struct session*)malloc(sizeof(struct session));

                if (!client) {
                    syslog(LOG_ERR, "HTTP Session malloc Fail...");
                    continue;
                }

                client->fd = new_fd;
                client->list_node = client_list->ops->insert_first(client_list, new_fd);

                event.events = EPOLLIN;
                event.data.ptr = client;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
                    syslog(LOG_ERR, "Epoll Add Client Fail...");
                    close(new_fd);
                    client_list->ops->remove_node(client_list, client->list_node);
                    free(client);
                    continue;
                }

                syslog(LOG_INFO, "New HTTP Client Connected! [FD: %d]", new_fd);
            }
            else {
                struct session *client = (struct session *)events[i].data.ptr;
                int client_fd = client->fd;

                char buf[MAX_BUF_SIZE] = {0};
                num_bytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
                
                if (num_bytes <= 0) {
                    syslog(LOG_INFO, "Client Disconnected [FD: %d]", client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);

                    if (client->list_node != NULL) {
                        client_list->ops->remove_node(client_list, client->list_node);
                    }
                    
                    free(client);
                }
                else {
                    buf[num_bytes] = '\0';

                    /* TCP Request 처리 */
                    if (client->list_node == NULL) {
                        char target[64] = {0};
                        char action[64] = {0};

                        if (parse_command(buf, target, action)) {
                            syslog(LOG_WARNING, "TCP Request parse Fail...");
                            continue;
                        }

                        syslog(LOG_INFO, "TCP Request: target[%s], action[%s]", target, action);

                        int is_handled = 0;
                        int j;
                        for (j = 0; j < cmd_size; j++) {
                            if (!strcasecmp(target, cmd[j].command)) {
                                cmd[j].action(action);
                                is_handled = 1;
                                break;
                            }
                        }

                        if (!is_handled) {
                            syslog(LOG_WARNING, "Unknown TCP Request target[%s], action[%s]", target, action);
                        }

                        extern struct device_state g_state;
                        char res_buf[MAX_BUF_SIZE] = {0};

                        if (!strcasecmp(target, "CDS") && strcasecmp(action, "OFF")) {
                            sleep(1);
                            snprintf(res_buf, sizeof(res_buf),
                                 "[ACK] 명령 처리 완료\n"
                                 "  └─ 현재 장치 상태 ──> LED: %s | BUZZER: %s | CDS: %d | SEGMENT: %d",
                                 g_state.led,
                                 (g_state.buzzer_on) ? "ON" : "OFF",
                                 g_state.cds_current,
                                 g_state.segment);

                            send(client_fd, res_buf, strlen(res_buf), 0);
                        }
                    }
                    /* HTTP Request 처리 */
                    else {
                        char method[16] = {0};
                        char path[256] = {0};

                        if (sscanf(buf, "%15s %255s", method, path) == 2) {
                            
                            // index.html 요청
                            if (strcasecmp(method, "GET") == 0 && (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0)) {
                                syslog(LOG_INFO, "HTTP GET %s [FD: %d]", path, client_fd);
                                send_html(client_fd, "index.html");
                                
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                close(client_fd);
                                client_list->ops->remove_node(client_list, client->list_node);
                                free(client);
                            }
                            
                            // SSE 요청
                            else if (strcasecmp(method, "GET") == 0 && strcmp(path, "/stream") == 0) {
                                syslog(LOG_INFO, "HTTP SSE Stream Connected [FD: %d]", client_fd);
                                
                                const char *sse_header = 
                                    "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/event-stream\r\n"
                                    "Cache-Control: no-cache\r\n"
                                    "Connection: keep-alive\r\n\r\n";
                                send(client_fd, sse_header, strlen(sse_header), 0);
                                
                                broadcast_sse();
                            }

                            // 명령어 요청
                            else if (strcasecmp(method, "POST") == 0 && strncmp(path, "/cmd?", 5) == 0) {
                                char target[64] = {0};
                                char action[64] = {0};

                                if (sscanf(path, "/cmd?target=%63[^&]&action=%63s", target, action) == 2) {
                                    syslog(LOG_INFO, "HTTP POST CMD: target[%s], action[%s]", target, action);
                                    
                                    int j;
                                    for (j = 0; j < cmd_size; j++) {
                                        if (!strcasecmp(target, cmd[j].command)) {
                                            cmd[j].action(action);
                                            break;
                                        }
                                    }
                                }

				// 연결 종료
                                const char *ok_res = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
                                send(client_fd, ok_res, strlen(ok_res), 0);

                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                close(client_fd);
                                client_list->ops->remove_node(client_list, client->list_node);
                                free(client);
                            }
                            
                            // 비정상 접근
                            else {
                                const char *err_res = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
                                send(client_fd, err_res, strlen(err_res), 0);
                                
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                                close(client_fd);
                                client_list->ops->remove_node(client_list, client->list_node);
                                free(client);
                            }
                        }
                    }
                }
            }
        } // for (i = 0; i < event_count; i++) end
    } // while (1) end

    close(tcp_sockfd);
    close(http_sockfd);
}
