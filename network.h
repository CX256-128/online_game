#ifndef NETWORK_H
#define NETWORK_H

/* 注意：原 extern-inlcude 是笔误，已改为 extern-include */
#include "./extern-include/mongoose.h"
#include <stdbool.h>
#include <stdint.h>
#include "package.h"
#include "data.h"

#ifndef MAX_PLAYERS
#error "MAX_PLAYERS 未定义，请先在 data.h 中定义"
#endif

#define DISCOVERY_PORT      12345
#define TCP_SERVER_PORT     9000
#define DISCOVERY_WAIT_MS   3000

/* 一个已发现节点的完整路由信息 */
typedef struct {
    char     name[256];     /* 玩家名 */
    char     ip[64];        /* 纯 IP，IPv6 链路本地地址带 %scope，可直接拼 URL */
    uint16_t tcp_port;      /* 对端 TCP 端口（来自发现包，非 c->rem.port） */
    bool     is_ipv6;       /* 拼 URL 时是否加方括号 */
    int      player_index;  /* 对应 package->_player 的下标，找不到为 -1 */
} PeerInfo;

typedef struct ip_to_player_index_map {
    int      active_player_num;
    PeerInfo peers[MAX_PLAYERS];
    package* pack;
} ip_to_player_index_map;

/* 阻塞约 DISCOVERY_WAIT_MS 毫秒，完成后填充 map->peers[]。 */
void UDPsearch(ip_to_player_index_map* map);

/* 启动 TCP 服务端（被其他玩家连接），应在 UDPsearch 之前调用。 */
int  NetworkStartTCPServer(uint16_t port);

/* 根据一个 PeerInfo 主动发起 TCP 连接。 */
struct mg_connection* NetworkConnectTo(const PeerInfo* peer);

/* 对 map 中所有非自己节点发起 TCP 连接。 */
int  NetworkConnectAll(const ip_to_player_index_map* map, int my_index);

/* 主循环里持续调用，驱动所有 UDP/TCP 事件。 */
void NetworkPoll(int timeout_ms);

/* 关闭 mgr，释放所有连接。 */
void NetworkStop(void);

/* 工具：在 package->_player_name[] 中查玩家下标。 */
int  FindPlayerIndex(package* pack, const char* name);

#endif