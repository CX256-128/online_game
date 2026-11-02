#include"package.h"
package* init(int active_player_num,int principal,int start_price_index){
    if(active_player_num >MAX_PLAYERS){ return NULL; }
    package* pak = malloc(sizeof(package));

    // 初始化ID_to_connection table
    pak->itc = malloc(sizeof(ID_to_connection));
    for(int i=0;i<MAX_PLAYERS;i++){
        pak->itc->map[i] = NULL;
    }
    //for(int i=0;i<MAX_PLAYERS;i++){
    //    pak->_player[i] = malloc(sizeof(player));
    //}

    for(int i=0;i<6;i++){
        pak->_share[i] = malloc(sizeof(share));
    }
    pak->_r_pool = malloc(sizeof(resource_pool));
    // 绑定pool 与share并初始化各个share里面的ptr = NULL
    bond_array(pak->_r_pool,pak->_share);
    // 初始化各个price
    for(int i=0;i<6;i++)
        pak->_r_pool->price[i] = start_price_index;
    
    // 绑定玩家
    pak->active_player_num = active_player_num;
    int i=0;
    for(;i<active_player_num;i++){
        pak->_player[i]->money = principal;
        build_charity(pak->_player[i]);
        for(int j=0;j<6;j++)
            pak->_player[i]->assert[j] = 0;
    }
    for(;i<MAX_PLAYERS;i++)
        pak->_player[i] = NULL;
    //生成各自的card
    generate(pak->_player[0],pak->_player[1],true);
    for(i=1;i<active_player_num;i++){
        generate(pak->_player[i%active_player_num],pak->_player[(i+1)%active_player_num],false);
    }
    return pak;
}