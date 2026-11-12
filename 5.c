#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_START 1000
#define MAX_CAPACITY 100000
#define MAX_DIMENSION 1000000.0
#define TOLERANCE 0.0001

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

// ---------------- input functions ----------------

// reads one line, returns 1 if ok, 0 if too long, -1 if input ended
int read_line(char *line, int size) {
    if (fgets(line, size, stdin) == NULL) {
        return -1;
    }
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        return 1;
    }
    // line was too long, throw the rest away
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
    return 0;
}

// asks for a whole number between min and max until it is valid
// returns 1 on success, -1 if the input ended
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
        // "%d %c" gives 2 if there are extra letters after the number
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

// asks for a decimal number that is finite and between min and max
// (nan and inf are rejected here)
int get_float(char *message, float min, float max, float *result) {
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
            printf("Error: please enter a number like 4.5\n");
        } else if (isnan(number) || isinf(number)) {
            printf("Error: NaN and infinity are not allowed.\n");
        } else if (number < min || number > max) {
            printf("Error: value must be between %g and %g.\n", min, max);
        } else {
            *result = number;
            return 1;
        }
    }
}

// width and height: must be greater than 0 (0.000001 is the smallest allowed)
int get_dimension(char *message, float *result) {
    return get_float(message, 0.000001, MAX_DIMENSION, result);
}

// ---------------- original functions (from the first version) ----------------

void calculate_area(Rectangle *r) {
    r->area = r->width * r->height;
}

// new: perimeter = 2 * (width + height)
void calculate_perimeter(Rectangle *r) {
    r->perimeter = 2 * (r->width + r->height);
}

Rectangle* create_rectangle_array(int size) {
    if (size <= 0 || size > MAX_CAPACITY) {
        return NULL;
    }
    Rectangle *rectangles = (Rectangle*) malloc(size * sizeof(Rectangle));
    return rectangles;
}

// sets id, width and height and recalculates area and perimeter,
// so the fields always match each other
void add_rectangle(Rectangle *rectangles, int index, int id, float width, float height) {
    rectangles[index].id = id;
    rectangles[index].width = width;
    rectangles[index].height = height;
    calculate_area(&rectangles[index]);
    calculate_perimeter(&rectangles[index]);
}

// 1 if the rectangle is a square (using the tolerance), otherwise 0
int is_square(Rectangle *r) {
    if (fabs(r->width - r->height) < TOLERANCE) {
        return 1;
    }
    return 0;
}

void display_rectangles(Rectangle *rectangles, int size) {
    for (int i = 0; i < size; i++) {
        printf("ID %d: Width = %.2f, Height = %.2f, Area = %.2f, Perimeter = %.2f, %s\n",
               rectangles[i].id, rectangles[i].width, rectangles[i].height,
               rectangles[i].area, rectangles[i].perimeter,
               is_square(&rectangles[i]) ? "Square" : "Not a square");
    }
}

void free_rectangle_array(Rectangle *rectangles) {
    free(rectangles);
}

// ---------------- new functions ----------------

// returns the position of the id, or -1 if it is not there
int find_rectangle(Rectangle *rects, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (rects[i].id == id) {
            return i;
        }
    }
    return -1;
}

// changes the size of the array to new_capacity using realloc
// returns 1 if it worked, 0 if not (then the old array is still fine)
int resize_list(Rectangle **rects, int *capacity, int new_capacity) {
    if (new_capacity < 1 || new_capacity > MAX_CAPACITY) {
        return 0;
    }
    // temporary pointer so we do not lose the old array if realloc fails
    Rectangle *temp = (Rectangle*) realloc(*rects, new_capacity * sizeof(Rectangle));
    if (temp == NULL) {
        return 0;
    }
    *rects = temp;
    *capacity = new_capacity;
    return 1;
}

// returns -1 if the input ended, otherwise 0
int add_new_rectangle(Rectangle **rects, int *count, int *capacity) {
    int id;
    float width, height;

    if (get_int("Enter rectangle ID: ", 1, 2000000000, &id) == -1) return -1;
    if (find_rectangle(*rects, *count, id) != -1) {
        printf("Error: ID %d already exists.\n", id);
        return 0;
    }
    if (get_dimension("Enter width: ", &width) == -1) return -1;
    if (get_dimension("Enter height: ", &height) == -1) return -1;

    // if the array is full, make it twice as big
    if (*count == *capacity) {
        if (resize_list(rects, capacity, *capacity * 2) == 0 &&
            resize_list(rects, capacity, *capacity + 1) == 0) {
            printf("Error: array is full and could not be made bigger.\n");
            return 0;
        }
        printf("Array was full, capacity is now %d.\n", *capacity);
    }

    add_rectangle(*rects, *count, id, width, height);
    (*count)++;
    printf("Rectangle %d added successfully.\n", id);
    return 0;
}

int search_rectangle(Rectangle *rects, int count) {
    int id;
    if (get_int("Enter ID to search: ", 1, 2000000000, &id) == -1) return -1;

    int pos = find_rectangle(rects, count, id);
    if (pos == -1) {
        printf("Error: ID %d not found.\n", id);
    } else {
        display_rectangles(&rects[pos], 1);
    }
    return 0;
}

int update_rectangle(Rectangle *rects, int count) {
    int id;
    float width, height;

    if (get_int("Enter ID to update: ", 1, 2000000000, &id) == -1) return -1;
    int pos = find_rectangle(rects, count, id);
    if (pos == -1) {
        printf("Error: ID %d not found.\n", id);
        return 0;
    }
    if (get_dimension("Enter new width: ", &width) == -1) return -1;
    if (get_dimension("Enter new height: ", &height) == -1) return -1;

    // add_rectangle recalculates area and perimeter
    add_rectangle(rects, pos, id, width, height);
    printf("Rectangle %d updated successfully.\n", id);
    return 0;
}

int delete_rectangle(Rectangle **rects, int *count, int *capacity) {
    int id;
    if (get_int("Enter ID to delete: ", 1, 2000000000, &id) == -1) return -1;

    int pos = find_rectangle(*rects, *count, id);
    if (pos == -1) {
        printf("Error: ID %d not found.\n", id);
        return 0;
    }

    // move every rectangle after pos one place to the left
    for (int i = pos; i < *count - 1; i++) {
        (*rects)[i] = (*rects)[i + 1];
    }
    (*count)--;
    printf("Rectangle %d deleted successfully.\n", id);

    // if the array is mostly empty, make it smaller (if this fails it is ok,
    // the old array is still there)
    if (*capacity > 4 && *count <= *capacity / 4) {
        if (resize_list(rects, capacity, *capacity / 2)) {
            printf("Capacity reduced to %d.\n", *capacity);
        }
    }
    return 0;
}

void show_summary(Rectangle *rects, int count) {
    if (count == 0) {
        printf("No rectangles, so there is no largest, smallest, total or average area.\n");
        return;
    }

    int largest = 0;
    int smallest = 0;
    float total = 0;

    for (int i = 0; i < count; i++) {
        if (rects[i].area > rects[largest].area) largest = i;
        if (rects[i].area < rects[smallest].area) smallest = i;
        total = total + rects[i].area;
    }

    printf("Largest area: %.2f (ID %d)\n", rects[largest].area, rects[largest].id);
    printf("Smallest area: %.2f (ID %d)\n", rects[smallest].area, rects[smallest].id);
    printf("Total area: %.2f\n", total);
    printf("Average area: %.2f\n", total / count);
}

// sorts a copy: smallest area first, if the area is the same then smaller ID first
void show_sorted(Rectangle *rects, int count) {
    if (count == 0) {
        printf("No rectangles to sort.\n");
        return;
    }

    Rectangle *copy = (Rectangle*) malloc(count * sizeof(Rectangle));
    if (copy == NULL) {
        printf("Error: not enough memory to sort.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        copy[i] = rects[i];
    }

    // bubble sort
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            int swap = 0;
            if (copy[j].area > copy[j + 1].area) {
                swap = 1;
            } else if (copy[j].area == copy[j + 1].area && copy[j].id > copy[j + 1].id) {
                swap = 1;
            }
            if (swap) {
                Rectangle temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("Rectangles sorted by area (smallest first):\n");
    display_rectangles(copy, count);
    free(copy);
}

// shows only the rectangles with area bigger than a number the user types
int filter_by_area(Rectangle *rects, int count) {
    float threshold;
    // area can be at most MAX_DIMENSION * MAX_DIMENSION
    if (get_float("Enter area threshold: ", 0, MAX_DIMENSION * MAX_DIMENSION, &threshold) == -1) {
        return -1;
    }

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (rects[i].area > threshold) {
            display_rectangles(&rects[i], 1);
            found++;
        }
    }
    if (found == 0) {
        printf("No rectangle has an area greater than %.2f.\n", threshold);
    }
    return 0;
}

// the original program from Problem A
void run_original_demo() {
    int number_of_rectangles = 2;
    Rectangle *rectangles = create_rectangle_array(number_of_rectangles);
    if (rectangles == NULL) {
        printf("Error: could not allocate memory.\n");
        return;
    }

    add_rectangle(rectangles, 0, 1, 5.0, 4.0);
    add_rectangle(rectangles, 1, 2, 6.5, 3.5);

    display_rectangles(rectangles, number_of_rectangles);

    free_rectangle_array(rectangles);
}

void show_menu() {
    printf("\n----- Rectangle Inventory -----\n");
    printf("1. Add rectangle\n");
    printf("2. Display all rectangles\n");
    printf("3. Search by ID\n");
    printf("4. Update dimensions by ID\n");
    printf("5. Delete by ID\n");
    printf("6. Area summary (largest, smallest, total, average)\n");
    printf("7. Display sorted by area\n");
    printf("8. Filter by area threshold\n");
    printf("9. Run original demo\n");
    printf("0. Exit\n");
}

int main() {
    int capacity;
    int count = 0;
    Rectangle *rectangles = NULL;

    // ask for the starting size until we manage to allocate it
    while (rectangles == NULL) {
        if (get_int("Enter number of rectangles (1-1000): ", -2000000000, 2000000000, &capacity) == -1) {
            return 0;
        }
        if (capacity < 1 || capacity > MAX_START) {
            printf("Error: number must be between 1 and %d.\n", MAX_START);
        } else {
            rectangles = create_rectangle_array(capacity);
            if (rectangles == NULL) {
                printf("Error: memory allocation failed.\n");
            }
        }
    }
    printf("Space for %d rectangles allocated.\n", capacity);

    int choice = -1;
    int result = 0;
    while (choice != 0 && result != -1) {
        show_menu();
        if (get_int("Enter your choice: ", 0, 9, &choice) == -1) {
            break;
        }

        if (choice == 1) {
            result = add_new_rectangle(&rectangles, &count, &capacity);
        } else if (choice == 2) {
            if (count == 0) {
                printf("No rectangles to display.\n");
            } else {
                display_rectangles(rectangles, count);
            }
        } else if (choice == 3) {
            result = search_rectangle(rectangles, count);
        } else if (choice == 4) {
            result = update_rectangle(rectangles, count);
        } else if (choice == 5) {
            result = delete_rectangle(&rectangles, &count, &capacity);
        } else if (choice == 6) {
            show_summary(rectangles, count);
        } else if (choice == 7) {
            show_sorted(rectangles, count);
        } else if (choice == 8) {
            result = filter_by_area(rectangles, count);
        } else if (choice == 9) {
            run_original_demo();
        }
    }

    free_rectangle_array(rectangles);
    printf("Memory freed. Bye!\n");
    return 0;
}
