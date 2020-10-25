#pragma once

int pchat_conn_new(evutil_socket_t fd, struct sockaddr *addr, int socklen,
        pchat_ctx_s *pchat_ctx, pchat_conn_dir_t dir);
struct rbtree *pchat_conntree_new();
void pchat_conntree_free(struct rbtree *conntree);

