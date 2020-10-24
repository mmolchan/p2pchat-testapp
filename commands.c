/**
 * @file commands.c
 * @brief Command subsystem and dispatcher of all actions in the system
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "commands.h"

/** 'Singleton' for chat commands */
static struct bufferevent *pchat_cmd_bev;

static void cmd_readcb(struct bufferevent *bev, void *ctx);
static void cmd_eventcb(struct bufferevent *bev, short events, void *ctx);

int pchat_cmd_init(struct event_base *evb) {
    pchat_cmd_bev = bufferevent_socket_new(evb, STDIN_FILENO, BEV_OPT_DEFER_CALLBACKS|BEV_OPT_CLOSE_ON_FREE);
    if (pchat_cmd_bev) {
        bufferevent_setcb(pchat_cmd_bev, cmd_readcb, NULL, cmd_eventcb, evb);
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
    char buf[256];
    int sz;
    struct evbuffer *evb_in = bufferevent_get_input(bev);

    while ((sz = evbuffer_remove(evb_in, buf, sizeof(buf))) > 0) {
        fwrite(buf, sz, 1, stdout);
    }
}

static void cmd_eventcb(struct bufferevent *bev, short events, void *ctx) {
    if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         struct event_base *evb = ctx;
         printf("Exiting...\n");
         /* Process all pending events and exit the loop */
         event_base_loopexit(evb, NULL);
    }
}

