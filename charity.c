#include"charity.h"
void build_charity(player* _player){
    charity_pool* c =malloc(sizeof(charity_pool));
    _player->_pool = c;
    c->_player = _player;
    c->value = 0;
    for(int i=0;i<ONEROUND;i++)
        c->res[i] = none;
}
enum c_return_code deposit(player* _player,resource_pool* _pool,enum resource type){
    enum b_return_code re = sell(_player,_pool,type,1);
    if(re == NOT_ENOUGH_CARD){ return c_NOT_ENOUGH_CARD; }
    _player->money -= price_table[_pool->price[type]];

    charity_pool* _pol = _player->_pool;
    int i=0;
    for(;_pol->res[i]!=none && i<ONEROUND;i++)
        ;
    _pol->res[i] = type;
    return c_CHECKED_DEPOSIT;
}
enum c_return_code deposit_index(player *_player, resource_pool *_pool, int index){
    enum resource type=none;
    int count = 0;
    int i=0;
    for(;i<6;i++)
        if((count+=_player->assert[i])>index){
            type = i;
            break;
        }
    if(type == none){ return c_ERROR_INDEX; }
    enum b_return_code re = sell(_player,_pool,type,1);   
    if(re == NOT_ENOUGH_CARD){ return c_NOT_ENOUGH_CARD; }
    _player->money -= price_table[_pool->price[type]];    
    charity_pool* _pol = _player->_pool;                  
    i=0;                                              
    for(;_pol->res[i]!=none && i<ONEROUND;i++)            
        ;                                                 
    _pol->res[i] = type;                                  
    return c_CHECKED_DEPOSIT;                             
}
void discount(player *_player, resource_pool *_pool)
{
    charity_pool* _pol = _player->_pool;
    for(int i=0;i<ONEROUND&& _pol->res[i] !=none;i++){
        _pol->value+= price_table[_pool->price[_pol->res[i]]];
        _pol->res[i] = none;
    }
}