#ifndef CHARITY_H
#define CHARITY_H

#define ONEROUND 4
#include"data.h"
#include"core.h"
#include<stdlib.h>
enum c_return_code{
    c_NOT_ENOUGH_CARD,
    c_CHECKED_DEPOSIT,
    c_ERROR_INDEX
};
typedef struct charity_pool{
    int value;
    enum resource res[ONEROUND];
    player* _player;
}charity_pool;

void build_charity(player* _player);
enum c_return_code deposit(player* _player,resource_pool* _pool,enum resource type);
enum c_return_code deposit_index(player* _player,resource_pool* _pool,int index);
void discount(player* _player,resource_pool* _pool);
#endif