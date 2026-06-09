#ifndef MINING_H
#define MINING_H

#include "blockchain.h"

#define MINING_REWARD 50
#define NUM_POOL_MINERS 4
#define POOL_FEE_RATE 0.02
#define CLOUD_REWARD_PER_ROUND 12
#define CLOUD_MAINTENANCE_FEE 2

extern int mining_difficulty;

void set_difficulty(int level);

long proof_of_work(Block *block);

void solo_mine();
void pool_mine();
void cloud_mine();

#endif
