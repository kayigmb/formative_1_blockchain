#include <stdio.h>

#include "blockchain.h"
#include "crypto.h"
#include "registry.h"

void menu() {

  printf("\n==================================\n");
  printf(" BLOCKCHAIN ATTENDANCE SYSTEM\n");
  printf("==================================\n");

  printf("1. Mark Attendance\n");
  printf("2. View Records\n");
  printf("3. Validate Blockchain\n");
  printf("4. Demonstrate Tampering\n");
  printf("5. Exit\n");

  printf("==================================\n");
  printf("Enter Choice: ");
}

int main() {

  if (!load_students("students.txt")) {

    return 1;
  }

  printf("Student registry loaded.\n");

  initialize_keys();

  head = create_genesis_block();

  printf("Genesis block created.\n");

  int choice;

  while (1) {

    menu();

    scanf("%d", &choice);

    switch (choice) {

    case 1:
      mark_attendance();
      break;

    case 2:
      view_records();
      break;

    case 3:
      validate_blockchain();
      break;

    case 4:
      tamper_demo();
      break;

    case 5:

      printf("\nExiting...\n");
      cleanup_keys();
      return 0;

    default:
      printf("Invalid option.\n");
    }
  }

  return 0;
}
