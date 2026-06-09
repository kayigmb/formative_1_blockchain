#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "registry.h"
#include <time.h>

#define HASH_SIZE 65

typedef struct Block {

  int index;
  time_t timestamp;

  char student_id[20];
  char full_name[50];
  char course_code[10];
  char status[10];

  int token_reward;
  char transaction_id[HASH_SIZE];
  unsigned long nonce;

  char previous_hash[HASH_SIZE];
  char hash[HASH_SIZE];

  unsigned char signature[100];
  unsigned int signature_length;

  struct Block *next;

} Block;

extern Block *head;
extern Block *pending_head;

Block *create_genesis_block();

void mark_attendance();

void add_to_pending(Block *block);
int pending_count();
void view_pending();

Block *get_chain_tail();
void append_block(Block *new_block);

void view_records();
int validate_blockchain();
void tamper_demo();

int reward_for_status(const char *status);

#endif
