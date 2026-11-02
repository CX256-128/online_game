#ifndef DATA_H
#define DATA_H

#define MAX_PLAYERS 6
#define MAX_CARDS 10 
#define MAX_BOUGHT_IN_ONE 3
#define MAX_SOLD_IN_ONE 3
extern const int price_table[20];

enum resource{                                                                                                                  
    coal,                                                                                                                     
    crop,                                                                                                                       
    corn,                                                                                                                       
    tea,                                                                                                                        
    cotton,                                                                                                                     
    coffee,
    none                                                                                                                      
};                                                                                                                              
enum b_return_code{                                                                                                             
    NOT_ENOUGH_MONEY,                                                                                                           
    NOT_ENOUGH_CARD,                                                                                                            
    PRICE_ZERO,                                                                                                                 
    NON_POSITIVE_NUM,                                                                                                           
    CHECKED_TRANSACTION,
    BOUGHT_TOO_MUCH,
    UNKNOWN_SHARE,
    SOLD_TOO_MUCH                                                                                                         
};                                                                                                                              
                                                                                                                                
struct onside_card;
struct charity_pool;                                                                                                                     
typedef struct player{                                                                                                          
    int money;                                                                                                                  
    int assert[6];
    struct oneside_card* left;
    struct oneside_card* right;
    struct charity_pool* _pool;                                                                            
}player;                                                                                                                        
                                                                                                                                
                                                                                                                                
typedef struct share{ // share的核心功能还是提供一个从 resource - > player的指针可以在某系可能的场景下帮助查看这个资源还有谁持有
    player* ptr[MAX_PLAYERS];                                                                                                   
}share;                                                                                                                         
                                                                                                                                
                                                                                                                                
typedef struct resource_pool{                                                                                                   
    share* shares[6];                                                                                                           
    int price[6];    // 保存price_table上的对应序号                                                                                                           
}resource_pool;                

#endif