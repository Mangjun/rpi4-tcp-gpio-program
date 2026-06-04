/*******************************************************************************
 * File Name    : <core.h>
 * Description  : <서버 초기화 및 코어 동작 함수>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 *******************************************************************************/

#pragma once

/**
 * @brief   서버 데몬화
 */
void daemonize();

/**
 * @brief   서버 소켓 생성
 * @param   port    서버 포트 번호
 * @return  서버 소켓 디스크립터
 */
int init_server_socket(const int port);

/**
 * @brief   epoll을 사용하여 이벤트를 비동기적으로 처리하는 메인 루프
 * @param   tcp_sockfd  TCP 서버 소켓
 * @param   http_sockfd HTTP 서버 소켓
 */
void run(const int tcp_sockfd, const int http_sockfd);

void broadcast_sse();