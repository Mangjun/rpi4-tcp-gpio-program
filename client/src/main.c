#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "172.20.33.117"
#define SERVER_PORT 60000

void (*menu_func[])(int) = { led_menu, buzzer_menu, cds_menu, segment_menu, exit_menu };

int main (int argc, char **argv)
{
	int menu;
	int sockfd;
    struct sockaddr_in server_addr;
	sigset_t set;

	sigfillset(&set);
	sigdelset(&set, SIGINT);

	sigprocmask(SIG_SETMASK, &set, NULL);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

	while (1) {
		print_menu();	
		scanf("%d", &menu);

		if (menu < 1 || menu > 5) {
			printf("\nSelect mode only [1 | 2 | 3 | 4 | 5]\n");
			continue;
		}

		menu_func[menu - 1](sockfd);
	}

	return 0;
}