/**
 * @file connections.c
 * @brief Establishes and accepts P2P connections and handles their events
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "common.h"

static void conn_readcb(struct bufferevent *bev, void *ctx);
static void conn_eventcb(struct bufferevent *bev, short events, void *ctx);

void pchat_conn_free(pchat_conn_s *pconn) {
    if (pconn) {
        struct bufferevent *bev = pconn->bev;
        if (bev) {
            bufferevent_disable(bev, EV_READ|EV_WRITE|EV_TIMEOUT);
            bufferevent_setcb(bev, NULL, NULL, NULL, NULL);
            bufferevent_free(bev);
        }
        if(pconn->evb_in) {
            evbuffer_free(pconn->evb_in);
        }
        /* XXX delete from trees, if any? */
        free(pconn->peername);
        free(pconn);
    }
}

/** Initiate a new P2P connection or accept it if received
 */
int pchat_conn_new(evutil_socket_t fd, struct sockaddr *addr, int socklen,
        void *ctx, pchat_conn_dir_t dir) {
    pchat_ctx_s *pchat_ctx = ctx;
    struct event_base *evb = pchat_ctx->evbase;
    //struct timeval tmout = { .tv_sec = CONNECT_TMOUT, .tv_usec = 0 };

    pchat_conn_s *pconn = calloc(1, sizeof(*pconn));
    if (pconn) {
        pconn->pchat_ctx = pchat_ctx; /* For upref if needed */
        pconn->evb_in = evbuffer_new();
        pconn->bev = bufferevent_socket_new(evb, fd,
                BEV_OPT_DEFER_CALLBACKS|BEV_OPT_CLOSE_ON_FREE);
        if (pconn->evb_in && pconn->bev) {
            pconn->state = (dir == PCONN_DIR_CONNECT ? PCONN_INITIATED : PCONN_ACCEPTED);

            bufferevent_setcb(pconn->bev, conn_readcb, NULL, conn_eventcb, pconn);
            // XXX connect only bufferevent_set_timeouts(pconn->bev, &tmout, NULL);
            bufferevent_enable(pconn->bev, EV_READ|EV_TIMEOUT);
            /* XXX insert into tree by bev? */
            struct evbuffer *evb_out = bufferevent_get_output(pconn->bev);
            evbuffer_add_printf(evb_out, PROTO_GREETING_HDR "\n", pchat_ctx->username);
            LOG_DBG("%s: sent greeting with '%s' username\n", __func__, pchat_ctx->username);
            return 0;
        }
        pchat_conn_free(pconn);
    }
    return -1;
}

static void conn_readcb(struct bufferevent *bev, void *ctx) {
    pchat_conn_s *pconn = ctx;
    struct evbuffer *evb_in = bufferevent_get_input(bev);
    size_t readsz = 0;
    char  *in_msg;
    char username[64] = {0};

    /* Concatenate for previously read partial data */
    evbuffer_remove_buffer(evb_in, pconn->evb_in, evbuffer_get_length(evb_in));

    do {
        in_msg = evbuffer_readln(pconn->evb_in, &readsz, EVBUFFER_EOL_CRLF);
        if (in_msg) {
            LOG_DBG("%s(%u): msg received is: '%s'\n", __func__, pconn->state, in_msg);
            if (pconn->state != PCONN_ESTABLISHED) {
                if (readsz < sizeof(username)) {
                    if (1 == sscanf(in_msg, PROTO_GREETING_HDR, username) &&
                            strlen(username)) {
                        LOG_DBG("%s(%u): username received is: '%s'\n", __func__, pconn->state, username);
                        pconn->state = PCONN_ESTABLISHED;
                        pconn->peername = strdup(username);
                        continue;
                    }
                }
                bufferevent_trigger_event(bev, BEV_EVENT_ERROR, BEV_TRIG_DEFER_CALLBACKS);
                return;
            } else {
                /* For established conns we just print a 'username: msg' into the stdout */
                fprintf(stdout, "%s: '%s'\n", pconn->peername, in_msg);
            }
        }
    } while (in_msg);
}

static void conn_eventcb(struct bufferevent *bev, short events, void *ctx) {
    if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
        pchat_conn_free(ctx);
        return;
    }
}


