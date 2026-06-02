/*******************************************************************************
 * File Name    : <setup.h>
 * Description  : <클라이언트 설정 함수 선언부>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   signal mask 설정
 * @details SIGINT 빼고 나머지 signal 블록킹
 */
void setup_signal();

/**
 * @brief   서버 소켓 설정
 * @param   sockfd  서버 소켓 담을 공간
 * @param   ip      서버 IP
 * @param   port    서버 port
 */
int setup_server_socket(int* const sockfd, const char* ip, const char* port);