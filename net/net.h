#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>


#define PORT "12345"
#define BACKLOG 10

void *get_addr(const struct sockaddr* sa);

int connect_to_server(const char* server, const char *port);

int netbind(const char *addr, const char *port);

int send_data(int sockfd, const char* buffer, size_t *len);

ssize_t recv_data(int sockfd, char *buffer, size_t buf_len);

#endif 
