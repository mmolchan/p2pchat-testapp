/**
 * @file common.h
 * @brief Including headers common for the whole app
 * @author Maxim Molchanov <i.censo[AT]gmail.com>
 *
 * 
 */
#pragma once
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

typedef struct {
    struct event_base *evbase;
    char *username;
} pchat_ctx_s;

