#include"network.h"
bool is_room_onwer=false;
bool tls_shaking=false;
bool waiting = false;
bool done = false;
static size_t strlen_(char* msg,char end_label){
    size_t num = 0;
    for(;*(msg+num)!=end_label;)
        ;
    return num;
}
enum n_return_code send_by_id(package* pak,int i,char* msg,char end_label){
    if(pak->itc->map[i]==NULL){ return SEND_TO_NULL; }
    if(mg_send(pak->itc->map[i],msg,strlen_(msg,end_label))){
        //调试信息
        char buf[64];                                                           
        mg_snprintf(buf,sizeof(buf),"%M",mg_print_ip_port,pak->itc->map[i].rem);
        printf("发送成功_to_%s\n",buf);
        return n_SEND_SUCCESS;
    }else{
        char buf[64];
        mg_snprintf(buf,sizeof(buf),"%M",mg_print_ip_port,pak->itc->map[i].rem);
        printf("失败_transfer_%s:\n",buf);
        return n_SEND_FAIL;
    }
}
void invite(int* argc,char* buf,size_t len){
    // 这里传入的(char *)buf实质上是(struct ID_to_connection*)buf
    // 因为指针本质都是void* 这里直接复用接口就不修改了
    // 本质上本函数的操作逻辑就是先广播一下发现信息
    // 然后设置标记位为已经发送，并记录时间
    // 然后get_message()函数中mg_poll()轮询
    // 如果 有人有返回特定的代码的话，代表接受邀请，就注册ID_to_connection*中的IP
    // 并将TLS标记为置为true
    // 开始等待下一次握手
    // 握手完成连接之后开始发送当前的ID_to_connection*的copy 向着这个列表中所有的对象
    // 之后将waiting标记置为true等待回信确认收到
    // 等到所有的对象都回信收到之后将waiting TLS标记都置于false
    // 如果等待时间超过默认retry时间了就再次广播发现信息，同时更新时间
}
void get_message(package* pak,handler handle_function,int argc,char* argv){

}