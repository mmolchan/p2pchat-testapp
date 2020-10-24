/**
 * @file listener.c
 * @brief Creates a TCP listener and processes ACCEPT events
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */
#include "common.h"

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *ctx);
static void accept_error_cb(struct evconnlistener *listener, void *ctx);

/** Creates a TCP listener
 * @param bind_addr Listen address in the "IP:port" form
 */
int pchat_listener_init(const char *bind_addr, pchat_ctx_s *pchat_ctx) {
    char ip4[16] = {0};
    unsigned port;
    struct sockaddr_in sin;

    if (!bind_addr || (2 != sscanf(bind_addr, "%15[0-9\\.]:%u", ip4, &port)) || (port > 65535)) {
        fprintf(stderr, "* %s: Bind address '%s' format error\n", __func__, bind_addr);
        return -1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip4);
    sin.sin_port = htons(port);

    pchat_ctx->listener = evconnlistener_new_bind(pchat_ctx->evbase, accept_conn_cb,
            pchat_ctx, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
            (struct sockaddr*)&sin, sizeof(sin));

    if (!pchat_ctx->listener) {
        fprintf(stderr, "* %s: Listener creation failed\n", __func__);
        return -1;
    }
    evconnlistener_set_error_cb(pchat_ctx->listener, accept_error_cb);
    return 0;
}

void pchat_listener_fini(pchat_ctx_s *pchat_ctx) {
    if (pchat_ctx->listener) {
        evconnlistener_free(pchat_ctx->listener);
        pchat_ctx->listener = NULL;
    }
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *addr, int socklen, void *ctx) {

    /* TODO add new TCP connection to the tree */
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {

        struct event_base *evb = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        fprintf(stderr, "* %s: Error %d (%s), exiting... ", __func__, err,
                evutil_socket_error_to_string(err));

        event_base_loopexit(evb, NULL);
}

