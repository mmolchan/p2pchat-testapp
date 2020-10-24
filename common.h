/**
 * @file common.h
 * @brief Including headers common for the whole app
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */
#pragma once
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

/* Net */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Libevent */
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "3rdparty/redblack.h"

/** One P2P connection */
typedef struct pchat_conn {
    char *peername;
    char *peeraddr;
    struct bufferevent *conn_bev;
} pchat_conn_s;

/** Root node for all app data and its connections */
typedef struct {
    struct event_base *evbase;
    struct evconnlistener *listener; /*< In theory multiple listeners are OK */
    struct event *sigterm_ev;
    char *username;
    struct rbtree *conn_tree;
} pchat_ctx_s;


