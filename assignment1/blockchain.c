#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blockchain.h"
#include "crypto.h"
#include "storage.h"

Block *head = NULL;

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

  memset(block->previous_hash, '0', 64);

  block->previous_hash[64] = '\0';

  calculate_hash(block, block->hash);

  block->signature_length = 0;

  block->next = NULL;

  return block;
}

Block *create_block(int index, Student *student, const char *status,
                    const char *previous_hash) {

  Block *block = malloc(sizeof(Block));

  if (block == NULL) {

    printf("Memory allocation failed.\n");
    exit(1);
  }

  block->index = index;

  block->timestamp = time(NULL);

  strcpy(block->student_id, student->student_id);
  strcpy(block->full_name, student->full_name);
  strcpy(block->course_code, student->course_code);

  strcpy(block->status, status);

  strcpy(block->previous_hash, previous_hash);

  calculate_hash(block, block->hash);

  sign_block(block);

  block->next = NULL;

  return block;
}

void append_block(Block *new_block) {

  Block *temp = head;

  while (temp->next != NULL) {

    temp = temp->next;
  }

  temp->next = new_block;
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

  Block *temp = head;

  while (temp->next != NULL) {

    temp = temp->next;
  }

  Block *new_block = create_block(temp->index + 1, student, status, temp->hash);

  append_block(new_block);

  save_block(new_block);

  printf("\nAttendance recorded successfully.\n");
}

void view_records() {

  Block *temp = head;

  printf("\n=========== RECORDS ===========\n");

  while (temp != NULL) {

    char *time_str = ctime(&temp->timestamp);

    time_str[strlen(time_str) - 1] = '\0';

    printf("\n--------------------------------\n");

    printf("Block Index : %d\n", temp->index);
    printf("Student ID  : %s\n", temp->student_id);
    printf("Full Name   : %s\n", temp->full_name);
    printf("Course Code : %s\n", temp->course_code);
    printf("Status      : %s\n", temp->status);
    printf("Timestamp   : %s\n", time_str);

    printf("Hash        : %s\n", temp->hash);

    if (temp->index != 0) {

      int valid = verify_signature(temp);

      printf("Signature   : %s\n", valid == 1 ? "VALID" : "INVALID");
    }

    printf("--------------------------------\n");

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
