/**
 * @file commands.c
 * @brief Command subsystem and dispatcher of all actions in the system
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "common.h"

/** 'Singleton' for chat commands */
static struct bufferevent *pchat_cmd_bev;

static void cmd_readcb(struct bufferevent *bev, void *ctx);
static void cmd_eventcb(struct bufferevent *bev, short events, void *ctx);

int pchat_cmd_init(pchat_ctx_s *pchat_ctx) {
    struct event_base *evb = pchat_ctx->evbase;
    pchat_cmd_bev = bufferevent_socket_new(evb, STDIN_FILENO, BEV_OPT_DEFER_CALLBACKS|BEV_OPT_CLOSE_ON_FREE);
    if (pchat_cmd_bev) {
        bufferevent_setcb(pchat_cmd_bev, cmd_readcb, NULL, cmd_eventcb, pchat_ctx);
        bufferevent_enable(pchat_cmd_bev, EV_READ|EV_WRITE);
        return 0;
    }
    return -1;
}

void pchat_cmd_fini() {
    if (pchat_cmd_bev) {
        bufferevent_free(pchat_cmd_bev);
        pchat_cmd_bev = NULL;
    }
}

static void cmd_readcb(struct bufferevent *bev, void *ctx) {
    pchat_ctx_s *pchat_ctx = ctx;
    char  *in_cmd;
    size_t readsz = 0;
    char peername[64] = {0};
    struct evbuffer *evb_in = bufferevent_get_input(bev);
    struct rbnode *rbnode;
    pchat_conn_s sconn = {0}; /* Search tree by comparing to this conn */
    pchat_conn_s *peerconn = NULL;

    do {
        in_cmd = evbuffer_readln(evb_in, &readsz, EVBUFFER_EOL_CRLF);
        if (in_cmd && 1 == sscanf(in_cmd, "%32s", peername)) {
            sconn.peername = &peername[0];
            rbnode = rbfind(pchat_ctx->conn_tree, &sconn);
            if (!rbnode) {
                fprintf(stderr, "* No peer '%s' found...\n", peername);
                free(in_cmd);
                continue;
            }
            peerconn = rbnode->data;
            if (readsz > strlen(peername) + 1) {
                struct evbuffer *evb_out = bufferevent_get_output(peerconn->bev);
                evbuffer_add_printf(evb_out, "%s\n", &in_cmd[strlen(peername) + 1]);
            }
            free(in_cmd);
        }
    } while (in_cmd);
}

static void cmd_eventcb(struct bufferevent *bev, short events, void *ctx) {
    pchat_ctx_s *pchat_ctx = ctx;
    if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         fprintf(stderr, "* Exiting...\n");
         /* Process all pending events and exit the loop */
         event_base_loopexit(pchat_ctx->evbase, NULL);
    }
}

