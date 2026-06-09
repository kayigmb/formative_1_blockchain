#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transaction.h"

LedgerModel current_model = MODEL_UTXO;

Account accounts[MAX_STUDENTS];
int account_count = 0;

void ledger_init() {

  account_count = student_count;

  for (int i = 0; i < student_count; i++) {

    strcpy(accounts[i].student_id, students[i].student_id);
    accounts[i].balance = 0;
    accounts[i].nonce = 0;
    accounts[i].history = NULL;
  }

  utxo_reset();
}

const char *model_name() {

  return current_model == MODEL_UTXO ? "UTXO" : "ACCOUNT-BASED";
}

void select_model() {

  int choice;

  printf("\nSelect Transaction Model:\n");
  printf("1. UTXO model\n");
  printf("2. Account-based model\n");
  printf("Enter choice: ");
  scanf("%d", &choice);

  if (choice == 2)
    current_model = MODEL_ACCOUNT;
  else
    current_model = MODEL_UTXO;

  printf("Active model: %s\n", model_name());
}

static Account *find_account(const char *student_id) {

  for (int i = 0; i < account_count; i++) {

    if (strcmp(accounts[i].student_id, student_id) == 0)
      return &accounts[i];
  }

  return NULL;
}

static void add_history(Account *acc, const char *sender, const char *recipient,
                        int amount, int fee, unsigned long nonce) {

  TxRecord *r = malloc(sizeof(TxRecord));

  if (r == NULL) {

    printf("Memory allocation failed.\n");
    exit(1);
  }

  strcpy(r->sender, sender);
  strcpy(r->recipient, recipient);
  r->amount = amount;
  r->fee = fee;
  r->nonce = nonce;
  r->next = NULL;

  if (acc->history == NULL) {

    acc->history = r;
    return;
  }

  TxRecord *temp = acc->history;

  while (temp->next != NULL)
    temp = temp->next;

  temp->next = r;
}

int get_balance(const char *student_id) {

  if (current_model == MODEL_UTXO)
    return utxo_balance(student_id);

  Account *acc = find_account(student_id);

  return acc == NULL ? 0 : acc->balance;
}

void apply_reward(const char *student_id, int gross_reward, const char *txid) {

  if (gross_reward <= 0)
    return;

  if (current_model == MODEL_UTXO) {

    utxo_apply_reward(student_id, gross_reward, txid);
    return;
  }

  Account *acc = find_account(student_id);

  if (acc == NULL)
    return;

  acc->balance += gross_reward;

  add_history(acc, "COINBASE", student_id, gross_reward, 0, 0);
}

static int transfer_account(const char *from, const char *to, int amount,
                            unsigned long nonce) {

  Account *sender = find_account(from);
  Account *recipient = find_account(to);

  if (sender == NULL) {

    printf("TRANSACTION REJECTED: sender account not found.\n");
    return 0;
  }

  if (recipient == NULL) {

    printf("TRANSACTION REJECTED: recipient account not found.\n");
    return 0;
  }

  if (nonce != sender->nonce) {

    printf("TRANSACTION REJECTED: invalid nonce.\n");
    printf("Expected nonce: %lu, Provided: %lu\n", sender->nonce, nonce);
    return 0;
  }

  if (sender->balance < amount + TX_FEE) {

    printf("TRANSACTION REJECTED: insufficient balance.\n");
    printf("Balance: %d, Required (amount + fee): %d\n", sender->balance,
           amount + TX_FEE);
    return 0;
  }

  sender->balance -= (amount + TX_FEE);
  recipient->balance += amount;

  add_history(sender, from, to, amount, TX_FEE, sender->nonce);
  add_history(recipient, from, to, amount, TX_FEE, sender->nonce);

  sender->nonce++;

  printf("TRANSACTION CONFIRMED (ACCOUNT).\n");
  printf("From: %s, To: %s, Amount: %d, Fee: %d, Nonce used: %lu\n", from, to,
         amount, TX_FEE, sender->nonce - 1);

  return 1;
}

int transfer_tokens(const char *from, const char *to, int amount,
                    unsigned long nonce) {

  if (amount <= 0) {

    printf("---> REJECTED: amount must be positive.\n");
    return 0;
  }

  if (current_model == MODEL_UTXO)
    return utxo_transfer(from, to, amount, TX_FEE);

  return transfer_account(from, to, amount, nonce);
}

void print_balances() {

  printf("\n=========== STUDENT BALANCES (%s) ===========\n", model_name());

  for (int i = 0; i < account_count; i++) {

    if (current_model == MODEL_ACCOUNT)
      printf("%-8s : %4d coins   (nonce %lu)\n", accounts[i].student_id,
             accounts[i].balance, accounts[i].nonce);
    else
      printf("%-8s : %4d coins\n", accounts[i].student_id,
             get_balance(accounts[i].student_id));
  }

  printf("=================================================\n");
}

void print_ledger_state() {

  if (current_model == MODEL_UTXO)
    utxo_print_set();

  print_balances();
}

void print_account_history(const char *student_id) {

  if (current_model != MODEL_ACCOUNT) {

    printf("\nTransaction history is only tracked in the ACCOUNT model.\n");
    return;
  }

  Account *acc = find_account(student_id);

  if (acc == NULL) {

    printf("\nNo account found for %s.\n", student_id);
    return;
  }

  printf("\n===== TRANSACTION HISTORY: %s =====\n", student_id);

  if (acc->history == NULL) {

    printf("(no transactions)\n");
    printf("====================================\n");
    return;
  }

  TxRecord *temp = acc->history;

  while (temp != NULL) {

    printf("%-8s -> %-8s | amount %4d | fee %d | nonce %lu\n", temp->sender,
           temp->recipient, temp->amount, temp->fee, temp->nonce);

    temp = temp->next;
  }

  printf("====================================\n");
}

void free_ledger() {

  utxo_free();

  for (int i = 0; i < account_count; i++) {

    TxRecord *r = accounts[i].history;

    while (r != NULL) {

      TxRecord *next = r->next;
      free(r);
      r = next;
    }

    accounts[i].history = NULL;
  }
}
