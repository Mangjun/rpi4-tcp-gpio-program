/*******************************************************************************
 * File Name    : <main.c>
 * Description  : <리눅스 클라이언트>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 * - <2026.06.02> : signal 마스크 설정 및 소켓 설정 setup.c로 분할, ip주소와 port번호 사용자에게 입력 받도록 수정 (<김명준>)
 * - <2026.06.03> : select 적용된 input_num으로 메뉴 입력받도록 수정 (<김명준>)
 *******************************************************************************/

#include "menu.h"
#include "setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void (*menu_func[])(int) = { led_menu, buzzer_menu, cds_menu, segment_menu, exit_menu };

int main (int argc, char **argv)
{
	int sockfd;

	if (argc != 3) {
		printf("usage: %s server_ip server_port\n", argv[0]);
		exit(1);
	}

	setup_signal();
	if (setup_server_socket(&sockfd, argv[1], argv[2])) {
		printf("Can't connect server...\n");
		exit(1);
	}
    
	while (1) {
		int menu;

		print_menu();
		menu = input_num(sockfd, "Input > ");

		if (menu < 1 || menu > 5) {
			printf("\nSelect menu only [ 1 | 2 | 3 | 4 | 5 ]\n");
			continue;
		}

		menu_func[menu - 1](sockfd);
	}

	return 0;
}