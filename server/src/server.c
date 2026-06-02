#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <pthread.h>

#include "handler.h"

#define BACKLOG 10
#define BUF_SIZE 1024
#define PORT 60000

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

const static int cmd_size = sizeof(cmd) / sizeof(cmd[0]);

/*
void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);
}
*/

int parse_command(const char *input, char *target, char *action);
void *client_handler(void *arg);

int main(void)
{
    int sockfd, new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    
    // daemonize();

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        exit(1);
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        exit(1);
    }

    sin_size = sizeof(struct sockaddr_in);

    while (1) {
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
            sleep(1);
            continue;
        }

        int *new_fd_copy = malloc(sizeof(int));
        if (new_fd_copy == NULL) {
            close(new_fd);
            continue;
        }
        *new_fd_copy = new_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)new_fd_copy) != 0) {
            syslog(LOG_ERR, "pthread_create failed");
            free(new_fd_copy);
            close(new_fd);
            continue;
        }

        pthread_detach(tid);
    }

    close(sockfd);
    closelog();

    return 0;
}

int parse_command(const char *input, char *target, char *action) {
    if (input == NULL || target == NULL || action == NULL) {
        return 0;
    }

    if (sscanf(input, "/%63[^/]/%63s", target, action) == 2) {
        return 1;
    }

    return 0;
}

void *client_handler(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);

    char buffer[BUF_SIZE] = {0};
    int numbytes;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        numbytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (numbytes > 0) {
            buffer[numbytes] = '\0';

            char target[64] = {0};
            char action[64] = {0};

            if (parse_command(buffer, target, action)) {
                printf("target: %s, action: %s\n", target, action);

                int is_handled = 0;
                for (int i = 0; i < cmd_size; i++) {
                    if (strcasecmp(target, cmd[i].command) == 0) {
                        cmd[i].action(action);
                        is_handled = 1;
                        break;
                    }
                }

                if (!is_handled) {
                    // 잘못된 명령어
                }
                
            } else {
                // 파싱 실패
            }
        } else if (numbytes == -1) {
            // 통신 실패
            break;
        }
    }

    close(client_fd);
    
    return NULL;
}