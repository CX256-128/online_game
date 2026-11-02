#ifndef CARD_H
#define CARD_H
#include"data.h"
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>


extern const int card_price[6];
enum u_return_code{
    u_NOT_ENOUGH_CARD,
    u_CHECKED_PLAY
};
typedef struct oneside_card{
    int (*array)[6];// array 是一个6行 6列的数组，只生成一次
    player* left;
    player* right;
}oneside_card;

void generate(player* left_player,player* right_player, bool seed_init);
enum u_return_code use(int i,int j,bool is_left,bool is_half,player* _player,resource_pool* _pool);
enum u_return_code use_index(int index,bool is_left,bool is_half,player* _player,resource_pool* _pool);
#endif