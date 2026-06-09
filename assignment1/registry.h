#ifndef REGISTRY_H
#define REGISTRY_H

#define MAX_STUDENTS 100

typedef struct {
  char student_id[20];
  char full_name[50];
  char course_code[10];
} Student;

extern Student students[MAX_STUDENTS];
extern int student_count;

int load_students(const char *filename);

Student *find_student(const char *student_id);

#endif
