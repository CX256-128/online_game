#include "mongoose.h"
#include <stdio.h>
#include <string.h>
#include"cert.h"
#include"key.h"
static void on_server(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_ACCEPT) {
    // TLS 服务端：必须提供 cert + key
    struct mg_tls_opts opts = {
        .cert = mg_str_n((char *) server_crt, server_crt_len),
        .key  = mg_str_n((char *) server_key, server_key_len),
    };
    mg_tls_init(c, &opts);
    printf("accepted, starting TLS handshake\n");
  } else if (ev == MG_EV_TLS_HS) {
    printf("TLS handshake done\n");
  } else if (ev == MG_EV_READ) {
    // TLS 解密后的明文已经在 c->recv 里，处理方式和普通 TCP 完全一样
    size_t start = 0;
    for (size_t i = 0; i < c->recv.len; i++) {
      if (c->recv.buf[i] == '\n') {
        size_t len = i - start;
        printf("got line (%zu bytes): %.*s\n", len, (int) len,
               (char *) c->recv.buf + start);

        char reply[64];
        int n = snprintf(reply, sizeof(reply), "RECEIVED,%zu\n", len);
        mg_send(c, reply, (size_t) n);

        start = i + 1;
      }
    }
    if (start > 0) mg_iobuf_del(&c->recv, 0, start);
  } else if (ev == MG_EV_ERROR) {
    printf("error: %s\n", (char *) ev_data);
  } else if (ev == MG_EV_CLOSE) {
    printf("closed\n");
  }
}

int main(void) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  // 裸 TLS：用 tls://，不是 https://
  struct mg_connection *c =
      mg_listen(&mgr, "tls://127.0.0.1:12345", on_server, NULL);
  if (c == NULL) {
    printf("listen failed\n");
    mg_mgr_free(&mgr);
    return 1;
  }

  for (;;) mg_mgr_poll(&mgr, 200);
  mg_mgr_free(&mgr);
  return 0;
}