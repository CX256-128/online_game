#include"visualization.h"
const char* gap[20] = {"  ", "  ","   ","   ","   ","   ","   ","   ","   ","   ","   ","   ","    ","    ","    ","    ","    ","    ","    ","    "};
const char* gap_f[6] = {"  ", "  ","  ","  "," "," "};
const char* gap_b[6] = {"  ","  ","  ","   "," "," "};
const char* mask[6] = {"    ","    ","    ","   ","      ","      "};
const char* number_gap = "   ";
void exhibit_price(package* game){
    printf("\t");                                          
    for(int i=0;i<20;i++){                                 
        printf("%d%c",price_table[i],(i == 19)? '\n' :' ');
    }                                                      
    for(int i=0;i<6;i++){                                  
        printf("%s\t",names[i]);                           
        for(int j=0;j<game->_r_pool->price[i];j++)
            printf("%s",gap[j]);
        printf("x\n");                                               
    }                                 
}

// id 这里是表示package下的player数组的序号
void exhibit_card(package* game,int id){
    struct oneside_card* left = game->_player[id]->left;
    struct oneside_card* right = game->_player[id]->right;
    int count_L=0;
    int count_R=0;
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            if(left!=NULL && left->array[i][j]!=0)
                count_L+= left->array[i][j];
            if(right !=NULL && right->array[i][j]!=0)
                count_R+= right->array[i][j];
        }
    }
    printf("左边的卡牌\n");
    for(int i=0;i<count_L;i++){
        printf(" ┌--------┐%c",(i!=count_L-1)?' ':'\n');
    }
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            if(left->array[i][j]!=0){
                int temp = left->array[i][j];
                for(;temp!=0;temp--)
                    printf(" |%s%s%s| ",gap_f[i],names[i],gap_b[i]);
            }
        }
    }
    printf("\n");
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            if(left->array[i][j]!=0){
                int temp = left->array[i][j];
                for(;temp!=0;temp--)
                    printf(" |%s%+d%s| ",number_gap,card_price[j],number_gap);
            }
        }
    }
    printf("\n");
    for(int i=0;i<count_L;i++){
        printf(" └--------┘%c",(i!=count_L-1)?' ':'\n');
    }
    printf("\n右边的卡牌\n");                                                   
    for(int i=0;i<count_R;i++){                                               
        printf(" ┌--------┐%c",(i!=count_R-1)?' ':'\n');                      
    }                                                                         
    for(int i=0;i<6;i++){                                                     
        for(int j=0;j<6;j++){                                                 
            if(right->array[i][j]!=0){                                         
                int temp = right->array[i][j];                                 
                for(;temp!=0;temp--)                                          
                    printf(" |%s%s%s| ",gap_f[i],names[i],gap_b[i]);          
            }                                                                 
        }                                                                     
    }                                                                         
    printf("\n");                                                             
    for(int i=0;i<6;i++){                                                     
        for(int j=0;j<6;j++){                                                 
            if(right->array[i][j]!=0){                                         
                int temp = right->array[i][j];                                 
                for(;temp!=0;temp--)                                          
                    printf(" |%s%+d%s| ",number_gap,card_price[j],number_gap);
            }                                                                 
        }                                                                     
    }                                                                         
    printf("\n");                                                             
    for(int i=0;i<count_R;i++){                                               
        printf(" └--------┘%c",(i!=count_R-1)?' ':'\n');                      
    }             
    printf("\n");                                                            
}

int exhibit_your_assert(package* game,int id){
    player* _this = game->_player[id];
    int count = 0;
    int i=0;
    int temp = 0;
    for(;i<6;i++){
        count+=_this->assert[i];
    }
    int label = 0;
    int line_num = (count/MAX_NUM_OF_INLINE_CARDS) + 1;
    int leave = count % MAX_NUM_OF_INLINE_CARDS;
    count = 0;
    temp = _this->assert[label];
    for(i=0;i<line_num;i++){
        count = 0;
        for(int j=0;j<((i==line_num-1)?leave:MAX_NUM_OF_INLINE_CARDS);j++)
            printf(" ┌--------┐ ");
        printf("\n");
        print_judge:
        if((temp + count )>= ((i==line_num-1)?leave:MAX_NUM_OF_INLINE_CARDS)){
            for(int i=0;(i<(MAX_NUM_OF_INLINE_CARDS - count) )&&(temp!=0);i++,temp--)
                printf(" |%s%s%s| ",gap_f[label],names[label],gap_b[label]);
            printf("\n");
            for(int j=0;j<((i==line_num-1)?leave:MAX_NUM_OF_INLINE_CARDS);j++)
                printf(" └--------┘ ");                                     
            printf("\n");
            continue;
        }
        else if((temp + count)< ((i==line_num-1)?leave:MAX_NUM_OF_INLINE_CARDS)){
            count+=temp;
            for(;temp!=0;temp--)
                printf(" |%s%s%s| ",gap_f[label],names[label],gap_b[label]);
            temp = _this->assert[++label];
            goto print_judge;
        }
    }
    return (line_num-1)*MAX_NUM_OF_INLINE_CARDS + leave;
}
void show_your_charity(package* game,int id,bool visible){
    int count = 0;
    charity_pool* _pol = game->_player[id]->_pool;
    for(;count<ONEROUND && _pol->res[count]!=none;count++)
        printf(" ┌--------┐ ");
    printf("\n");
    for(int i=0;i<count;i++)
        printf(" |%s%s%s| ",gap_f[_pol->res[i]],(visible)?names[_pol->res[i]]:mask[_pol->res[i]],gap_b[_pol->res[i]]);
    printf("\n");
    for(int i=0;i<count;i++)
        printf(" └--------┘%c",(i!=count - 1)?' ':'\n');
}