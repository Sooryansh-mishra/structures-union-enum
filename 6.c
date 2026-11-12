
#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

enum ValueType { VALUE_INT, VALUE_FLOAT, VALUE_TEXT };

typedef struct {
    enum ValueType type;
    UnionData value;
} TaggedData;

int setInt(TaggedData *r, int x) {
    if (r == NULL) {
        return 0;
    }
    r->value.i = x;
    r->type = VALUE_INT;
    return 1;
}

int setFloat(TaggedData *r, float x) {
    if (r == NULL) {
        return 0;
    }
    r->value.f = x;
    r->type = VALUE_FLOAT;
    return 1;
}

int setText(TaggedData *r, const char *s) {
    if (r == NULL || s == NULL) {
        return 0;
    }
    if (strlen(s) > 19) {
        return 0;
    }
    strcpy(r->value.str, s);
    r->type = VALUE_TEXT;
    return 1;
}

void printRecord(TaggedData *r) {
    if (r->type == VALUE_INT) {
        printf("Integer: %d\n", r->value.i);
    } else if (r->type == VALUE_FLOAT) {
        printf("Float: %.2f\n", r->value.f);
    } else if (r->type == VALUE_TEXT) {
        printf("Text: %s\n", r->value.str);
    } else {
        printf("Invalid tag, cannot print\n");
    }
}

int main() {
    UnionData udata;
    StructData sdata;
    TaggedData rec[5];
    TaggedData t;
    int ok;

    printf("--- Size and alignment ---\n");
    printf("UnionData:  size = %zu, alignment = %zu\n", sizeof(UnionData), _Alignof(UnionData));
    printf("StructData: size = %zu, alignment = %zu\n", sizeof(StructData), _Alignof(StructData));
    printf("TaggedData: size = %zu, alignment = %zu\n\n", sizeof(TaggedData), _Alignof(TaggedData));

    printf("--- Addresses of union members ---\n");
    printf("udata.i:   %p\n", (void *)&udata.i);
    printf("udata.f:   %p\n", (void *)&udata.f);
    printf("udata.str: %p\n\n", (void *)&udata.str);

    printf("--- Addresses of structure members ---\n");
    printf("sdata.i:   %p\n", (void *)&sdata.i);
    printf("sdata.f:   %p\n", (void *)&sdata.f);
    printf("sdata.str: %p\n\n", (void *)&sdata.str);

    printf("--- Offsets in StructData ---\n");
    printf("i:   %zu\n", offsetof(StructData, i));
    printf("f:   %zu\n", offsetof(StructData, f));
    printf("str: %zu\n\n", offsetof(StructData, str));

    printf("--- Memory for 100 records ---\n");
    printf("100 structures: %zu bytes\n", 100 * sizeof(StructData));
    printf("100 unions:     %zu bytes\n", 100 * sizeof(UnionData));
    printf("100 tagged:     %zu bytes\n\n", 100 * sizeof(TaggedData));

    printf("--- Structure demo (all values stay) ---\n");
    sdata.i = 10;
    sdata.f = 220.5;
    strcpy(sdata.str, "Struct Programming");
    printf("%d, %.2f, %s\n\n", sdata.i, sdata.f, sdata.str);

    printf("--- Union demo (one value at a time) ---\n");
    udata.i = 10;
    printf("udata.i = %d\n", udata.i);
    udata.f = 220.5;
    printf("udata.f = %.2f\n", udata.f);
    strcpy(udata.str, "Union Programming");
    printf("udata.str = %s\n\n", udata.str);

    printf("--- Five tagged records ---\n");
    setInt(&rec[0], 10);
    setFloat(&rec[1], 220.5);
    setText(&rec[2], "Union Programming");
    setInt(&rec[3], 42);
    setText(&rec[4], "Hello");

    for (int i = 0; i < 5; i++) {
        printf("Record %d -> ", i + 1);
        printRecord(&rec[i]);
    }
    printf("\n");

    printf("--- Edge cases ---\n");

    ok = setText(&t, "1234567890123456789");
    printf("19 characters: ");
    if (ok == 1) {
        printf("accepted -> ");
        printRecord(&t);
    } else {
        printf("rejected\n");
    }

    ok = setText(&t, "12345678901234567890");
    printf("20 characters: ");
    if (ok == 1) {
        printf("accepted\n");
    } else {
        printf("rejected, record still has: ");
        printRecord(&t);
    }

    setInt(&t, 77);
    printf("After setInt:   ");
    printRecord(&t);
    setText(&t, "now text");
    printf("After setText:  ");
    printRecord(&t);
    setFloat(&t, 3.5);
    printf("After setFloat: ");
    printRecord(&t);

    t.type = 99;
    printf("Bad tag: ");
    printRecord(&t);
    printf("\n");

    printf("--- Size comparison ---\n");
    printf("Union:  %zu\n", sizeof(UnionData));
    printf("Tagged: %zu\n", sizeof(TaggedData));
    printf("Struct: %zu\n\n", sizeof(StructData));

    printf("--- Observations ---\n");
    printf("1. Union members share the same address, so the union is only as big as its largest member.\n");
    printf("2. Structure members have different offsets. Padding may be added for alignment.\n");
    printf("3. A union saves memory when a record holds only one type at a time.\n");
    printf("4. The values cannot be stored together, because a new value overwrites the old one.\n");
    printf("5. The tag tells us which member is safe to read.\n");

    return 0;
}
