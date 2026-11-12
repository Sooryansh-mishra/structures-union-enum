

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 49           
#define MAX_CAPACITY 100000   

typedef struct {
    int roll;
    char name[MAX_NAME + 1];
    int age;
    float gpa;
} Student;


int read_line(char *line, int size) {
    if (fgets(line, size, stdin) == NULL) {
        return -1;                       // input ended
    }
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';            // remove the newline
        return 1;
    }
   
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
    return 0;
}


int get_int(char *message, int min, int max, int *result) {
    char line[100];
    int number;
    char extra;

    while (1) {
        printf("%s", message);
        int r = read_line(line, 100);
        if (r == -1) {
            return -1;
        }
        if (r == 0 || sscanf(line, "%d %c", &number, &extra) != 1) {
            printf("Error: please enter a whole number.\n");
        } else if (number < min || number > max) {
            printf("Error: number must be between %d and %d.\n", min, max);
        } else {
            *result = number;
            return 1;
        }
    }
}

int get_gpa(char *message, float *result) {
    char line[100];
    float number;
    char extra;

    while (1) {
        printf("%s", message);
        int r = read_line(line, 100);
        if (r == -1) {
            return -1;
        }
        if (r == 0 || sscanf(line, "%f %c", &number, &extra) != 1) {
            printf("Error: please enter a number like 3.5\n");
        } else if (number < 0.0 || number > 4.0) {
            printf("Error: GPA must be between 0.00 and 4.00.\n");
        } else {
            *result = number;
            return 1;
        }
    }
}

int get_name(char *message, char *name) {
    char line[200];

    while (1) {
        printf("%s", message);
        int r = read_line(line, 200);
        if (r == -1) {
            return -1;
        }
        if (r == 0 || strlen(line) > MAX_NAME) {
            printf("Error: name must be at most %d characters.\n", MAX_NAME);
        } else if (strlen(line) == 0) {
            printf("Error: name cannot be empty.\n");
        } else {
            strcpy(name, line);
            return 1;
        }
    }
}


Student* create_student_list(int n) {
    if (n <= 0 || n > MAX_CAPACITY) {
        return NULL;
    }
    Student *studs = (Student*) malloc(n * sizeof(Student));
    return studs;
}

void add_student(Student *studs, int i, int roll, char *name, int age, float gpa) {
    studs[i].roll = roll;
    strcpy(studs[i].name, name);   
    studs[i].age = age;
    studs[i].gpa = gpa;
}

void display_students(Student *students, int size) {
    for (int i = 0; i < size; i++) {
        printf("Roll: %d, Name: %s, Age: %d, GPA: %.2f\n", students[i].roll,
               students[i].name, students[i].age, students[i].gpa);
    }
}

void free_student_list(Student *students) {
    free(students);
}




int find_student(Student *studs, int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (studs[i].roll == roll) {
            return i;
        }
    }
    return -1;
}



int grow_list(Student **studs, int *capacity) {
    if (*capacity >= MAX_CAPACITY) {
        return 0;
    }
    int new_capacity = *capacity * 2;
    if (new_capacity > MAX_CAPACITY) {
        new_capacity = MAX_CAPACITY;
    }
   
    Student *temp = (Student*) realloc(*studs, new_capacity * sizeof(Student));
    if (temp == NULL) {
        return 0;
    }
    *studs = temp;
    *capacity = new_capacity;
    return 1;
}


int add_new_student(Student **studs, int *count, int *capacity) {
    int roll, age;
    float gpa;
    char name[MAX_NAME + 1];

    if (get_int("Enter roll number: ", 1, 2000000000, &roll) == -1) return -1;
    if (find_student(*studs, *count, roll) != -1) {
        printf("Error: roll number %d already exists.\n", roll);
        return 0;
    }
    if (get_name("Enter name: ", name) == -1) return -1;
    if (get_int("Enter age (16-100): ", 16, 100, &age) == -1) return -1;
    if (get_gpa("Enter GPA (0.00-4.00): ", &gpa) == -1) return -1;


    if (*count == *capacity) {
        if (grow_list(studs, capacity) == 0) {
            printf("Error: array is full and could not be made bigger.\n");
            return 0;
        }
        printf("Array was full, capacity is now %d.\n", *capacity);
    }

    add_student(*studs, *count, roll, name, age, gpa);
    (*count)++;
    printf("Student %d added successfully.\n", roll);
    return 0;
}

int search_student(Student *studs, int count) {
    int roll;
    if (get_int("Enter roll number to search: ", 1, 2000000000, &roll) == -1) return -1;

    int pos = find_student(studs, count, roll);
    if (pos == -1) {
        printf("Error: roll number %d not found.\n", roll);
    } else {
        display_students(&studs[pos], 1);
    }
    return 0;
}

int update_student(Student *studs, int count) {
    int roll, age;
    float gpa;
    char name[MAX_NAME + 1];

    if (get_int("Enter roll number to update: ", 1, 2000000000, &roll) == -1) return -1;
    int pos = find_student(studs, count, roll);
    if (pos == -1) {
        printf("Error: roll number %d not found.\n", roll);
        return 0;
    }

    if (get_name("Enter new name: ", name) == -1) return -1;
    if (get_int("Enter new age (16-100): ", 16, 100, &age) == -1) return -1;
    if (get_gpa("Enter new GPA (0.00-4.00): ", &gpa) == -1) return -1;

    add_student(studs, pos, roll, name, age, gpa);
    printf("Student %d updated successfully.\n", roll);
    return 0;
}

int delete_student(Student *studs, int *count) {
    int roll;
    if (get_int("Enter roll number to delete: ", 1, 2000000000, &roll) == -1) return -1;

    int pos = find_student(studs, *count, roll);
    if (pos == -1) {
        printf("Error: roll number %d not found.\n", roll);
        return 0;
    }

    for (int i = pos; i < *count - 1; i++) {
        studs[i] = studs[i + 1];
    }
    (*count)--;
    printf("Student %d deleted successfully.\n", roll);
    return 0;
}

void show_summary(Student *studs, int count) {
    if (count == 0) {
        printf("No students yet, so there is no highest, lowest or average GPA.\n");
        return;
    }

    int highest = 0;
    int lowest = 0;
    float total = 0;

    for (int i = 0; i < count; i++) {
        if (studs[i].gpa > studs[highest].gpa) highest = i;
        if (studs[i].gpa < studs[lowest].gpa) lowest = i;
        total = total + studs[i].gpa;
    }

    printf("Highest GPA: %.2f (%s, roll %d)\n", studs[highest].gpa,
           studs[highest].name, studs[highest].roll);
    printf("Lowest GPA: %.2f (%s, roll %d)\n", studs[lowest].gpa,
           studs[lowest].name, studs[lowest].roll);
    printf("Average GPA: %.2f\n", total / count);
}

void show_sorted(Student *studs, int count) {
    if (count == 0) {
        printf("No students to sort.\n");
        return;
    }

    Student *copy = (Student*) malloc(count * sizeof(Student));
    if (copy == NULL) {
        printf("Error: not enough memory to sort.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        copy[i] = studs[i];
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            int swap = 0;
            if (copy[j].gpa < copy[j + 1].gpa) {
                swap = 1;
            } else if (copy[j].gpa == copy[j + 1].gpa && copy[j].roll > copy[j + 1].roll) {
                swap = 1;
            }
            if (swap) {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("Students sorted by GPA (highest first):\n");
    display_students(copy, count);
    free(copy);
}

void run_original_demo() {
    int number_of_students = 3;
    Student *student_list = create_student_list(number_of_students);
    if (student_list == NULL) {
        printf("Error: could not allocate memory.\n");
        return;
    }

    add_student(student_list, 0, 1, "Aladdin", 20, 3.5);
    add_student(student_list, 1, 2, "Batman", 21, 3.7);
    add_student(student_list, 2, 3, "Catwoman", 22, 3.8);

    display_students(student_list, number_of_students);

    free_student_list(student_list);
}

void show_menu() {
    printf("\n----- Student Records -----\n");
    printf("1. Add student\n");
    printf("2. Display all students\n");
    printf("3. Search by roll number\n");
    printf("4. Update student\n");
    printf("5. Delete student\n");
    printf("6. GPA summary\n");
    printf("7. Display sorted by GPA\n");
    printf("8. Run original demo\n");
    printf("0. Exit\n");
}

int main() {
    int capacity;
    int count = 0;
    Student *students = NULL;

    
    while (students == NULL) {
        if (get_int("Enter initial capacity: ", -2000000000, 2000000000, &capacity) == -1) {
            return 0;
        }
        if (capacity <= 0) {
            printf("Error: capacity must be at least 1.\n");
        } else if (capacity > MAX_CAPACITY) {
            printf("Error: capacity is too large (max %d).\n", MAX_CAPACITY);
        } else {
            students = create_student_list(capacity);
            if (students == NULL) {
                printf("Error: memory allocation failed.\n");
            }
        }
    }
    printf("Space for %d students allocated.\n", capacity);

    int choice = -1;
    int result = 0;
    while (choice != 0 && result != -1) {
        show_menu();
        if (get_int("Enter your choice: ", 0, 8, &choice) == -1) {
            break;
        }

        if (choice == 1) {
            result = add_new_student(&students, &count, &capacity);
        } else if (choice == 2) {
            if (count == 0) {
                printf("No students to display.\n");
            } else {
                display_students(students, count);
            }
        } else if (choice == 3) {
            result = search_student(students, count);
        } else if (choice == 4) {
            result = update_student(students, count);
        } else if (choice == 5) {
            result = delete_student(students, &count);
        } else if (choice == 6) {
            show_summary(students, count);
        } else if (choice == 7) {
            show_sorted(students, count);
        } else if (choice == 8) {
            run_original_demo();
        }
    }

    free_student_list(students);
    printf("Memory freed. Bye!\n");
    return 0;
}
