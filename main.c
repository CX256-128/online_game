#include<stdio.h>
#include<ctype.h>
#include"getch.h" // 这里引入getch()函数
#include"data.h"
#include"core.h"
#include"card.h"
#include"charity.h"
#include"package.h"
#include"visualization.h"
enum resource stores(char* buf){
    if(strcmp(buf,pure_names[0])==0)
        return coal;
    else if(strcmp(buf,pure_names[1])==0)
        return crop;
    else if(strcmp(buf,pure_names[2])==0)
        return corn;
    else if(strcmp(buf,pure_names[3])==0)
        return tea;
    else if(strcmp(buf,pure_names[4])==0)
        return cotton;
    else if(strcmp(buf,pure_names[5])==0)
        return coffee;
    else
        return none;
}
#if defined(_WIN32)
  #include <conio.h>
  #define PRESS_ANY_KEY() _getch()
#else
  #include <termios.h>
  #include <unistd.h>
  static int press_any_key_posix(void) {
      struct termios oldt, newt;
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      int c = getchar();
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return c;
  }
  #define PRESS_ANY_KEY() press_any_key_posix()
#endif
int switch_card(int total_row,int total_line,bool can_quit){
    int pos_r = 0;
    int pos_l = 0;
    if(total_row<=0){ return -1; }
    printf("/\\");
    while(1){
        fflush(stdout);
        int command = getch();
        if(command == 'j'|| command == 'J')
            return pos_r+pos_l*total_row;
        else if(command == 'a' || command == 'A'){
            printf("\033[3D");
            printf("   ");
            fflush(stdout);
            if(pos_r == 0){
                int i=0;
                for(;i<total_row-2;i++)
                    printf("\033[12C");
                if(i<total_row-1)
                    printf("\033[10C");
                printf("/\\");
            }
            else{
                printf("\033[14D");
                printf("/\\");
            }
            pos_r = (pos_r -1+total_row)%total_row;
        }
        else if(command == 'd'|| command == 'D'){
            printf("\033[3D");
            printf("   ");
            fflush(stdout);
            if(pos_r == total_row-1){
                int i=0;
                for(;i<total_row-2;i++)
                    printf("\033[12D");
                if(i<total_row-1)
                    printf("\033[14D");
                printf("/\\");
            }
            else{
                printf("\033[10C");
                printf("/\\");
            }
            pos_r = (pos_r+1)%total_row;
        }
        else if(command == 's'|| command == 'S'){
            printf("\033[3D");
            printf("   ");
            fflush(stdout);
            if(pos_l == total_line -1){
                for(int i=0;i<total_line-1;i++)
                    printf("\033[4A");
                printf("\033[2D");
                printf("/\\");
            }
            else{
                printf("\033[4B");
                printf("\033[2D");
                printf("/\\");
            }
            pos_l = (pos_l+1)%total_line;
        }
        else if(command == 'w'||command == 'W'){
            printf("\033[3D");
            printf("   ");
            fflush(stdout);
            if(pos_l == 0){
                for(int i=0;i<total_line-1;i++)
                    printf("\033[4B");
                printf("\033[2D");
                printf("/\\");
            }
            else{
                printf("\033[4A");
                printf("\033[2D");
                printf("/\\");
            }
            pos_l = (pos_l -1 +total_line) %total_line;
        }
        else if((command == 'q' || command == 'Q') && can_quit)
            return -1;
        else if((command == 27)){
            command = getch();
            if(command == 91){
                command = getch();
            }
        }
    }
}
int main(){
    /*package* game = init(4,200,7);
    printf("This is the player1's money:%d\n",game->_player[1]->money);
    printf("This is the %s's price: %d\n",names[coal],price_table[game->_r_pool->price[coal]]);
    bought(game->_player[1],game->_r_pool,coal,3);
    printf("this is the result of BOUGHT:money left player1: %d\n",game->_player[1]->money);
    printf("Now we will take a look at the card we have\n");
    for(int i=0;i<4;i++){
        printf("player%d's left\n",i);
        for(int j=0;j<36;j++)
            printf("%d%c",game->_player[i]->left->array[j/6][j%6],((j+1)%6!=0)?' ':'\n');
        printf("player%d's right\n",i);                                              
        for(int j=0;j<36;j++)                                                       
            printf("%d%c",game->_player[i]->right->array[j/6][j%6],((j+1)%6!=0)?' ':'\n');
    }
    exhibit_price(game);
    exhibit_card(game,0);
    game->_player[0]->assert[0] = 3;
    game->_player[0]->assert[1] = 4;
    game->_player[0]->assert[2] = 4;
    game->_player[0]->assert[3] = 3;
    game->_player[0]->assert[4] = 2;
    game->_player[0]->assert[5] = 1;
    exhibit_your_assert(game,0);
    printf("\n");
    game->_player[0]->_pool->res[0] = coal;
    game->_player[0]->_pool->res[1] = cotton;
    game->_player[0]->_pool->res[2] = coffee;
    //game->_player[0]->_pool->res[3] = crop;
    printf("My money:%d\n",game->_player[0]->money);
    deposit(game->_player[0],game->_r_pool,crop);
    show_your_charity(game,0,true);
    show_your_charity(game,0,false);
    exhibit_your_assert(game,0);
    printf("My money:%d\n",game->_player[0]->money);
    /*package* game = init(4,200,6);
    for(int i=0;i<ONEROUND;i++){
        for(int j=0;j<MAX_PLAYERS;j++){
            printf("\033[2J");
            printf("Player ID: %d\n",j);

        }
    }*/
    printf("投资小游戏\n");
    printf("玩家数量(输入一个1-%d的整数):",MAX_PLAYERS);
    int num=2;
    scanf("%d",&num);
    int c;                                     
    while ((c = getchar()) != '\n' && c != EOF)
        ;                                      
    package* game = init(num,200,6);
    printf("\033[2J");
    for(int i=0;i<ONEROUND;i++){
        for(int j=0;j<num;j++){
            printf("\033[2J");
            printf("This is player \033[1m%d\033[0m\n",j);
            printf("你拥有的资金: \033[1m%d\033[0m\n",game->_player[j]->money);
            printf("当前的市场行情:\n");
            exhibit_price(game);
            printf("你拥有的牌\n");
            exhibit_card(game,j);
            check_if_buy:
            printf("你是否要购买？(Y/N y/n)");
            char single = 'n';
            single = getchar();
            single = tolower(single); 
            if(single == 'y'){
                printf("\n下面输入你要购买的期货英文名称以及数量，用空格分隔，回车结束\n");
                char* buf = NULL;
                //if(fgets(buf,sizeof(buf),stdin)!=NULL){
                //    buf[strcspn(buf, "\n")] = '\0';
                //}
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                size_t cap = 0;      // 容量，由 getline 自动管理
                ssize_t n = getline(&buf, &cap, stdin);
                
                char share_name[8];
                int num = 0;
                sscanf(buf,"%s %d",share_name,&num);
                enum b_return_code k =bought(game->_player[j],game->_r_pool,stores(share_name),num);
                free(buf);
                switch(k){
                    case NOT_ENOUGH_MONEY:
                        printf("你要买%d的%s,但是你的钱不够\n返回购买意向页面\n",num,share_name);
                        goto check_if_buy;
                    case NOT_ENOUGH_CARD:
                        printf("本期货已经爆仓了，你最多只能买%d个单位\n返回购买意向页面\n",leave_(game->_r_pool,stores(share_name)));
                        goto check_if_buy;
                    case PRICE_ZERO:
                        printf("该期货价格归零，禁止买卖\n返回购买意向页面\n");
                        goto check_if_buy;
                    case NON_POSITIVE_NUM:
                        printf("禁止挖漏洞\n返回购买意向页面\n");
                        goto check_if_buy;
                    case CHECKED_TRANSACTION:
                        printf("完成交易\n");
                        printf("下面展示你拥有的资产\n");
                        exhibit_your_assert(game,j);
                        printf("按任意键继续...\n");
                        PRESS_ANY_KEY();
                        break;
                    case UNKNOWN_SHARE:
                        printf("不知道你输入的是什么\n返回购买意向页面\n");
                        goto check_if_buy;
                    case BOUGHT_TOO_MUCH:
                        printf("你买多了，单笔最多买%d个单位\n返回购买意向页面\n",MAX_BOUGHT_IN_ONE);
                        goto check_if_buy;
                    default:
                        break;
                }
            }
            else{
                check_if_sold:
                printf("你是否要卖出？(Y/N y/n)");
                single = getchar();       
                single = tolower(single);     
                if(single =='y'){
                    printf("\n下面展示你拥有的期货\n");
                    exhibit_your_assert(game,j);
                    printf("\n输入你你要售出的期货英文名称以及数量，用空格分隔，回车结束\n");
                    char* buf = NULL;                                            
                    while ((c = getchar()) != '\n' && c != EOF)      
                        ;                                            
                    size_t cap = 0;      // 容量，由 getline 自动管理
                    ssize_t n = getline(&buf, &cap, stdin);                                          
                    char share_name[8];                              
                    int num = 0;                                     
                    sscanf(buf,"%s %d",share_name,&num);
                    enum b_return_code k =sell(game->_player[j],game->_r_pool,stores(share_name),num);
                    switch(k){
                        case NON_POSITIVE_NUM:                       
                            printf("禁止挖漏洞\n返回售出意向页面\n");
                            goto check_if_sold;                       
                        case SOLD_TOO_MUCH:
                            printf("单笔最大交易量为%d\n返回售出意向页面\n",MAX_SOLD_IN_ONE);
                            goto check_if_sold;
                        case NOT_ENOUGH_CARD:
                            printf("你没有 %d 的期货可供售出，你的 %s 的数量为 %d\n返回售出意向页面\n",num,share_name,game->_player[j]->assert[stores(share_name)]);
                            goto check_if_sold;
                        case CHECKED_TRANSACTION:
                            printf("交易成功\n");
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        for(int j=0;j<num;j++){
            fflush(stdout);
            printf("\033[2J");
            printf("下面进入操作市场环节\n");
            printf("This is player \033[1m%d\033[0m\n",j);
            exhibit_card(game,j);
            printf("上面左手边的牌可以用一张，右手边的牌也可以用一张，但是用出的两张牌中必须指定一张实现一半的效果\n");
            printf("先选择左手的牌(使用a向左移动光标 d向右移动光标 j选中)\n");
            printf("\033[9F\033[5C");
            int left_use = switch_card(8-2*i,1,false);
            printf("\033[10E");
            printf("你使用了左边的%d号卡片，接下来继续右手的牌\n",left_use);
            fflush(stdout);
            printf("\033[4F\033[5C");
            int right_use = switch_card(8-2*i,1,false);
            printf("\033[4E");
            printf("你使用了右边的%d号卡片\n",right_use);
            half_opt:
            printf("你需要将哪张卡片的操作取半(L/l表示左边的卡片 R/r表示右边的卡片 按回车确认):");
            char opt;
            scanf("%c",&opt);
            opt = tolower(opt);
            if(opt!='l'&&opt!='r'){
                printf("输入错误请重新输入\n");
                goto half_opt;
            }
            enum u_return_code u = use_index(left_use,true,(opt=='l'),game->_player[j],game->_r_pool);
            switch(u){
                case u_NOT_ENOUGH_CARD:
                    printf("没有这张卡片，重新操作\n");
                    j--;
                    continue;
                case u_CHECKED_PLAY:
                    break;
                default:
                    break;
            }
            u = use_index(right_use,false,(opt=='r'),game->_player[j],game->_r_pool);
            switch(u){
                case u_NOT_ENOUGH_CARD:
                    printf("没有这张卡片，重新操作\n");
                    j--;
                    continue;
                case u_CHECKED_PLAY:
                    break;
                default:
                    break;
            }
            printf("按任意键继续");
            PRESS_ANY_KEY();
        }
        for(int j=0;j<num;j++){
            fflush(stdout);   
            printf("\033[2J");
            printf("下面进入\"自愿\"环节\n");
            printf("This is player \033[1m%d\033[0m\n",j);
            printf("下面是当前行情\n");
            exhibit_price(game);
            printf("下面是你已经投入的慈善期货\n");
            show_your_charity(game,j,true);
            printf("下面是你当前持有的期货\n");
            int total = exhibit_your_assert(game,j);
            if(total == 0){ printf("请按任意键继续...\n");PRESS_ANY_KEY(); continue; }
            printf("\n先选择一张牌(使用a向左移动光标 d向右移动光标 j选中 q放弃)注:选中空的地方视同放弃\n");
            printf("\033[%dA\033[5C",total/MAX_NUM_OF_INLINE_CARDS*4+2);
            int u = switch_card((total>MAX_NUM_OF_INLINE_CARDS)?MAX_NUM_OF_INLINE_CARDS:total,total/MAX_NUM_OF_INLINE_CARDS +1,true);
            if(u==-1||u>=total){ printf("放弃本轮投入");}
            else{
                enum c_return_code m = deposit_index(game->_player[j],game->_r_pool,u);
                switch(m){
                    case c_ERROR_INDEX:
                        printf("选择的牌不存在，重新选或弃权\n");
                        j--;
                        break;
                    case c_NOT_ENOUGH_CARD:
                        printf("选择的牌不够，重新选或弃权\n");
                        j--;
                        break;
                    case c_CHECKED_DEPOSIT:
                        printf("成功投入");
                        break;
                    default:
                        break;
                }
            }
            printf("  按任意键继续...");
            PRESS_ANY_KEY();
        }
    }
    return 0;
}