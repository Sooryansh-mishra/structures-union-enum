
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define CMD_SIZE 20
#define LINE_SIZE 100
#define DESC_SIZE 50
#define START_CAPACITY 2

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef enum {
    PENDING,
    DONE
} Status;

typedef struct {
    int id;
    char description[DESC_SIZE];
    Status status;
} Task;

typedef struct {
    Task *tasks;
    int count;
    int capacity;
} TaskList;

int readLine(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        return 0;
    }

    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return 1;
    }

    int c;
    int extra = 0;
    while ((c = getchar()) != '\n' && c != EOF) {
        extra = 1;
    }
    if (extra == 1) {
        return -1;
    }
    return 1;
}

void trim(char *s) {
    int start = 0;
    while (s[start] != '\0' && isspace((unsigned char)s[start])) {
        start++;
    }

    int end = strlen(s);
    while (end > start && isspace((unsigned char)s[end - 1])) {
        end--;
    }

    int j = 0;
    for (int i = start; i < end; i++) {
        s[j] = s[i];
        j++;
    }
    s[j] = '\0';
}

int readId(const char *prompt, int *id) {
    char buf[LINE_SIZE];
    char *end;

    printf("%s", prompt);
    int r = readLine(buf, LINE_SIZE);
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        printf("Error: input is too long.\n");
        return -1;
    }

    trim(buf);
    if (buf[0] == '\0') {
        printf("Error: ID is missing.\n");
        return -1;
    }

    errno = 0;
    long value = strtol(buf, &end, 10);
    if (*end != '\0') {
        printf("Error: '%s' is not a valid number.\n", buf);
        return -1;
    }
    if (errno == ERANGE || value > INT_MAX) {
        printf("Error: ID is too large.\n");
        return -1;
    }
    if (value <= 0) {
        printf("Error: ID must be a positive number.\n");
        return -1;
    }

    *id = (int)value;
    return 1;
}

int readDescription(char *desc) {
    printf("Enter description: ");
    int r = readLine(desc, DESC_SIZE);
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        printf("Error: description is too long (max %d characters).\n", DESC_SIZE - 2);
        return -1;
    }

    trim(desc);
    if (desc[0] == '\0') {
        printf("Error: description cannot be empty.\n");
        return -1;
    }
    return 1;
}

CommandType get_command_type(const char *commandStr) {
    char upper[CMD_SIZE];
    int i;

    for (i = 0; commandStr[i] != '\0' && i < CMD_SIZE - 1; i++) {
        upper[i] = toupper((unsigned char)commandStr[i]);
    }
    upper[i] = '\0';

    if (strcmp(upper, "ADD") == 0) return CMD_ADD;
    if (strcmp(upper, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(upper, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(upper, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(upper, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(upper, "FIND") == 0) return CMD_FIND;
    if (strcmp(upper, "HELP") == 0) return CMD_HELP;
    if (strcmp(upper, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

const char *statusText(Status s) {
    if (s == DONE) {
        return "DONE";
    }
    return "PENDING";
}

int findIndex(TaskList *list, int id) {
    for (int i = 0; i < list->count; i++) {
        if (list->tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

int addTask(TaskList *list) {
    int id;
    char desc[DESC_SIZE];

    int r = readId("Enter task ID: ", &id);
    if (r == 0) return 0;
    if (r == -1) return 1;

    if (findIndex(list, id) != -1) {
        printf("Error: a task with ID %d already exists.\n", id);
        return 1;
    }

    r = readDescription(desc);
    if (r == 0) return 0;
    if (r == -1) return 1;

    if (list->count == list->capacity) {
        int newCapacity = list->capacity * 2;
        Task *temp = realloc(list->tasks, newCapacity * sizeof(Task));
        if (temp == NULL) {
            printf("Error: not enough memory, task not added.\n");
            return 1;
        }
        list->tasks = temp;
        list->capacity = newCapacity;
        printf("(List expanded, capacity is now %d)\n", list->capacity);
    }

    Task *t = &list->tasks[list->count];
    t->id = id;
    strcpy(t->description, desc);
    t->status = PENDING;
    list->count++;

    printf("Task %d added.\n", id);
    return 1;
}

int deleteTask(TaskList *list) {
    int id;

    if (list->count == 0) {
        printf("The task list is empty.\n");
        return 1;
    }

    int r = readId("Enter task ID to delete: ", &id);
    if (r == 0) return 0;
    if (r == -1) return 1;

    int index = findIndex(list, id);
    if (index == -1) {
        printf("Error: task %d was not found.\n", id);
        return 1;
    }

    for (int i = index; i < list->count - 1; i++) {
        list->tasks[i] = list->tasks[i + 1];
    }
    list->count--;

    printf("Task %d deleted.\n", id);
    return 1;
}

void viewTasks(TaskList *list) {
    if (list->count == 0) {
        printf("The task list is empty.\n");
        return;
    }

    printf("%-6s %-50s %s\n", "ID", "Description", "Status");
    for (int i = 0; i < list->count; i++) {
        printf("%-6d %-50s %s\n", list->tasks[i].id,
               list->tasks[i].description, statusText(list->tasks[i].status));
    }
}

int updateTask(TaskList *list) {
    int id;
    char desc[DESC_SIZE];

    if (list->count == 0) {
        printf("The task list is empty.\n");
        return 1;
    }

    int r = readId("Enter task ID to update: ", &id);
    if (r == 0) return 0;
    if (r == -1) return 1;

    int index = findIndex(list, id);
    if (index == -1) {
        printf("Error: task %d was not found.\n", id);
        return 1;
    }

    r = readDescription(desc);
    if (r == 0) return 0;
    if (r == -1) return 1;

    strcpy(list->tasks[index].description, desc);
    printf("Task %d updated.\n", id);
    return 1;
}

int completeTask(TaskList *list) {
    int id;

    if (list->count == 0) {
        printf("The task list is empty.\n");
        return 1;
    }

    int r = readId("Enter task ID to complete: ", &id);
    if (r == 0) return 0;
    if (r == -1) return 1;

    int index = findIndex(list, id);
    if (index == -1) {
        printf("Error: task %d was not found.\n", id);
        return 1;
    }

    if (list->tasks[index].status == DONE) {
        printf("Task %d is already DONE.\n", id);
        return 1;
    }

    list->tasks[index].status = DONE;
    printf("Task %d marked as DONE.\n", id);
    return 1;
}

int findTask(TaskList *list) {
    int id;

    if (list->count == 0) {
        printf("The task list is empty.\n");
        return 1;
    }

    int r = readId("Enter task ID to find: ", &id);
    if (r == 0) return 0;
    if (r == -1) return 1;

    int index = findIndex(list, id);
    if (index == -1) {
        printf("Error: task %d was not found.\n", id);
        return 1;
    }

    printf("ID: %d\nDescription: %s\nStatus: %s\n", list->tasks[index].id,
           list->tasks[index].description, statusText(list->tasks[index].status));
    return 1;
}

void showHelp() {
    printf("Commands (not case-sensitive):\n");
    printf("  ADD      - add a new task (ID and description)\n");
    printf("  DELETE   - delete a task by ID\n");
    printf("  VIEW     - show all tasks\n");
    printf("  UPDATE   - change the description of a task\n");
    printf("  COMPLETE - mark a task as DONE\n");
    printf("  FIND     - show one task by ID\n");
    printf("  HELP     - show this list\n");
    printf("  EXIT     - quit the program\n");
}

int main() {
    TaskList list;
    char line[CMD_SIZE];
    int running = 1;

    list.count = 0;
    list.capacity = START_CAPACITY;
    list.tasks = malloc(list.capacity * sizeof(Task));
    if (list.tasks == NULL) {
        printf("Error: could not allocate memory.\n");
        return 1;
    }

    printf("Task List Manager. Type HELP to see the commands.\n");

    while (running) {
        printf("\nEnter command: ");
        int r = readLine(line, CMD_SIZE);

        if (r == 0) {
            printf("\nEnd of input reached.\n");
            break;
        }
        if (r == -1) {
            printf("Error: command is too long (max %d characters).\n", CMD_SIZE - 2);
            continue;
        }

        trim(line);
        if (line[0] == '\0') {
            continue;
        }

        int ok = 1;
        CommandType cmd = get_command_type(line);

        switch (cmd) {
            case CMD_ADD:      ok = addTask(&list);      break;
            case CMD_DELETE:   ok = deleteTask(&list);   break;
            case CMD_VIEW:     viewTasks(&list);         break;
            case CMD_UPDATE:   ok = updateTask(&list);   break;
            case CMD_COMPLETE: ok = completeTask(&list); break;
            case CMD_FIND:     ok = findTask(&list);     break;
            case CMD_HELP:     showHelp();               break;
            case CMD_EXIT:
                printf("Exiting program.\n");
                running = 0;
                break;
            default:
                printf("Error: unknown command '%s'. Type HELP for the list.\n", line);
        }

        if (ok == 0) {
            printf("\nEnd of input reached.\n");
            break;
        }
    }

    free(list.tasks);
    printf("Memory released. Goodbye.\n");
    return 0;
}
