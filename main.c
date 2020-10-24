/**
 * @file main.c
 * @brief Main event loop
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */

#include "common.h"
#include "commands.h"

int main(int argc, char **argv) {
    struct event_base *evbase = event_base_new();
    if (!evbase) { return -1; }

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
