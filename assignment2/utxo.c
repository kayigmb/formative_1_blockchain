#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto.h"
#include "utxo.h"

UTXO *utxo_head = NULL;

static unsigned long txid_counter = 0;

static void add_utxo(const char *txid, int output_index, const char *owner,
                     int amount) {

  UTXO *u = malloc(sizeof(UTXO));

  if (u == NULL) {

    printf("Memory allocation failed.\n");
    exit(1);
  }

  strcpy(u->txid, txid);
  u->output_index = output_index;
  strcpy(u->owner, owner);
  u->amount = amount;
  u->spent = 0;
  u->next = utxo_head;

  utxo_head = u;
}

void utxo_reset() { utxo_head = NULL; }

int utxo_balance(const char *student_id) {

  int total = 0;

  UTXO *temp = utxo_head;

  while (temp != NULL) {

    if (!temp->spent && strcmp(temp->owner, student_id) == 0)
      total += temp->amount;

    temp = temp->next;
  }

  return total;
}

void utxo_apply_reward(const char *student_id, int amount, const char *txid) {

  add_utxo(txid, 0, student_id, amount);
}

int utxo_transfer(const char *from, const char *to, int amount, int fee) {

  int collected = 0;

  UTXO *temp = utxo_head;

  while (temp != NULL) {

    if (!temp->spent && strcmp(temp->owner, from) == 0) {

      collected += temp->amount;

      if (collected >= amount + fee)
        break;
    }

    temp = temp->next;
  }

  if (collected < amount + fee) {

    printf("TRANSACTION REJECTED: insufficient funds.\n");
    printf("Available: %d, Required (amount + fee): %d\n", collected,
           amount + fee);
    return 0;
  }

  collected = 0;

  temp = utxo_head;

  while (temp != NULL) {

    if (!temp->spent && strcmp(temp->owner, from) == 0) {

      temp->spent = 1;
      collected += temp->amount;

      if (collected >= amount + fee)
        break;
    }

    temp = temp->next;
  }

  char txid[HASH_SIZE];
  char tx_data[128];

  snprintf(tx_data, sizeof(tx_data), "%s%s%d%lu", from, to, amount,
           txid_counter++);

  sha256_string(tx_data, txid);

  add_utxo(txid, 0, to, amount);

  int change = collected - amount - fee;

  if (change > 0)
    add_utxo(txid, 1, from, change);

  printf("TRANSACTION CONFIRMED (UTXO).\n");
  printf("Inputs spent: %d, Sent: %d, Fee: %d, Change: %d\n", collected, amount,
         fee, change);
  printf("New txid: %s\n", txid);

  return 1;
}

void utxo_print_set() {

  printf("\n============== UTXO SET ==============\n");

  UTXO *temp = utxo_head;
  int total = 0;
  int found = 0;

  while (temp != NULL) {

    if (!temp->spent) {

      char short_id[13];
      strncpy(short_id, temp->txid, 12);
      short_id[12] = '\0';

      printf("txid %s.. out[%d] owner %-8s amount %d\n", short_id,
             temp->output_index, temp->owner, temp->amount);

      total += temp->amount;
      found = 1;
    }

    temp = temp->next;
  }

  if (!found)
    printf("(empty)\n");

  printf("======================================\n");
  printf("Total coins in circulation: %d\n", total);
  printf("======================================\n");
}

void utxo_free() {

  UTXO *u = utxo_head;

  while (u != NULL) {

    UTXO *next = u->next;
    free(u);
    u = next;
  }

  utxo_head = NULL;
}
