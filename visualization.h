#ifndef VISUALIZATION_H
#define VISUALIZATION_H
#define MAX_NUM_OF_INLINE_CARDS 6
#include"package.h"
extern const char* gap[20];
extern const char* gap_f[6];
extern const char* gap_b[6];
extern const char* number_gap;
extern const char* mask[6];
void exhibit_price(package* game);
void exhibit_card(package* game,int id);
int exhibit_your_assert(package* game,int id);
void show_your_charity(package*game,int id,bool visible);
#endif