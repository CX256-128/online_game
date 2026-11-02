#include"core.h"

const char* names[]={
    "\033[38;5;240mcoal\033[0m",
    "\033[38;5;70mcrop\033[0m",
    "\033[38;5;220mcorn\033[0m",
    "\033[38;5;64mtea\033[0m",
    "\033[38;5;230mcotton\033[0m",
    "\033[38;5;94mcoffee\033[0m",
};
const char* pure_names[]={
    "coal",
    "crop",
    "corn",
    "tea",
    "cotton",
    "coffee"
};
bool _bond(resource_pool* pool, share* _share, enum resource type,int num){
    for(int i=0;i<MAX_PLAYERS;i++){
        _share->ptr[i] = NULL;
    }
    pool->shares[type] = _share;
    return true;
}

bool bond_array(resource_pool* pool, share* array[6]){
    for(int i=0;i<6;i++){
        if(_bond(pool,array[i],i,1));
        else{ 
            printf("one error happened when bonding %s\n",names[i]);
        }
    }
    return true;
}

enum b_return_code bought(player* _player, resource_pool* _pool, enum resource type,int num){
    if(num <=0){ return NON_POSITIVE_NUM; }
    if(_pool->price[type] == 0){ return PRICE_ZERO; }
    if(num >MAX_BOUGHT_IN_ONE){ return BOUGHT_TOO_MUCH; }
    if(type == none){ return UNKNOWN_SHARE; }
    bool is_found = false;
    int max = 0;
    int possessed_cards = 0;
    for(int i=0;i<MAX_PLAYERS;i++){
        if(_pool->shares[type]->ptr[i] == NULL){
            max = i;
            break;
        }
        else{
            possessed_cards += _pool->shares[type]->ptr[i]->assert[type];
        }
        if(_pool->shares[type]->ptr[i] == _player){
            is_found = true;                       
        }                                          
    }
    if(possessed_cards + num > MAX_CARDS){ return NOT_ENOUGH_CARD; }
    if(_player->money < _pool->price[type] * num){ return NOT_ENOUGH_MONEY; }
    else{
        _player->money -= price_table[_pool->price[type]] * num;
        _player->assert[type] +=num;
        if(!is_found){
            _pool->shares[type]->ptr[max] = _player;
        }
    }
    return CHECKED_TRANSACTION;
}
enum b_return_code sell(player* _player, resource_pool* _pool, enum resource type, int num){
    if(num <0){ return NON_POSITIVE_NUM; }
    if(num > MAX_SOLD_IN_ONE){ return SOLD_TOO_MUCH; }
    if(_player->assert[type]<num){ return NOT_ENOUGH_CARD; }
    _player->assert[type] -=num;
    _player->money+=num*price_table[_pool->price[type]];
    if(_player->assert[type] == 0){
        int i=0;
        for(;_pool->shares[type]->ptr[i] != _player;i++)
            ;
        for(;i<MAX_PLAYERS-1 && _pool->shares[type]->ptr[i] != NULL;i++){
            _pool->shares[type]->ptr[i] = _pool->shares[type]->ptr[i+1];
        }
        if(i == MAX_PLAYERS - 1)
            _pool->shares[type]->ptr[i] = NULL;
    }
    return CHECKED_TRANSACTION;
}

int leave_(resource_pool *_pool,enum resource res){   
    int possessed_cards = 0;
    for(int i=0;i<MAX_PLAYERS;i++){                                                                             
        if(_pool->shares[res]->ptr[i] == NULL){                                                                           
            break;                                                       
        }                                                                
        else{                                                            
            possessed_cards += _pool->shares[res]->ptr[i]->assert[res];
        }                                                                
    }   
    return MAX_CARDS - possessed_cards;
}                                                                    
