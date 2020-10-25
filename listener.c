/**
 * @file listener.c
 * @brief Creates a TCP listener and processes ACCEPT events
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */
#include "common.h"
#include "connections.h"

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *ctx);
static void accept_error_cb(struct evconnlistener *listener, void *ctx);

/** Creates a TCP listener
 * @param bind_addr Listen address in the "IP:port" form
 */
int pchat_listener_init(struct sockaddr *addr, int socklen, pchat_ctx_s *pchat_ctx) {

    pchat_ctx->listener = evconnlistener_new_bind(pchat_ctx->evbase, accept_conn_cb,
            pchat_ctx, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
            addr, socklen);

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

    if (0 != pchat_conn_new(fd, addr, socklen, ctx, PCONN_DIR_ACCEPT)) {
        fprintf(stderr, "* %s: Failed to accept connection on fd %u\n", __func__, fd);
    }
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {

        struct event_base *evb = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        fprintf(stderr, "* %s: Error %d (%s), exiting... ", __func__, err,
                evutil_socket_error_to_string(err));

        event_base_loopexit(evb, NULL);
}

