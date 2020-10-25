#pragma once

int pchat_conn_new(evutil_socket_t fd, struct sockaddr *addr, int socklen,
        void *ctx, pchat_conn_dir_t dir);
