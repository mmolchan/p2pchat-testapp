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

static char *usage =
            "Usage: %s -n username\n";

static pchat_ctx_s pchat_ctx;

static void pchat_sigterm_cb(int s, short ev, void *arg) {
    if (pchat_ctx.evbase) {
        event_base_loopexit(pchat_ctx.evbase, NULL);
    }
}

static void pchat_fini() {
    struct event_base *evbase = pchat_ctx.evbase;
    if (evbase) {
        if (pchat_ctx.sigterm_ev) {
            evsignal_del(pchat_ctx.sigterm_ev);
            event_free(pchat_ctx.sigterm_ev);
        }

        pchat_listener_fini(&pchat_ctx);
        pchat_cmd_fini();

        /* Run remaining events that might be triggered by 'fini' routines */
        event_base_loop(evbase, EVLOOP_ONCE|EVLOOP_NONBLOCK);
        event_base_free(evbase);
    }
    free(pchat_ctx.username);
}

static int pchat_init(int argc, char **argv) {
    int opt;
    const char *bind_addr = NULL;

    while (-1 != (opt = getopt(argc, argv, "n:l:"))) {
        switch(opt) {
            case 'n':
                pchat_ctx.username = strdup(optarg);
                break;
            case 'l':
                bind_addr = strdup(optarg);
                break;
            default:
                break;
        }
    }

    if (!pchat_ctx.username) {
        fprintf(stderr, usage, argv[0]);
        return -1;
    }

    pchat_ctx.evbase = event_base_new();
    if (pchat_ctx.evbase) {
        if (0 == pchat_listener_init(bind_addr, &pchat_ctx) &&
            0 == pchat_cmd_init(&pchat_ctx)) {
            pchat_ctx.sigterm_ev = evsignal_new(pchat_ctx.evbase, SIGTERM, pchat_sigterm_cb, NULL);
            evsignal_add(pchat_ctx.sigterm_ev, NULL);
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

    fprintf(stderr, "* Starting chat with username: %s\n", pchat_ctx.username);

    event_base_loop(pchat_ctx.evbase, 0);

    pchat_fini();

    return 0;
}
