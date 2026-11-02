#include "mongoose.h"
#include <stdio.h>
#include <string.h>
#include"cert.h"
static int g_sent = 0, g_done = 0;
static int g_connected = 0;
// 注意：结尾必须有 '\n'，匹配服务端的行分帧
static const char *g_msg = "你好世界,Hello,world\n";

static void on_client(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_CONNECT) {
    // 测试用：跳过证书验证。生产应改为 .ca = mg_str(CA_CERT)
    struct mg_tls_opts opts = {
         .ca   = mg_str_n((char *) server_crt, server_crt_len),
        //.name = mg_str("localhost"),
    };
    mg_tls_init(c, &opts);
  } else if (ev == MG_EV_TLS_HS) {
    // TLS 握手完成，发送一次；不要立刻 is_draining
    if (!g_sent) {
      g_sent = 1;
      size_t len = strlen(g_msg);
      if (mg_send(c, g_msg, len)) {
        printf("sent %zu bytes\n", len);
      } else {
        printf("send failed\n");
        g_done = 1;
      }
    }
  } else if (ev == MG_EV_READ) {
    // 数据在 c->recv，按 '\n' 判断完整回复
    size_t start = 0;
    for (size_t i = 0; i < c->recv.len; i++) {
      if (c->recv.buf[i] == '\n') {
        size_t len = i - start;
        printf("server reply: %.*s\n", (int) len,
               (char *) c->recv.buf + start);
        start = i + 1;
        c->is_draining = true;  // 收到完整回复后再关闭
      }
    }
    if (start > 0) mg_iobuf_del(&c->recv, 0, start);
  } else if (ev == MG_EV_ERROR) {
    printf("error: %s\n", (char *) ev_data);
    g_done = 1;
  } else if (ev == MG_EV_CLOSE) {
    printf("closed\n");
    g_done = 1;
  }
}
// SNTP 同步完成后的回调
static void on_sntp(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_SNTP_TIME) {
    uint64_t epoch_ms = *(uint64_t *) ev_data;
    printf("SNTP synced: %llu ms since epoch\n",
           (unsigned long long) epoch_ms);
    // 时间已同步，关闭这个临时 SNTP 连接
    c->is_closing = true;

    // 现在再发起 TLS 连接
    if (!g_connected) {
      g_connected = 1;
      struct mg_connection *tls =
          mg_connect(c->mgr, "tls://127.0.0.1:12345", on_client, NULL);
      if (tls == NULL) {
        printf("mg_connect failed\n");
      }
    }
  } else if (ev == MG_EV_ERROR) {
    printf("SNTP error: %s\n", (char *) ev_data);
    c->is_closing = true;
  }
}
int main(void) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  // 使用国内 NTP 服务器，DNS 也换成国内公共 DNS 更稳
  mgr.dns4.url = "udp://223.5.5.5:53";  // 阿里云 DNS

  // 先发起 SNTP 同步
  struct mg_connection *sntp = mg_sntp_connect(
      &mgr, "udp://1.cn.pool.ntp.org:123", on_sntp, NULL);
  if (sntp == NULL) {
    printf("mg_sntp_connect failed\n");
    mg_mgr_free(&mgr);
    return 1;
  }

  while (!g_done) mg_mgr_poll(&mgr, 200);
  mg_mgr_free(&mgr);
  return 0;
}