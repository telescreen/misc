#include "net.h"
#include <ev.h>

#include <set>
#include <iostream>

#include <stdarg.h>
#include <string.h>
#include <getopt.h>

#define forall(it, c) for(typeof((c).begin()) it = (c).begin(); it != (c).end(); ++it)

/* This program name */
const char *program_name = "ktmtbot";


static const char* irc_server = "irc.freenode.com";
static const char* port = "6667";
static const char* nick = "ktmtbot";
static const char* channel = "#ktmt.github";

static const char* local_addr = "127.0.0.1";
static const char* local_port = "12345";


static std::set<int> localfds;
static int sockfd;

static struct option long_options[] = {
        { "front", 1, NULL, 'f'},
        { "port", 1, NULL, 'p'},
        { "back", 1, NULL, 'b'},
        { "listen", 1, NULL, 'l'},
        { "mode", 1, NULL, 'm' },
        { "nick", 1, NULL, 'n' },
        { "channel", 1, NULL, 'c'},
        { "help", 0, NULL, 'h' },
        { NULL, 0, NULL, 0 }
};
static const char* short_options = "f:p:b:l:m:n:c:h";

/* prototypes */
static void client_callback(struct ev_loop *loop, ev_io *w, int revents);
static void accept_callback(struct ev_loop *loop, ev_io *w, int revents);

struct GlobalInfo
{
        int mode;
        char *front;
        char *front_port;
        char *back;
        char *back_port;
        char *nick;
        char *channel;
};
static struct GlobalInfo *info;
#define FORWARD_MODE 0
#define PROCESS_MODE 1

class Msg 
{
public:
        enum MsgType { JOIN, PING, PRIVMSG, QUIT, UNKNOWN };
        Msg(): type(UNKNOWN), sender(NULL), receiver(NULL), content(NULL){}
        //Msg(MsgType type, char *sender, char *receiver, char *content);
        virtual ~Msg() {};

public:
        MsgType type;
        char *sender;
        char *receiver;
        char *content;
private:
        Msg(Msg const&);
        void operator=(Msg const&);
};

void print_usage(FILE *stream, int exit_code)
{
        fprintf(stream, "Usage: %s options\n", program_name);
        fprintf(stream, "   -f\t--front\t front server\n"
                        "   -p\t--port\t front server's port\n"
                        "   -b\t--back\t back server\n"
                        "   -l\t--listen\t back server listening port\n"
                        "   -m\t--mode\t process message or forward mode\n"
                        "   -n\t--nick\t bot nickname\n"
                        "   -c\t--channel\t channel to join\n"
                        "   -h\t--help\t Print this help\n");
        exit(exit_code);
}


void freeglobalinfo(struct GlobalInfo *info)
{
        free(info->front);
        free(info->front_port);
        free(info->back);
        free(info->back_port);
        free(info->nick);
        free(info->channel);
        free(info);
}

struct GlobalInfo *parseglobalinfo(int argc, char *argv[])
{
        struct GlobalInfo *info = (struct GlobalInfo*)calloc(1, sizeof(struct GlobalInfo));
        int next_option;
        if (info == NULL) {
                fprintf(stderr, "Not enough memory\n");
                return NULL;
        }
        do {
                next_option = getopt_long(argc, argv, short_options, long_options, 0);
                switch(next_option) {
                case 'h':
                        print_usage(stdout, 0);
                case 'f':
                        info->front = strdup(optarg);
                        break;
                case 'p':
                        info->front_port = strdup(optarg);
                        break;
                case 'b':
                        info->back = strdup(optarg);
                        break;
                case 'l':
                        info->back_port = strdup(optarg);
                        break;
                case 'n':
                        info->nick = strdup(optarg);
                        break;
                case 'c':
                        info->channel = strdup(optarg);
                        break;
                /* 2 kinds of mode: forward (f) and process (p) */
                case 'm':
                {
                        char mode = *(char*)optarg;
                        if (mode == 'p') {
                                printf("mode = %c\n", mode);
                                info->mode |= PROCESS_MODE;
                        }
                        else
                                info->mode |= FORWARD_MODE;
                        break;
                }
                case '?':
                        print_usage(stderr, -1);
                        break;
                default:
                        if (info->front == NULL) 
                                info->front = strdup(irc_server);
                        if (info->front_port == NULL) 
                                info->front_port = strdup(port);
                        if (info->back == NULL) 
                                info->back = strdup(local_addr);
                        if (info->back_port == NULL)
                                info->back_port = strdup(local_port);
                        if (info->nick == NULL)
                                info->nick = strdup(nick);
                        if (info->channel == NULL)
                                info->channel = strdup(channel);
                        break;

                }
        } while(next_option != -1);
        return info;
}

// 1024KB
#define MAXLENGTH 1024 * 1024

static void send_message(int sockfd, const char *message, ...)
{
        size_t sbyte;
        char buffer[MAXLENGTH];
        va_list argptr;

        va_start(argptr, message);
        vsnprintf(buffer, MAXLENGTH, message, argptr);
        va_end(argptr);

        // send connect data
        printf(">>> %s", buffer);
        sbyte = strlen(buffer);
        send_data(sockfd, buffer, &sbyte);
}

/* IRC Talks */
static void pong(int sockfd, const char *buffer)
{
        char pongbuf[MAXLENGTH];
        strcpy(pongbuf, "PONG ");
        strncat(pongbuf, buffer, strlen(buffer));
        strcat(pongbuf, "\r\n");
        send_message(sockfd, pongbuf);
}

static void privmsg(int sockfd, const char *buffer)
{
        char buf[MAXLENGTH];
        strcpy(buf, "PRIVMSG ");
        strncat(buf, info->channel, strlen(info->channel));
        strncat(buf, " :", 2);
        strncat(buf, buffer, strlen(buffer));
        strcat(buf, "\r\n");
        send_message(sockfd, buf);
}

static void hand_shake(struct ev_loop *loop, ev_io *w, int revents)
{
        send_message(w->fd, "NICK %s\r\nUSER %s 0 * :try to be a smart bot\r\nJOIN %s\r\n", 
                     info->nick, info->nick, info->channel);
        ev_io_stop(loop, w);
}


Msg* parse_message(char *buf)
{
        char *p;
        char *saveptr;
        Msg *msg = new Msg;
        if (msg == NULL) {
                fprintf(stderr, "Not enough memory\n");
                goto done;
        }
        p = strtok_r(buf, ": ", &saveptr);
        if (!p) goto done;
        if (strcmp(p, "PING") == 0) {
                msg->type = Msg::PING;
                p = strtok_r(NULL, ": ", &saveptr);
                msg->sender = strdup(p);
                goto done;
        }
        msg->sender = strdup(p);
        
        // sender
        p = strtok_r(NULL, ": ", &saveptr);
        if (!p) goto done;
        if (strncmp(p, "QUIT", 4) == 0)
                msg->type = Msg::QUIT;
        else if (strncmp(p, "PRIVMSG", 7) == 0)
                msg->type = Msg::PRIVMSG;
        else if (strncmp(p, "JOIN", 4) == 0)
                msg->type = Msg::JOIN;
        else 
                msg->type = Msg::UNKNOWN;

        p = strtok_r(NULL, ": ", &saveptr);
        if (!p) goto done;
        msg->receiver = strdup(p);

        p = strtok_r(NULL, ":", &saveptr);
        if (!p) goto done;
        msg->content = strdup(p);
done: 
        return msg;
}


static void process_message(struct ev_loop *loop, ev_io *w, int revents)
{
        Msg* msg;
        size_t byte;
        char buffer[MAXLENGTH];
        char *p, *saveptr;
        
        //char *ptr, *buf;

        byte = recv_data(w->fd, buffer, MAXLENGTH);
        if (byte == 0) {
                ev_io_stop(loop, w);
                ev_break(loop, EVBREAK_ALL);
        }
        buffer[byte] = '\0';
        printf("%s", buffer);

        p = strtok_r(buffer, "\r\n", &saveptr);
        while (p != NULL) {
                msg = parse_message(p);
                if (!msg) {
                        fprintf(stderr, "Not enough memory to process: %s\n", buffer);
                        break;
                }

                // Receive PING, need to pong back
                // printf("%d\n", msg->type);
                switch(msg->type) {
                case Msg::PING:
                        pong(w->fd, msg->sender);
                        break;
                case Msg::QUIT:
                        fprintf(stdout, ">> %s quits. Bye bye\n", msg->sender);
                        break;
                case Msg::PRIVMSG:
                        forall(it, localfds) {
                                size_t buflen = strlen(msg->content);
                                int nbytes = send_data(*it, msg->content, &buflen);
                                fprintf(stdout, "%d bytes left after sending buffer to %d\n", nbytes, *it);
                        }
                        break;
                case Msg::JOIN:
                        /*
                        buf = (char*)calloc(1, 100);
                        ptr = strchr(msg->sender, '!');
                        strncpy(buf, "Hi ", 3);
                        strncat(buf, msg->sender, ptr - msg->sender);
                        privmsg(w->fd, buf);
                        free(buf);
                        */
                default:
                        break;
                }

                delete msg;
                p = strtok_r(NULL, "\r\n", &saveptr);
        }
}

/* Backend callbacks */
static void accept_callback(struct ev_loop *loop, ev_io *w, int revents)
{
        struct sockaddr_storage clientaddr;
        socklen_t sin_size = sizeof(struct sockaddr);
        int clientfd;
        struct ev_io *client_watcher = (struct ev_io*)malloc(sizeof(struct ev_io));
        char ipstr[INET6_ADDRSTRLEN];

        if (revents & EV_ERROR) {
                perror("Got invalid client");
                return;
        }
        
        clientfd = accept(w->fd, (struct sockaddr*)&clientaddr, &sin_size);
        if (clientfd < 0) {
                perror("accept");
                return;
        }

        inet_ntop(clientaddr.ss_family, get_addr((struct sockaddr*)&clientaddr), ipstr, sizeof(ipstr));
        fprintf(stdout, "Got access from %s\n", ipstr);
        localfds.insert(clientfd);

        ev_io_init(client_watcher, client_callback, clientfd, EV_READ);
        ev_io_start(loop, client_watcher);
}

static void client_callback(struct ev_loop *loop, ev_io *w, int revents)
{
        char buffer[MAXLENGTH];
        int nbytes;

        if ((nbytes = recv_data(w->fd, buffer,  MAXLENGTH)) <= 0) {
                if (nbytes == 0) {
                        fprintf(stdout, "Peer %d closed connection\n", w->fd);
                        close(w->fd);
                }
                localfds.erase(localfds.find(w->fd));
                ev_io_stop(loop, w);
                free(w);
        } else {
                buffer[nbytes] = 0;
                fprintf(stdout, "Received from client %s\n", buffer);
                printf("mode = %c\n", info->mode);
                if (info->mode & PROCESS_MODE) {
                        char *p = strtok(buffer, "\n");
                        while (p != NULL) {
                            privmsg(sockfd, p);
                            p = strtok(NULL, "\n");
                        }
                }
        }
}


int main(int argc, char **argv)
{
        struct ev_loop *loop = ev_default_loop(0);
        ev_io fd_read_watcher, fd_write_watcher;
        ev_io backend_watcher;
        int servfd;

        info = parseglobalinfo(argc, argv);
        if (info == NULL)
                exit(EXIT_FAILURE);


        sockfd = connect_to_server(info->front, info->front_port);
        if (sockfd == -1) {
		fprintf(stderr, "Error when making connection\n");
		exit(EXIT_FAILURE);
        }

        servfd = netbind(info->back, info->back_port);
        if (servfd == -1) {
                close(sockfd);
		exit(EXIT_FAILURE);
        }

        ev_init(&backend_watcher, accept_callback);
        ev_io_set(&backend_watcher, servfd, EV_READ);
        ev_io_start(loop, &backend_watcher);

        ev_init(&fd_read_watcher, process_message);
        ev_init(&fd_write_watcher, hand_shake);
        ev_io_set(&fd_read_watcher, sockfd, EV_READ);
        ev_io_set(&fd_write_watcher, sockfd, EV_WRITE);        
        ev_io_start(loop, &fd_read_watcher);
        ev_io_start(loop, &fd_write_watcher);
       
        ev_run(loop, 0);

        freeglobalinfo(info);

        return EXIT_SUCCESS;
}
