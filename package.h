#ifndef PACKAGE_H
#define PACKAGE_H
#include"data.h"
#include"card.h"
#include"charity.h"
#include"./network.h"
typedef struct package{
    int active_player_num;
    player* _player[MAX_PLAYERS];
    resource_pool* _r_pool;
    share* _share[6];
    char _player_name[MAX_PLAYERS][256];
    ID_to_connection* itc;
}package;

package* init(int active_player_num,int principal,int start_price_index);
#endif