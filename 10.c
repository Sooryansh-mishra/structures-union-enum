// Container that can hold one value of different types (int, float, char, string)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum to remember which type is stored (TYPE_EMPTY = nothing stored)
typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

// Union: only one member is used at a time
typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

// Struct: the union + its type
typedef struct {
    Data data;
    DataType type;
} Container;

// Set this to 1 to make the next string allocation fail (used only for testing)
int fail_alloc = 0;

// Function declarations
// Setters, getters and copy return 1 for success and 0 for failure.
// Always call container_init on a new container first!
void container_init(Container *c);
void container_clear(Container *c);

int container_set_int(Container *c, int value);
int container_set_float(Container *c, float value);
int container_set_char(Container *c, char value);
int container_set_string(Container *c, const char *value);

int container_get_int(const Container *c, int *out_value);
int container_get_float(const Container *c, float *out_value);
int container_get_char(const Container *c, char *out_value);
int container_get_string(const Container *c, const char **out_value);

int container_copy(Container *destination, const Container *source);
void container_print(const Container *c);
const char *container_type_name(const Container *c);
char *make_copy(const char *text);

int main() {
    Container c;
    int i;
    float f;
    char ch;
    const char *s;

    // ---------- Part 1: original demo ----------
    printf("=== Part 1: one container, four types ===\n");
    container_init(&c);

    container_set_int(&c, 10);
    if (container_get_int(&c, &i)) {
        printf("Int: %d\n", i);
    }

    container_set_float(&c, 3.14f);
    if (container_get_float(&c, &f)) {
        printf("Float: %.2f\n", f);
    }

    container_set_char(&c, 'A');
    if (container_get_char(&c, &ch)) {
        printf("Char: %c\n", ch);
    }

    container_set_string(&c, "Hello, World!");
    if (container_get_string(&c, &s)) {
        printf("String: %s\n", s);
    }

    container_clear(&c);

    // ---------- Part 2: array of 6 containers ----------
    printf("\n=== Part 2: array of 6 containers ===\n");
    Container arr[6];
    for (int k = 0; k < 6; k++) {
        container_init(&arr[k]);
    }

    container_set_int(&arr[0], 10);
    container_set_float(&arr[1], 3.14f);
    container_set_char(&arr[2], 'A');
    container_set_string(&arr[3], "Hello");
    container_set_int(&arr[4], 0);
    container_set_string(&arr[5], "Data");

    for (int k = 0; k < 6; k++) {
        printf("Index %d, type %s, value: ", k, container_type_name(&arr[k]));
        container_print(&arr[k]);
        printf("\n");
    }

    // ---------- Part 3: deep copy ----------
    printf("\n=== Part 3: deep copy ===\n");
    Container copy;
    container_init(&copy);

    container_copy(&copy, &arr[5]);
    container_set_string(&arr[5], "Changed");

    printf("Original: ");
    container_print(&arr[5]);
    printf("\nCopy: ");
    container_print(&copy);
    printf("\n");

    container_clear(&arr[5]);
    printf("Copy after clearing original: ");
    container_print(&copy);
    printf("\n");
    container_clear(&copy);

    // ---------- Part 4: edge cases ----------
    printf("\n=== Part 4: edge cases ===\n");
    Container e;
    container_init(&e);

    // 1. integer zero is a valid value
    container_set_int(&e, 0);
    if (container_get_int(&e, &i)) {
        printf("1. Zero stored and read OK, value = %d\n", i);
    }

    // 2. wrong type getter
    if (container_get_float(&e, &f) == 0) {
        printf("2. get_float on an INT container failed (correct)\n");
    }

    // 3. empty container
    container_clear(&e);
    printf("3. Empty container prints: ");
    container_print(&e);
    printf("\n");
    if (container_get_int(&e, &i) == 0) {
        printf("   get_int on empty container failed (correct)\n");
    }

    // 4. empty string
    container_set_string(&e, "");
    container_get_string(&e, &s);
    printf("4. Empty string stored, length = %d\n", (int)strlen(s));

    // 5. allocation failure keeps the old value
    container_set_string(&e, "Original");
    fail_alloc = 1;
    if (container_set_string(&e, "New text") == 0) {
        printf("5. Allocation failed, old value kept: ");
        container_print(&e);
        printf("\n");
    }

    // 6. allocation failure when the old value is an int
    Container g;
    container_init(&g);
    container_set_int(&g, 42);
    fail_alloc = 1;
    if (container_set_string(&g, "text") == 0) {
        printf("6. Allocation failed, old value kept: ");
        container_print(&g);
        printf("\n");
    }

    // 7. copy fails because of allocation failure
    fail_alloc = 1;
    if (container_copy(&g, &e) == 0) {
        printf("7. Copy failed, destination still: ");
        container_print(&g);
        printf("\n");
    }

    // 8. copy onto itself
    container_copy(&e, &e);
    printf("8. After self-copy: ");
    container_print(&e);
    printf("\n");

    // 9. set_string with the container's own string
    container_get_string(&e, &s);
    container_set_string(&e, s);
    printf("9. After set_string with its own pointer: ");
    container_print(&e);
    printf("\n");

    // 10. switch away from a string
    container_set_int(&e, 7);
    printf("10. String changed to int: ");
    container_print(&e);
    printf("\n");

    // 11. clear twice
    container_clear(&e);
    container_clear(&e);
    printf("11. Cleared twice, type is %s\n", container_type_name(&e));

    // 12. NULL arguments
    if (container_set_int(NULL, 5) == 0) {
        printf("12. set_int on NULL container failed (correct)\n");
    }
    if (container_set_string(&e, NULL) == 0) {
        printf("    set_string with NULL text failed (correct)\n");
    }
    container_clear(&g);

    // ---------- Part 5: type changes ----------
    printf("\n=== Part 5: STRING -> INT -> STRING -> FLOAT ===\n");
    Container t;
    container_init(&t);

    container_set_string(&t, "first text");
    printf("%s: ", container_type_name(&t));
    container_print(&t);
    printf("\n");

    container_set_int(&t, 99);
    printf("%s: ", container_type_name(&t));
    container_print(&t);
    printf("\n");

    container_set_string(&t, "second text");
    printf("%s: ", container_type_name(&t));
    container_print(&t);
    printf("\n");

    container_set_float(&t, 2.5f);
    printf("%s: ", container_type_name(&t));
    container_print(&t);
    printf("\n");

    container_clear(&t);

    // ---------- Clean up the array ----------
    for (int k = 0; k < 6; k++) {
        container_clear(&arr[k]);
    }
    printf("\nAll containers cleared.\n");

    return 0;
}

// ---------- Function definitions ----------

// Makes a new copy of a string on the heap. Returns NULL if it fails.
char *make_copy(const char *text) {
    if (fail_alloc == 1) {
        fail_alloc = 0;     // fail only once (testing)
        return NULL;
    }
    char *copy = malloc(strlen(text) + 1);   // +1 for '\0'
    if (copy == NULL) {
        return NULL;
    }
    strcpy(copy, text);
    return copy;
}

void container_init(Container *c) {
    if (c == NULL) {
        return;
    }
    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;
}

void container_clear(Container *c) {
    if (c == NULL) {
        return;
    }
    if (c->type == TYPE_STRING) {
        free(c->data.string_value);
    }
    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;
}

int container_set_int(Container *c, int value) {
    if (c == NULL) {
        return 0;
    }
    container_clear(c);     // frees the old string if there was one
    c->data.int_value = value;
    c->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *c, float value) {
    if (c == NULL) {
        return 0;
    }
    container_clear(c);
    c->data.float_value = value;
    c->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *c, char value) {
    if (c == NULL) {
        return 0;
    }
    container_clear(c);
    c->data.char_value = value;
    c->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL) {
        return 0;
    }

    // Make the new copy first. If this fails, the old value is not touched.
    char *copy = make_copy(value);
    if (copy == NULL) {
        return 0;
    }

    container_clear(c);     // free the old string only after the copy is made
    c->data.string_value = copy;
    c->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *c, int *out_value) {
    if (c == NULL || out_value == NULL) {
        return 0;
    }
    if (c->type != TYPE_INT) {
        return 0;
    }
    *out_value = c->data.int_value;
    return 1;
}

int container_get_float(const Container *c, float *out_value) {
    if (c == NULL || out_value == NULL) {
        return 0;
    }
    if (c->type != TYPE_FLOAT) {
        return 0;
    }
    *out_value = c->data.float_value;
    return 1;
}

int container_get_char(const Container *c, char *out_value) {
    if (c == NULL || out_value == NULL) {
        return 0;
    }
    if (c->type != TYPE_CHAR) {
        return 0;
    }
    *out_value = c->data.char_value;
    return 1;
}

// The string still belongs to the container, so do not free it
int container_get_string(const Container *c, const char **out_value) {
    if (c == NULL || out_value == NULL) {
        return 0;
    }
    if (c->type != TYPE_STRING) {
        return 0;
    }
    *out_value = c->data.string_value;
    return 1;
}

// Deep copy: a string gets its own new memory
int container_copy(Container *destination, const Container *source) {
    if (destination == NULL || source == NULL) {
        return 0;
    }
    if (destination == source) {
        return 1;           // copying onto itself, nothing to do
    }

    if (source->type == TYPE_STRING) {
        char *copy = make_copy(source->data.string_value);
        if (copy == NULL) {
            return 0;       // destination is not changed
        }
        container_clear(destination);
        destination->data.string_value = copy;
        destination->type = TYPE_STRING;
    } else {
        container_clear(destination);
        destination->data = source->data;   // int, float, char are safe to copy
        destination->type = source->type;
    }
    return 1;
}

void container_print(const Container *c) {
    if (c == NULL) {
        printf("(null container)");
    } else if (c->type == TYPE_EMPTY) {
        printf("(empty)");
    } else if (c->type == TYPE_INT) {
        printf("%d", c->data.int_value);
    } else if (c->type == TYPE_FLOAT) {
        printf("%.2f", c->data.float_value);
    } else if (c->type == TYPE_CHAR) {
        printf("'%c'", c->data.char_value);
    } else if (c->type == TYPE_STRING) {
        printf("\"%s\"", c->data.string_value);
    } else {
        printf("(invalid type)");
    }
}

const char *container_type_name(const Container *c) {
    if (c == NULL) {
        return "NULL";
    }
    if (c->type == TYPE_EMPTY) return "EMPTY";
    if (c->type == TYPE_INT) return "INT";
    if (c->type == TYPE_FLOAT) return "FLOAT";
    if (c->type == TYPE_CHAR) return "CHAR";
    if (c->type == TYPE_STRING) return "STRING";
    return "UNKNOWN";
}
