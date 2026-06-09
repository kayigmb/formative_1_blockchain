#include "storage.h"
#include <stdio.h>

void save_block(Block *block) {

  FILE *file = fopen("ledger.txt", "a");

  if (!file) {
    printf("ERROR: Could not open ledger.txt\n");
    return;
  }

  fprintf(file, "%d|%ld|%s|%s|%s|%s|%d|%s|%lu|%s|%s\n", block->index,
          (long)block->timestamp, block->student_id, block->full_name,
          block->course_code, block->status, block->token_reward,
          block->transaction_id, block->nonce, block->previous_hash,
          block->hash);

  fclose(file);
}
