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

int main(int argc, char **argv) {
    char *username = NULL;
    struct event_base *evbase = NULL;
    int opt;

    while (-1 != (opt = getopt(argc, argv, "n:"))) {
        switch(opt) {
            case 'n':
                username = strdup(optarg);
                break;
            default:
                break;
        }
    }

    if (!username) {
        fprintf(stderr, usage, argv[0]);
        return -1;
    }
    fprintf(stderr, "* Starting chat as user: %s\n", username);

    evbase = event_base_new();
    if (!evbase) {
        return -1;
    }

    if (0 != pchat_cmd_init(evbase)) {
        event_base_free(evbase);
        return -1;
    }

    event_base_loop(evbase, 0);

    pchat_cmd_fini();

    /* Run remaining events that might be triggered by 'fini' routines */
    event_base_loop(evbase, EVLOOP_ONCE|EVLOOP_NONBLOCK);
    event_base_free(evbase);

    return 0;
}
