#pragma once

void daemonize();
int init_server_socket(const int port);
void run(const int tcp_sockfd, const int http_sockfd);