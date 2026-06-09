#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "blockchain.h"
#include "registry.h"
#include "utxo.h"

#define TX_FEE 1

typedef enum { MODEL_UTXO = 1, MODEL_ACCOUNT = 2 } LedgerModel;

extern LedgerModel current_model;

typedef struct TxRecord {

  char sender[20];
  char recipient[20];
  int amount;
  int fee;
  unsigned long nonce;

  struct TxRecord *next;

} TxRecord;

typedef struct Account {

  char student_id[20];
  int balance;
  unsigned long nonce;
  TxRecord *history;

} Account;

extern Account accounts[MAX_STUDENTS];
extern int account_count;

void ledger_init();
void select_model();
const char *model_name();

void apply_reward(const char *student_id, int gross_reward, const char *txid);

int transfer_tokens(const char *from, const char *to, int amount,
                    unsigned long nonce);

int get_balance(const char *student_id);

void print_balances();
void print_utxo_set();
void print_ledger_state();
void print_account_history(const char *student_id);

void free_ledger();

#endif
