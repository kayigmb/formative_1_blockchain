#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "registry.h"

Student students[MAX_STUDENTS];
int student_count = 0;

int load_students(const char *filename) {

  FILE *file = fopen(filename, "r");

  if (file == NULL) {

    printf("ERROR: students.txt not found.\n");
    return 0;
  }

  char line[200];

  while (fgets(line, sizeof(line), file)) {

    if (strlen(line) <= 1)
      continue;

    char *token;

    token = strtok(line, ",");

    if (token == NULL)
      continue;

    strcpy(students[student_count].student_id, token);

    token = strtok(NULL, ",");

    if (token == NULL)
      continue;

    strcpy(students[student_count].full_name, token);

    token = strtok(NULL, ",\n");

    if (token == NULL)
      continue;

    strcpy(students[student_count].course_code, token);

    student_count++;
  }

  fclose(file);

  if (student_count == 0) {

    printf("ERROR: students.txt is empty.\n");
    return 0;
  }

  return 1;
}

Student *find_student(const char *student_id) {

  for (int i = 0; i < student_count; i++) {
    if (strcmp(students[i].student_id, student_id) == 0) {

      return &students[i];
    }
  }

  return NULL;
}
