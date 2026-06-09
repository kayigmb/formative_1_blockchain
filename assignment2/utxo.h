#ifndef UTXO_H
#define UTXO_H

#include "blockchain.h"

typedef struct UTXO {

  char txid[HASH_SIZE];
  int output_index;
  char owner[20];
  int amount;
  int spent;

  struct UTXO *next;

} UTXO;

extern UTXO *utxo_head;

void utxo_reset();

void utxo_apply_reward(const char *student_id, int amount, const char *txid);

int utxo_transfer(const char *from, const char *to, int amount, int fee);

int utxo_balance(const char *student_id);

void utxo_print_set();

void utxo_free();

#endif
