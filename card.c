#include"card.h"

const int price_table[20] = {0 , 5 , 10 , 15, 20, 35, 40, 50, 60, 70, 80, 90, 120, 150, 180, 210, 240, 270, 300, 330};
const int card_price[6] = {-6, -4, -2, 2, 4, 6};
void generate(player* left_player,player* right_player, bool seed_init){
    if(seed_init){ srandom((unsigned)time(NULL)); }
    oneside_card* _one = malloc(sizeof(oneside_card));
    _one->array = malloc(sizeof(int[6])*6);
    _one->left = left_player;
    _one->right = right_player;
    left_player->right = _one;
    right_player->left = _one;
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            _one->array[i][j] = 0;
        }
    }
    for(int i=0;i<8;i++){
        long int temp = random();
        _one->array[temp%36/6][temp%36%6] ++;
    }
}

enum u_return_code use(int i,int j,bool is_left,bool is_half,player* _player,resource_pool* _pool){
    oneside_card* c = (is_left) ? _player->left : _player->right;
    if(c->array[i][j]==0){ return u_NOT_ENOUGH_CARD; }
    c->array[i][j] --;
    _pool->price[i] += (is_half) ? card_price[j]/2 : card_price[j];
    _pool->price[i] = (_pool->price[i]>=0)? _pool->price[i]:0;
    return u_CHECKED_PLAY;
}
enum u_return_code use_index(int index,bool is_left,bool is_half,player* _player,resource_pool* _pool){
    oneside_card* c = (is_left) ? _player->left : _player->right;
    int i=0;
    int j=0;
    int count = 0;
    for(;i<6;i++){
        for(j=0;j<6;j++){
            if((count+=c->array[i][j])>index)
                goto out;   
        }
    }
    out:
    if(count <= index){ return u_NOT_ENOUGH_CARD; }
    c->array[i][j]--;
    _pool->price[i] += (is_half) ? card_price[j]/2 : card_price[j];
    _pool->price[i] = (_pool->price[i]>=0)? _pool->price[i]:0;
    return u_CHECKED_PLAY;
}                                                                       
                                                                       