#ifndef CORE_H
#define CORE_H

#include<stdbool.h>
#include<string.h>
#include<stdio.h>
#include"data.h"

extern const char* names[];
extern const char* pure_names[];
bool _bond(resource_pool* pool, share* _share, enum resource type,int num);

bool bond_array(resource_pool* pool, share* array[6]);


// 核心业务函数
enum b_return_code bought(player* _player, resource_pool* _pool, enum resource type,int num);

enum b_return_code sell(player* _player, resource_pool* _pool,enum resource type, int num);

int leave_(resource_pool* _pool,enum resource res);
#endif