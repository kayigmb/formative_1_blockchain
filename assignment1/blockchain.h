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

  char previous_hash[HASH_SIZE];
  char hash[HASH_SIZE];

  unsigned char signature[72];
  unsigned int signature_length;

  struct Block *next;

} Block;

extern Block *head;

Block *create_genesis_block();

Block *create_block(int index, Student *student, const char *status,
                    const char *previous_hash);

void append_block(Block *new_block);

void view_records();

int validate_blockchain();

void tamper_demo();

void mark_attendance();

#endif
