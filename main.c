/**
 * @file main.c
 * @brief Main event loop
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "common.h"
#include "commands.h"
#include "listener.h"
#include "connections.h"

static char *usage =
            "Usage: %s -n username -l listen_ip:port [-c connect_ip:port]\n"
            "       multiple [-c] options can be passed\n";

static pchat_ctx_s pchat_ctx;

static void pchat_sigterm_cb(int s, short ev, void *arg) {
    if (pchat_ctx.evbase) {
        event_base_loopexit(pchat_ctx.evbase, NULL);
    }
}

static void pchat_fini() {
    struct event_base *evbase = pchat_ctx.evbase;

    pchat_conntree_free(pchat_ctx.conn_tree);
    pchat_ctx.conn_tree = NULL;

    if (evbase) {
        if (pchat_ctx.sigterm_ev) {
            evsignal_del(pchat_ctx.sigterm_ev);
            event_free(pchat_ctx.sigterm_ev);
        }

        if (pchat_ctx.sigint_ev) {
            evsignal_del(pchat_ctx.sigint_ev);
            event_free(pchat_ctx.sigint_ev);
        }

        pchat_listener_fini(&pchat_ctx);
        pchat_cmd_fini();

        /* Run remaining events that might be triggered by 'fini' routines */
        event_base_loop(evbase, EVLOOP_ONCE|EVLOOP_NONBLOCK);
        event_base_free(evbase);
    }
    free(pchat_ctx.username);
}

/** Converts 'a.b.c.d:xyz' format string to sockaddr
 */
static int pchat_sockaddr_parse(const char *addr, struct sockaddr_in *sin) {
    char ip4[16] = {0};
    unsigned port;

    if (!addr || (2 != sscanf(addr, "%15[0-9\\.]:%u", ip4, &port))
              || (port == 0) || (port > 65535)) {
        fprintf(stderr, "* %s: IPv4 address '%s' format error\n", __func__, addr);
        return -1;
    }

    memset(sin, 0, sizeof(*sin));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = inet_addr(ip4);
    sin->sin_port = htons(port);
    return 0;
}

static int pchat_init(int argc, char **argv) {
    int opt;
    struct sockaddr_in bind_sin;
    struct sockaddr_in connect_sin;
    bool failed = false;

    pchat_ctx.evbase = event_base_new();
    pchat_ctx.conn_tree = pchat_conntree_new();

    if (pchat_ctx.evbase) {
        while (-1 != (opt = getopt(argc, argv, "n:l:c:"))) {
            switch(opt) {
                case 'n':
                    pchat_ctx.username = strdup(optarg);
                    break;
                case 'l':
                    if (0 != pchat_sockaddr_parse(optarg, &bind_sin)) {
                        failed = true;
                    }
                    break;
                case 'c':
                    if (0 != pchat_sockaddr_parse(optarg, &connect_sin) ||
                        0 != pchat_conn_new(-1, (struct sockaddr*)&connect_sin, sizeof(connect_sin),
                                &pchat_ctx, PCONN_DIR_CONNECT)) {
                        failed = true;
                    }
                    break;
                default:
                    break;
            }
        }

        if (failed || !pchat_ctx.username) {
            fprintf(stderr, usage, argv[0]);
            pchat_fini();
            return -1;
        }

        if (0 == pchat_listener_init((struct sockaddr*)&bind_sin, sizeof(bind_sin), &pchat_ctx) &&
            0 == pchat_cmd_init(&pchat_ctx)) {
            pchat_ctx.sigterm_ev = evsignal_new(pchat_ctx.evbase, SIGTERM, pchat_sigterm_cb, NULL);
            evsignal_add(pchat_ctx.sigterm_ev, NULL);

            pchat_ctx.sigint_ev = evsignal_new(pchat_ctx.evbase, SIGINT, pchat_sigterm_cb, NULL);
            evsignal_add(pchat_ctx.sigint_ev, NULL);
            return 0;
        }
    }

    pchat_fini();
    return -1;
}

int main(int argc, char **argv) {

    if (0 != pchat_init(argc, argv)) {
        fprintf(stderr, "* Init failed, exiting...\n");
        return -1;
    }

    fprintf(stderr, "* Starting chat as username: %s\n", pchat_ctx.username);
    fprintf(stderr, "* Write 'PeerName <message>' to send messages to PeerName\n");

    event_base_loop(pchat_ctx.evbase, 0);

    pchat_fini();

    return 0;
}
