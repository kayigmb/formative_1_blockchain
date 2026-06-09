#include "blockchain.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Block *head = NULL;
Block *pending_head = NULL;

int reward_for_status(const char *status) {

  if (strcmp(status, "PRESENT") == 0)
    return 10;

  if (strcmp(status, "LATE") == 0)
    return 5;

  return 0;
}

Block *create_genesis_block() {

  Block *block = malloc(sizeof(Block));

  if (block == NULL) {

    printf("Memory allocation failed.\n");
    exit(1);
  }

  block->index = 0;

  block->timestamp = time(NULL);

  strcpy(block->student_id, "GENESIS");
  strcpy(block->full_name, "GENESIS");
  strcpy(block->course_code, "NONE");
  strcpy(block->status, "NONE");

  block->token_reward = 0;
  strcpy(block->transaction_id, "GENESIS");
  block->nonce = 0;

  memset(block->previous_hash, '0', 64);

  block->previous_hash[64] = '\0';

  calculate_hash(block, block->hash);

  block->signature_length = 0;

  block->next = NULL;

  return block;
}

Block *get_chain_tail() {

  Block *temp = head;

  while (temp->next != NULL) {

    temp = temp->next;
  }

  return temp;
}

void append_block(Block *new_block) {

  Block *tail = get_chain_tail();

  tail->next = new_block;
}

void add_to_pending(Block *block) {

  if (pending_head == NULL) {

    pending_head = block;
    return;
  }

  Block *temp = pending_head;

  while (temp->next != NULL) {

    temp = temp->next;
  }

  temp->next = block;
}

int pending_count() {

  int count = 0;

  Block *temp = pending_head;

  while (temp != NULL) {

    count++;
    temp = temp->next;
  }

  return count;
}

void mark_attendance() {

  char student_id[20];
  char status[10];

  printf("\nEnter Student ID: ");
  scanf("%s", student_id);

  Student *student = find_student(student_id);

  if (student == NULL) {

    printf("ERROR: Student ID not found.\n");
    return;
  }

  printf("Enter Status (PRESENT/ABSENT/LATE): ");
  scanf("%s", status);

  Block *block = malloc(sizeof(Block));

  if (block == NULL) {

    printf("Memory allocation failed.\n");
    exit(1);
  }

  block->index = -1;
  block->timestamp = time(NULL);

  strcpy(block->student_id, student->student_id);
  strcpy(block->full_name, student->full_name);
  strcpy(block->course_code, student->course_code);
  strcpy(block->status, status);

  block->token_reward = reward_for_status(status);
  block->nonce = 0;

  if (block->token_reward > 0) {

    char tx_data[256];

    snprintf(tx_data, sizeof(tx_data), "%s%s%d%ld", block->student_id,
             block->status, block->token_reward, (long)block->timestamp);

    sha256_string(tx_data, block->transaction_id);

  } else {

    strcpy(block->transaction_id, "NO_TRANSACTION");
  }

  block->previous_hash[0] = '\0';
  block->hash[0] = '\0';
  block->signature_length = 0;
  block->next = NULL;

  add_to_pending(block);

  printf("\nAttendance queued in PENDING POOL (awaiting mining).\n");
  printf("Status        : %s\n", block->status);
  printf("Token Reward  : %d\n", block->token_reward);
  printf("Transaction ID: %s\n", block->transaction_id);

  if (block->token_reward == 0)
    printf("Note          : ABSENT generates no token transaction.\n");
}

void view_pending() {

  printf("\n========= PENDING POOL =========\n");

  if (pending_head == NULL) {

    printf("No pending attendance blocks.\n");
    printf("================================\n");
    return;
  }

  Block *temp = pending_head;

  while (temp != NULL) {

    char *time_str = ctime(&temp->timestamp);
    time_str[strlen(time_str) - 1] = '\0';

    printf("\n--------------------------------\n");
    printf("Student ID    : %s\n", temp->student_id);
    printf("Full Name     : %s\n", temp->full_name);
    printf("Course Code   : %s\n", temp->course_code);
    printf("Status        : %s\n", temp->status);
    printf("Timestamp     : %s\n", time_str);
    printf("Token Reward  : %d\n", temp->token_reward);
    printf("Transaction ID: %s\n", temp->transaction_id);
    printf("State         : UNCONFIRMED\n");

    temp = temp->next;
  }

  printf("--------------------------------\n");
  printf("Total pending : %d\n", pending_count());
}

void view_records() {

  Block *temp = head;

  printf("\n=========== CONFIRMED RECORDS ===========\n");

  while (temp != NULL) {

    char *time_str = ctime(&temp->timestamp);

    time_str[strlen(time_str) - 1] = '\0';

    printf("\n----------------------------------------\n");

    printf("Block Index   : %d\n", temp->index);
    printf("Student ID    : %s\n", temp->student_id);
    printf("Full Name     : %s\n", temp->full_name);
    printf("Course Code   : %s\n", temp->course_code);
    printf("Status        : %s\n", temp->status);
    printf("Timestamp     : %s\n", time_str);
    printf("Token Reward  : %d\n", temp->token_reward);
    printf("Transaction ID: %s\n", temp->transaction_id);
    printf("Nonce         : %lu\n", temp->nonce);
    printf("Hash          : %s\n", temp->hash);

    if (temp->index != 0) {

      int valid = verify_signature(temp);

      printf("Signature     : %s\n", valid == 1 ? "VALID" : "INVALID");
    }

    printf("----------------------------------------\n");

    temp = temp->next;
  }
}

int validate_blockchain() {

  Block *current = head->next;
  Block *previous = head;

  while (current != NULL) {

    char recalculated_hash[HASH_SIZE];

    calculate_hash(current, recalculated_hash);

    if (strcmp(recalculated_hash, current->hash) != 0) {

      printf("\nBLOCKCHAIN INVALID.\n");
      printf("Hash mismatch detected.\n");

      return 0;
    }

    if (strcmp(current->previous_hash, previous->hash) != 0) {

      printf("\nBLOCKCHAIN INVALID.\n");
      printf("Broken linkage detected.\n");

      return 0;
    }

    if (!verify_signature(current)) {

      printf("\nBLOCKCHAIN INVALID.\n");
      printf("Signature verification failed.\n");

      return 0;
    }

    previous = current;
    current = current->next;
  }

  printf("\nBLOCKCHAIN VALID.\n");

  return 1;
}

void tamper_demo() {

  if (head->next == NULL) {

    printf("\nNo blocks to tamper with.\n");
    return;
  }

  printf("\nTampering with block...\n");

  strcpy(head->next->status, "ABSENT");

  printf("Block modified.\n");
}
