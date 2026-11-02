#include "network.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

/* ---------- 协议结构 ---------- */
#pragma pack(push, 1)
typedef struct {
    char     magic[8];   /* "MG_DISC\0" 或 "MG_RESP\0" */
    char     name[256];  /* 玩家名 */
    uint16_t tcp_port;   /* 网络字节序 */
} NetPacket;
#pragma pack(pop)

#define MAGIC_DISC "MG_DISC"
#define MAGIC_RESP "MG_RESP"

/* ---------- 全局状态 ---------- */
/* 用独立名字避免与 mongoose.h 末尾的 extern g_mgr 冲突 */
static struct mg_mgr          s_mgr;
static bool                   s_mgr_inited = false;
static bool                   s_running    = true;

static struct mg_connection*  s_udp_listen = NULL;
static struct mg_connection*  s_tcp_listen = NULL;
static struct mg_connection*  s_bcast_conn = NULL;
static bool                   s_bcast_sent = false;

static ip_to_player_index_map* s_scan_map    = NULL;
static char                    s_my_name[256] = {0};
static uint16_t                s_my_tcp_port  = TCP_SERVER_PORT;

/* ============================================================
 * 工具
 * ============================================================ */

static void ensure_mgr(void)
{
    if (!s_mgr_inited) {
        mg_mgr_init(&s_mgr);
        s_mgr_inited = true;
    }
}

/* 从 c->rem (struct mg_addr) 提取可读 IP 字符串。
 * 依据 mongoose.h 7.23：struct mg_addr { union { uint8_t ip[16]; ... } addr;
 *                                     uint16_t port; uint8_t scope_id; bool is_ip6; } */
static void extract_peer_addr(struct mg_connection* c, char* ip_out,
                              size_t cap, bool* is_v6_out)
{
    ip_out[0]  = '\0';
    *is_v6_out = false;
    if (!c) return;

    struct mg_addr* a = &c->rem;
    *is_v6_out = a->is_ip6;

    if (a->is_ip6) {
        if (!inet_ntop(AF_INET6, a->addr.ip, ip_out, (socklen_t)cap)) {
            ip_out[0] = '\0';
            return;
        }
        /* 若是链路本地地址，追加 %scope_id（fe80::1%2 形式） */
        if (a->scope_id != 0 && strchr(ip_out, '%') == NULL) {
            size_t used = strlen(ip_out);
            if (used + 16 < cap) {
                snprintf(ip_out + used, cap - used, "%%%u",
                         (unsigned)a->scope_id);
            }
        }
    } else {
        /* IPv4 存储在 ip[16] 前 4 字节（头文件注释明确说明） */
        if (!inet_ntop(AF_INET, a->addr.ip, ip_out, (socklen_t)cap)) {
            ip_out[0] = '\0';
        }
    }
}

int FindPlayerIndex(package* pack, const char* name)
{
    if (!pack || !name) return -1;
    for (int i = 0; i < pack->active_player_num; i++) {
        if (strcmp(pack->_player_name[i], name) == 0) return i;
    }
    return -1;
}

static void add_peer(ip_to_player_index_map* map, const char* name,
                     const char* ip, bool is_v6, uint16_t tcp_port)
{
    if (!map || !name || !ip || ip[0] == '\0') return;
    if (map->active_player_num >= MAX_PLAYERS) return;
    if (s_my_name[0] && strcmp(name, s_my_name) == 0) return;

    /* 去重：同 IP + 同端口视为同一节点 */
    for (int i = 0; i < map->active_player_num; i++) {
        if (strcmp(map->peers[i].ip, ip) == 0 &&
            map->peers[i].tcp_port == tcp_port) return;
    }

    PeerInfo* p = &map->peers[map->active_player_num++];
    snprintf(p->name, sizeof p->name, "%s", name);
    snprintf(p->ip,   sizeof p->ip,   "%s", ip);
    p->is_ipv6      = is_v6;
    p->tcp_port     = tcp_port;
    p->player_index = FindPlayerIndex(map->pack, name);
}

/* 在 UDP 广播 socket 上启用 SO_BROADCAST（Linux/Windows 都需要） */
static void enable_broadcast(struct mg_connection* c)
{
#if MG_ARCH == MG_ARCH_UNIX
    int fd = (int)(size_t)c->fd;
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
#elif MG_ARCH == MG_ARCH_WIN32
    SOCKET fd = (SOCKET)(size_t)c->fd;
    BOOL yes = TRUE;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
               (const char*)&yes, sizeof(yes));
#else
    (void)c;
#endif
}

/* ============================================================
 * UDP 回调
 * ============================================================ */

static void udp_listen_cb(struct mg_connection* c, int ev, void* ev_data)
{
    if (ev == MG_EV_READ) {
        struct mg_str* data = (struct mg_str*)ev_data;
        if (!data || data->len < sizeof(NetPacket)) { c->recv.len = 0; return; }

        const NetPacket* pkt = (const NetPacket*)data->buf;
        char ip[64] = {0};
        bool is6    = false;
        extract_peer_addr(c, ip, sizeof ip, &is6);

        if (memcmp(pkt->magic, MAGIC_DISC, 8) == 0) {
            add_peer(s_scan_map, pkt->name, ip, is6, ntohs(pkt->tcp_port));

            /* 单播回复自己，让早到的节点也能发现我们 */
            NetPacket resp;
            memset(&resp, 0, sizeof resp);
            memcpy(resp.magic, MAGIC_RESP, 8);
            snprintf(resp.name, sizeof resp.name, "%s", s_my_name);
            resp.tcp_port = htons(s_my_tcp_port);
            mg_send(c, &resp, sizeof resp);
        } else if (memcmp(pkt->magic, MAGIC_RESP, 8) == 0) {
            add_peer(s_scan_map, pkt->name, ip, is6, ntohs(pkt->tcp_port));
        }

        c->recv.len = 0;  /* 必须清空缓冲区，否则数据累积 */
    }
}

static void udp_bcast_cb(struct mg_connection* c, int ev, void* ev_data)
{
    (void)ev_data;
    if (ev == MG_EV_CONNECT) {
        /* 必须显式开启 SO_BROADCAST，否则 sendto 返回 EACCES */
        enable_broadcast(c);

        if (!s_bcast_sent) {
            s_bcast_sent = true;
            NetPacket pkt;
            memset(&pkt, 0, sizeof pkt);
            memcpy(pkt.magic, MAGIC_DISC, 8);
            snprintf(pkt.name, sizeof pkt.name, "%s", s_my_name);
            pkt.tcp_port = htons(s_my_tcp_port);
            mg_send(c, &pkt, sizeof pkt);
        }
    } else if (ev == MG_EV_ERROR) {
        fprintf(stderr, "[UDP] bcast error: %s\n", (char*)ev_data);
    }
}

/* ============================================================
 * 公共 API：发现
 * ============================================================ */

void UDPsearch(ip_to_player_index_map* map)
{
    if (!map) return;
    ensure_mgr();

    map->active_player_num = 0;
    s_scan_map   = map;
    s_bcast_sent = false;

    /* 从 pack 取本机名字（约定 _player_name[0]） */
    if (map->pack && map->pack->_player_name[0][0] != '\0') {
        snprintf(s_my_name, sizeof s_my_name, "%s",
                 map->pack->_player_name[0]);
    }
    if (s_my_name[0] == '\0') {
        fprintf(stderr, "[UDP] warning: my name is empty\n");
    }

    /* 启动 UDP 监听（只创建一次） */
    if (!s_udp_listen) {
        char url[64];
        snprintf(url, sizeof url, "udp://0.0.0.0:%d", DISCOVERY_PORT);
        s_udp_listen = mg_listen(&s_mgr, url, udp_listen_cb, NULL);
        if (!s_udp_listen) {
            fprintf(stderr, "[UDP] listen failed on %s\n", url);
            return;
        }
    }

    /* 创建广播连接（UDP connect 到 255.255.255.255），
     * 实际发送在 udp_bcast_cb 的 MG_EV_CONNECT 里。 */
    if (s_bcast_conn) { s_bcast_conn->is_closing = 1; s_bcast_conn = NULL; }
    s_bcast_conn = mg_connect(&s_mgr, "udp://255.255.255.255:12345",
                              udp_bcast_cb, NULL);
    if (!s_bcast_conn) {
        fprintf(stderr, "[UDP] bcast connect failed\n");
        return;
    }

    /* 事件循环，收集 DISCOVERY_WAIT_MS */
    uint64_t start = mg_millis();
    while (s_running && (mg_millis() - start) < DISCOVERY_WAIT_MS) {
        mg_mgr_poll(&s_mgr, 50);
    }

    if (s_bcast_conn) {
        s_bcast_conn->is_closing = 1;
        mg_mgr_poll(&s_mgr, 0);
        s_bcast_conn = NULL;
    }

    printf("[UDP] discovery done, found %d peers\n", map->active_player_num);
    s_scan_map = NULL;
}

/* ============================================================
 * 公共 API：TCP 服务器 / 客户端
 * ============================================================ */

static void tcp_server_cb(struct mg_connection* c, int ev, void* ev_data)
{
    if (ev == MG_EV_ACCEPT) {
        char ip[64] = {0};
        bool is6 = false;
        extract_peer_addr(c, ip, sizeof ip, &is6);
        printf("[TCP] client connected from %s%s%s\n",
               is6 ? "[" : "", ip, is6 ? "]" : "");
    } else if (ev == MG_EV_READ) {
        struct mg_str* data = (struct mg_str*)ev_data;
        if (data) {
            /* TODO: 在这里反序列化数据包，分发给对应 player */
            printf("[TCP server] recv %u bytes\n", (unsigned)data->len);
        }
        c->recv.len = 0;
    } else if (ev == MG_EV_CLOSE) {
        printf("[TCP] client disconnected\n");
    } else if (ev == MG_EV_ERROR) {
        fprintf(stderr, "[TCP server] error: %s\n", (char*)ev_data);
    }
}

int NetworkStartTCPServer(uint16_t port)
{
    ensure_mgr();
    if (s_tcp_listen) return 0;

    char url[64];
    snprintf(url, sizeof url, "tcp://0.0.0.0:%u", (unsigned)port);
    s_tcp_listen = mg_listen(&s_mgr, url, tcp_server_cb, NULL);
    if (!s_tcp_listen) {
        fprintf(stderr, "[TCP] listen failed on %s\n", url);
        return -1;
    }
    s_my_tcp_port = port;
    printf("[TCP] server started on port %u\n", (unsigned)port);
    return 0;
}

static void tcp_client_cb(struct mg_connection* c, int ev, void* ev_data)
{
    if (ev == MG_EV_CONNECT) {
        char ip[64] = {0};
        bool is6 = false;
        extract_peer_addr(c, ip, sizeof ip, &is6);
        printf("[TCP] connected to %s%s%s\n",
               is6 ? "[" : "", ip, is6 ? "]" : "");
        /* TODO: 业务握手包在这里发送，例如 mg_send(c, "HELLO", 5); */
    } else if (ev == MG_EV_READ) {
        struct mg_str* data = (struct mg_str*)ev_data;
        if (data) {
            printf("[TCP client] recv %u bytes\n", (unsigned)data->len);
        }
        c->recv.len = 0;
    } else if (ev == MG_EV_CLOSE) {
        printf("[TCP client] disconnected\n");
    } else if (ev == MG_EV_ERROR) {
        fprintf(stderr, "[TCP client] error: %s\n", (char*)ev_data);
    }
}

struct mg_connection* NetworkConnectTo(const PeerInfo* peer)
{
    if (!peer || peer->ip[0] == '\0' || peer->tcp_port == 0) return NULL;
    ensure_mgr();

    char url[160];
    if (peer->is_ipv6) {
        snprintf(url, sizeof url, "tcp://[%s]:%u",
                 peer->ip, (unsigned)peer->tcp_port);
    } else {
        snprintf(url, sizeof url, "tcp://%s:%u",
                 peer->ip, (unsigned)peer->tcp_port);
    }
    printf("[TCP] connecting -> %s\n", url);
    return mg_connect(&s_mgr, url, tcp_client_cb, NULL);
}

int NetworkConnectAll(const ip_to_player_index_map* map, int my_index)
{
    if (!map) return 0;
    int n = 0;
    for (int i = 0; i < map->active_player_num; i++) {
        const PeerInfo* p = &map->peers[i];
        if (p->player_index == my_index) continue;    /* 跳过自己 */
        if (NetworkConnectTo(p)) n++;
    }
    return n;
}

/* ============================================================
 * 主循环 / 停止
 * ============================================================ */

void NetworkPoll(int timeout_ms)
{
    ensure_mgr();
    mg_mgr_poll(&s_mgr, timeout_ms);
}

void NetworkStop(void)
{
    s_running = false;
    if (s_mgr_inited) {
        mg_mgr_free(&s_mgr);
        s_mgr_inited = false;
        s_udp_listen = NULL;
        s_tcp_listen = NULL;
        s_bcast_conn = NULL;
    }
}