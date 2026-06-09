#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blockchain.h"
#include "crypto.h"
#include "mining.h"
#include "registry.h"
#include "transaction.h"

void menu() {

  printf("\n=========================================\n");
  printf(" BLOCKCHAIN ATTENDANCE SYSTEM \n");
  printf("=========================================\n");
  printf("Model: %-13s | Difficulty: %d\n", model_name(), mining_difficulty);
  printf("=========================================\n");

  printf("1.  Mark Attendance\n");
  printf("2.  View Pending Pool\n");
  printf("3.  Solo Mining\n");
  printf("4.  Pool Mining\n");
  printf("5.  Cloud Mining\n");
  printf("6.  View Confirmed Records\n");
  printf("7.  View Balances / UTXO Set\n");
  printf("8.  Manual Token Transfer\n");
  printf("9.  Print Account History\n");
  printf("10. Validate Blockchain\n");
  printf("11. Demonstrate Tampering\n");
  printf("12. Set Mining Difficulty\n");
  printf("13. Select Transaction Model\n");
  printf("0. Exit\n");

  printf("=========================================\n");
  printf("Enter your Choice: ");
}

void do_transfer() {

  char from[20];
  char to[20];
  int amount;
  unsigned long nonce = 0;

  printf("\nSender Student ID   : ");
  scanf("%s", from);
  printf("Recipient Student ID: ");
  scanf("%s", to);
  printf("Amount              : ");
  scanf("%d", &amount);

  if (current_model == MODEL_ACCOUNT) {

    printf("Nonce               : ");
    scanf("%lu", &nonce);
  }

  transfer_tokens(from, to, amount, nonce);
}

int main(int argc, char *argv[]) {

  srand(time(NULL));

  if (argc > 1) {

    int level = atoi(argv[1]);
    set_difficulty(level);
  }

  if (!load_students("students.txt")) {

    return 1;
  }

  printf("Student registry loaded.\n");

  initialize_keys();

  ledger_init();

  head = create_genesis_block();

  printf("Genesis block created.\n");

  select_model();

  int choice;

  while (1) {

    menu();

    scanf("%d", &choice);

    switch (choice) {

    case 1:
      mark_attendance();
      break;

    case 2:
      view_pending();
      break;

    case 3:
      solo_mine();
      break;

    case 4:
      pool_mine();
      break;

    case 5:
      cloud_mine();
      break;

    case 6:
      view_records();
      break;

    case 7:
      print_ledger_state();
      break;

    case 8:
      do_transfer();
      break;

    case 9: {

      char id[20];
      printf("\nEnter Student ID: ");
      scanf("%s", id);
      print_account_history(id);
      break;
    }

    case 10:
      validate_blockchain();
      break;

    case 11:
      tamper_demo();
      break;

    case 12: {

      int level;
      printf("\nEnter difficulty (1-4): ");
      scanf("%d", &level);
      set_difficulty(level);
      break;
    }

    case 13:
      select_model();
      break;

    case 0:

      printf("\n CHAOO ADIOS -- BYEE...\n");
      free_ledger();
      cleanup_keys();
      return 0;

    default:
      printf("Invalid option.\n");
    }
  }

  free_ledger();
  cleanup_keys();
  return 0;
}
