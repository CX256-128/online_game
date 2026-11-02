#include "network.h"
#include <stdio.h>
#include <string.h>

int main(void)
{


    /* 1) 先启动 TCP 服务端（端口会写进发现包） */
    if (NetworkStartTCPServer(TCP_SERVER_PORT) != 0) return 1;

    /* 2) 阻塞扫描 3 秒 */
    ip_to_player_index_map my_map;
    memset(&my_map, 0, sizeof my_map);
    my_map.pack = NULL;
    UDPsearch(&my_map);

    printf("--- found %d peers ---\n", my_map.active_player_num);
    for (int i = 0; i < my_map.active_player_num; i++) {
        const PeerInfo* p = &my_map.peers[i];
        printf("  %d) %-16s  %s%s%s  tcp_port=%u  player_index=%d\n",
               i, p->name,
               p->is_ipv6 ? "[" : "", p->ip, p->is_ipv6 ? "]" : "",
               (unsigned)p->tcp_port, p->player_index);
    }

    /* 3) 主动连接所有非自己节点 */
    int n = NetworkConnectAll(&my_map, /*my_index=*/0);
    printf("initiated %d TCP connections\n", n);

    /* 4) 主循环 */
    while (1) {
        NetworkPoll(100);

        // 可选：周期性重扫，例如每 10 秒重新发现一次
        static uint64_t last = 0;
        if (mg_millis() - last > 10000) { UDPsearch(&my_map); last = mg_millis(); }
        
    }

    NetworkStop();
    return 0;
}