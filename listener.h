#pragma once

int pchat_listener_init(struct sockaddr *addr, int socklen, pchat_ctx_s *pchat_ctx);
void pchat_listener_fini(pchat_ctx_s *pchat_ctx);

