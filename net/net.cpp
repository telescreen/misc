#include "net.h"

void *get_addr(const struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int connect_to_server(const char* server, const char *port)
{
    int sockfd;
    int status;
    struct addrinfo hints, *p, *res;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // Get address struct
    if ((status = getaddrinfo(server, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(status));
        return -1;
    }

    for(p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        return -1;
    }

    inet_ntop(p->ai_family, get_addr((struct sockaddr*)p->ai_addr),
            ipstr, sizeof ipstr);
    printf("Connecting to: %s\n", ipstr);
    freeaddrinfo(res);

    return sockfd;
}

int send_data(int sockfd, const char* buffer, size_t *len)
{
    size_t total = 0;
    size_t bytesleft = *len;
    int n;

    while (total < *len) {
        n = send(sockfd, buffer + total, bytesleft, 0);
        if (n == -1) break;
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
}

ssize_t recv_data(int sockfd, char *buffer, size_t buf_len)
{
    socklen_t sockaddr_len = sizeof(struct sockaddr_storage);
    ssize_t numbytes;
    //
    numbytes = recvfrom(sockfd, buffer, buf_len, 0, NULL, &sockaddr_len);
    if (numbytes <= 0) {
        // Peer close connection
        if (numbytes == 0) {
            return 0;
        } else {
            perror("recvfrom");
            return -1;
        }
    }
    return numbytes;
}

// bind and listen to a hostname or port
int netbind(const char *addr, const char *port)
{
    // socket file description 
    int sockfd;
    int status;
    struct addrinfo *p, hints, *res;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(addr, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
        
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(int)) == -1) {
            perror("setsockopt");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind server\n");
        return -1;
    }

    freeaddrinfo(res);

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(-1);
    }
            
    return sockfd;
}

