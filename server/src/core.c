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

static int parse_command(const char *input, char *target, char *action) {
    if (input == NULL || target == NULL || action == NULL) {
        return -1;
    }

    if (sscanf(input, "/%63[^/]/%63s", target, action) == 2) {
        return 0;
    }

    return -1;
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
    struct list* client_list;
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

                    }
                }
            }
        } // for (i = 0; i < event_count; i++) end
    } // while (1) end

    close(tcp_sockfd);
    close(http_sockfd);
}