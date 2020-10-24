/**
 * @file main.c
 * @brief Main event loop
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "common.h"
#include "commands.h"

static char *usage =
            "Usage: %s -n username\n";

static pchat_ctx_s pchat_ctx;

static void pchat_fini() {
    struct event_base *evbase = pchat_ctx.evbase;
    if (evbase) {
        pchat_cmd_fini();

        /* Run remaining events that might be triggered by 'fini' routines */
        event_base_loop(evbase, EVLOOP_ONCE|EVLOOP_NONBLOCK);
        event_base_free(evbase);
    }
    free(pchat_ctx.username);
}

static int pchat_init(int argc, char **argv) {
    int opt;
    while (-1 != (opt = getopt(argc, argv, "n:"))) {
        switch(opt) {
            case 'n':
                pchat_ctx.username = strdup(optarg);
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
        if (0 == pchat_cmd_init(&pchat_ctx)) {
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

    fprintf(stderr, "* Starting chat as user: %s\n", pchat_ctx.username);

    event_base_loop(pchat_ctx.evbase, 0);

    pchat_fini();

    return 0;
}
