#ifndef NETWORK_H
#define NETWORK_H
#include"mongoose.h"
#include"../package.h"
#include"../data.h"
//使用方法
// 在对应的主线程中直接调用get_message函数，同时传入对应的处理函数
// 这边会直接使用一个mg_poll()开始异步等待网络中数据传入同时在有数据的时候激活回调函数

typedef void (*handler)(int* argc, char *buf,size_t len);
extern bool is_room_onwer;

enum n_return_code{
    n_SEND_TO_NULL,
    n_SEND_SUCCESS,
    n_SEND_FAIL
};
typedef struct ID_to_connection{
    struct mg_connection* map[MAX_PLAYERS];
    char* name[MAX_PLAYERS];
    package* pak;
}ID_to_connection;

enum n_return_code send_by_id(package* pak,int i,char** msg);
enum n_return_code send_by_connection(struct mg_connection* _connect,char** msg);
void ip_sort(package* pak,int previous,int now);
void get_message(package* pak,handler handle_function,int* argc,char* argv,size_t len);


void invite(int* argc,char* buf,size_t len);// 本身是房源需要等对方如果同意了需要发送 ID_to_connection
void lack_id(int* argc,char* buf,size_t len);// 如果对方说有一个对象连接不上需要你测试一下你连通性，如果连接不上直接 end掉
void receive_invite(int* argc,char* buf,size_t len);
void ip_register(int* argc,char* buf,size_t len);
void name_register(int* argc,char* buf,size_t len);
