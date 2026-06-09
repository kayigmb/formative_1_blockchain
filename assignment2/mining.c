#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crypto.h"
#include "mining.h"
#include "storage.h"
#include "transaction.h"

int mining_difficulty = 2;

void set_difficulty(int level) {

  if (level < 1 || level > 4) {

    printf("Difficulty must be between 1 and 4. Keeping %d.\n",
           mining_difficulty);
    return;
  }

  mining_difficulty = level;

  printf("Mining difficulty set to %d.\n", mining_difficulty);
}

long proof_of_work(Block *block) {

  char target[5];

  memset(target, '0', mining_difficulty);
  target[mining_difficulty] = '\0';

  long attempts = 0;

  block->nonce = 0;

  do {

    block->nonce++;
    calculate_hash(block, block->hash);
    attempts++;

  } while (strncmp(block->hash, target, mining_difficulty) != 0);

  return attempts;
}

static int confirm_all_pending(const char *miner_id, long *out_attempts) {

  int confirmed = 0;
  long total_attempts = 0;

  while (pending_head != NULL) {

    Block *block = pending_head;
    pending_head = pending_head->next;
    block->next = NULL;

    Block *tail = get_chain_tail();

    block->index = tail->index + 1;
    strcpy(block->previous_hash, tail->hash);

    long attempts = proof_of_work(block);
    total_attempts += attempts;

    sign_block(block);
    append_block(block);
    save_block(block);

    apply_reward(block->student_id, block->token_reward, block->transaction_id);

    printf("\n[CONFIRMED] Block #%d  %s (%s)\n", block->index,
           block->student_id, block->status);
    printf("  Token reward : %d\n", block->token_reward);
    printf("  Nonce found  : %lu\n", block->nonce);
    printf("  Hash         : %s\n", block->hash);
    printf("  Hash attempts: %ld\n", attempts);

    print_ledger_state();

    confirmed++;
  }

  *out_attempts = total_attempts;

  return confirmed;
}

void solo_mine() {

  if (pending_count() == 0) {

    printf("\nNo pending attendance blocks to mine.\n");
    return;
  }

  printf("\n========== SOLO MINING ==========\n");
  printf("Difficulty: %d\n", mining_difficulty);

  long attempts = 0;
  int blocks = confirm_all_pending("SOLO_MINER", &attempts);

  int reward = MINING_REWARD * blocks;

  printf("\n--------- SOLO SUMMARY ----------\n");
  printf("Blocks confirmed   : %d\n", blocks);
  printf("Total hash attempts: %ld\n", attempts);
  printf("Mining reward       : %d coins (to SOLO_MINER)\n", reward);
  printf("=================================\n");
}

void pool_mine() {

  if (pending_count() == 0) {

    printf("\nNo pending attendance blocks to mine.\n");
    return;
  }

  printf("\n========== POOL MINING ==========\n");
  printf("Difficulty: %d\n", mining_difficulty);

  long attempts = 0;
  int blocks = confirm_all_pending("POOL", &attempts);

  int block_reward = MINING_REWARD * blocks;

  int miner_attempts[NUM_POOL_MINERS];
  long total = 0;

  for (int i = 0; i < NUM_POOL_MINERS; i++) {

    miner_attempts[i] = (rand() % 900) + 100;
    total += miner_attempts[i];
  }

  double pool_fee = block_reward * POOL_FEE_RATE;
  double distributable = block_reward - pool_fee;

  printf("\n--------- POOL REWARD DISTRIBUTION ----------\n");
  printf("Total block reward : %d coins\n", block_reward);
  printf("Pool fee (2%%)      : %.2f coins\n", pool_fee);
  printf("Distributable      : %.2f coins\n\n", distributable);

  printf("%-10s %-10s %-10s %-10s\n", "MinerID", "Attempts", "Share%",
         "Reward");
  printf("------------------------------------------\n");

  for (int i = 0; i < NUM_POOL_MINERS; i++) {

    double share = (double)miner_attempts[i] / total;
    double reward = share * distributable;

    printf("MINER-%-4d %-10d %-9.2f %-10.2f\n", i + 1, miner_attempts[i],
           share * 100.0, reward);
  }

  printf("------------------------------------------\n");
  printf("Total attempts (simulated): %ld\n", total);
  printf("============================================\n");
}

void cloud_mine() {

  printf("\n========== CLOUD MINING ==========\n");

  if (pending_count() > 0) {

    printf("Renting hash power to confirm pending blocks...\n");

    long attempts = 0;
    int blocks = confirm_all_pending("CLOUD", &attempts);

    printf("\nConfirmed %d block(s) via rented power (%ld attempts).\n", blocks,
           attempts);

  } else {

    printf("No pending blocks.\n");
  }

  int rounds;
  int rental_fee;

  printf("\nEnter rental duration (1-5 rounds): ");
  scanf("%d", &rounds);

  if (rounds < 1 || rounds > 5) {

    printf("Duration must be 1-5. Aborting cloud mining.\n");
    return;
  }

  printf("Enter rental fee per round: ");
  scanf("%d", &rental_fee);

  int cum_reward = 0;
  int cum_fee = 0;

  printf("\n%-8s %-10s %-12s %-10s %-10s\n", "Round", "Gross", "Fees", "Net",
         "CumNet");
  printf("--------------------------------------------------------\n");

  for (int r = 1; r <= rounds; r++) {

    int gross = CLOUD_REWARD_PER_ROUND;
    int fees = rental_fee + CLOUD_MAINTENANCE_FEE;

    cum_reward += gross;
    cum_fee += fees;

    int cum_net = cum_reward - cum_fee;

    printf("%-8d %-10d %-12d %-10d %-10d\n", r, gross, fees, gross - fees,
           cum_net);

    if (cum_fee > cum_reward)
      printf("  WARNING: rental is UNPROFITABLE at round %d.\n", r);
  }

  printf("--------------------------------------------------------\n");
  printf("Gross earnings : %d coins\n", cum_reward);
  printf("Total fees paid: %d coins\n", cum_fee);
  printf("Net profit     : %d coins\n", cum_reward - cum_fee);

  if (cum_reward - cum_fee < 0)
    printf("Result         : LOSS (cloud rental not worth it)\n");
  else
    printf("Result         : PROFIT\n");

  printf("==================================\n");
}
