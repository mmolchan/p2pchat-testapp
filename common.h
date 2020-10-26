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

/* Settings and constants */
static const unsigned CONNECT_TMOUT = 10;
#define PROTO_GREETING_HDR "PCHAT 1.0 Username:%s"

/** P2P connection direction */
typedef enum {
    PCONN_DIR_CONNECT
   ,PCONN_DIR_ACCEPT
} pchat_conn_dir_t;

typedef enum {
    PCONN_ACCEPTED
   ,PCONN_INITIATED
   ,PCONN_ESTABLISHED /**< Can send messages, last stage */
} pchat_conn_state_t;

/** One P2P connection */
typedef struct pchat_conn {
    struct pchat_ctx *pchat_ctx;
    char *peername;
    struct bufferevent *bev;
    struct evbuffer *evb_in; /**< Accumulator for the partial network msgs */
    pchat_conn_state_t state;
} pchat_conn_s;

/** Root node for all app data and its connections */
typedef struct pchat_ctx {
    struct event_base *evbase;
    struct evconnlistener *listener; /**< In theory multiple listeners are OK */
    struct event *sigterm_ev;
    struct event *sigint_ev;
    char *username;
    struct rbtree *conn_tree;        /**< Connections rbtree organized by uniaue peername */
} pchat_ctx_s;

//#define DEBUG
#ifdef DEBUG
# define LOG_DBG(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define LOG_DBG(fmt, ...) while(0){}
#endif /* DEBUG */


