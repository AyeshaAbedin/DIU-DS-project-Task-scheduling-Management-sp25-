#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <conio.h>
#define DATA_FILE "task_records.dat"

typedef struct {
    int id;
    char description[100];
    int priority;
    time_t arrival_time;
    time_t start_time;
    time_t completion_time;
    double wait_time;
    double processing_time;
} Task;

typedef struct Node {
    Task task;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    int size;
} PriorityQueue;

typedef struct {
    Task* tasks;
    int size;
    int capacity;
} ProcessedTasksList;

// Function prototypes
void displayLogo();

void gotoxy(int x, int y);
void clearScreen();
void initializeQueue(PriorityQueue* q);
int isEmpty(PriorityQueue* q);
void enqueue(PriorityQueue* q, Task newTask);
Task dequeue(PriorityQueue* q);
void displayQueue(PriorityQueue* q);
void initializeProcessedList(ProcessedTasksList* list);
void addToProcessedList(ProcessedTasksList* list, Task task);
void freeProcessedList(ProcessedTasksList* list);
void freeQueue(PriorityQueue* q);
void saveTasksToFile(ProcessedTasksList* list);
void loadTasksFromFile(ProcessedTasksList* list);
int isIdUnique(ProcessedTasksList* list, PriorityQueue* q, int id);
void removeTask(PriorityQueue* q);
void generateReport(ProcessedTasksList* processed);
void showMenu();
void simulateScheduler();
void clearInputBuffer();

// Cross-platform console positioning
#ifdef _WIN32
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
#else
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y+1, x+1);
}
#endif

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}




void displayLogo() {
    clearScreen();
    printf("\n\n");
    printf("\t\t\t __________            _      _____         _                    _\n");
    printf("\t\t\t|          |          | |    //____|       | |                  | |\n");
    printf("\t\t\t ---| | ---           | |    ||            | |                  | |\n");
    printf("\t\t\t    | |   ____  _____ | | // ||___    ____ | |___   _____    ___| |\n");
    printf("\t\t\t    | |  //  || ||    | |//       \\\\ //__()| |___|//_____\\\\ /___| |\n");
    printf("\t\t\t    | | ||   || ||--- | |\\\\  _____|| ||___ | |  ||||_____   |___| |\n");
    printf("\t\t\t    |_|  \\\\__\\\\ ___|| |_| \\\\ |____|| \\\\___/|_|  ||||_____|  |___|_|\n");

}

// Queue operations
void initializeQueue(PriorityQueue* q) {
    q->front = NULL;
    q->size = 0;
}

int isEmpty(PriorityQueue* q) {
    return q->front == NULL;
}

void enqueue(PriorityQueue* q, Task newTask) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->task = newTask;
    newNode->next = NULL;

    if(isEmpty(q)) {
        q->front = newNode;
    } else {
        Node* current = q->front;
        Node* prev = NULL;

        while(current != NULL && current->task.priority >= newTask.priority) {
            prev = current;
            current = current->next;
        }

        if(prev == NULL) {
            newNode->next = q->front;
            q->front = newNode;
        } else {
            prev->next = newNode;
            newNode->next = current;
        }
    }
    q->size++;
}

Task dequeue(PriorityQueue* q) {
    if(isEmpty(q)) {
        printf("Queue is empty!\n");
        exit(EXIT_FAILURE);
    }
    Node* temp = q->front;
    Task task = temp->task;
    q->front = q->front->next;
    free(temp);
    q->size--;
    return task;
}

// Display queue with borders
// Display queue with borders using ASCII codes
void displayQueue(PriorityQueue* q) {
    clearScreen();
    if(isEmpty(q)) {
        printf("Queue is empty!\n");
        Sleep(2000);
        return;
    }

    // Column width definitions
    #define SNO_WIDTH    7
    #define TASKID_WIDTH 9
    #define DESC_WIDTH   35
    #define PRIO_WIDTH   10
    #define TIME_WIDTH   19
    #define STATUS_WIDTH 14

    // Color definitions (if needed)
    #define BGRN "\033[1;32m"
    #define reset "\033[0m"

    printf(BGRN"\n\t\t\t   Current Task Queue \n"reset);

    // Top border
    printf("%c", 201);
    for(int i=0; i<SNO_WIDTH; i++) printf("%c", 205);
    printf("%c", 203);
    for(int i=0; i<TASKID_WIDTH; i++) printf("%c", 205);
    printf("%c", 203);
    for(int i=0; i<DESC_WIDTH; i++) printf("%c", 205);
    printf("%c", 203);
    for(int i=0; i<PRIO_WIDTH; i++) printf("%c", 205);
    printf("%c", 203);
    for(int i=0; i<TIME_WIDTH; i++) printf("%c", 205);
    printf("%c", 203);
    for(int i=0; i<STATUS_WIDTH; i++) printf("%c", 205);
    printf("%c\n", 187);

    // Header row
    printf("%c%-*s %c%-*s %c%-*s %c%-*s %c%-*s %c%-*s %c\n",
           186, SNO_WIDTH-1, "S.No",
           186, TASKID_WIDTH-1, "Task ID",
           186, DESC_WIDTH-1, "Description",
           186, PRIO_WIDTH-1, "Priority",
           186, TIME_WIDTH-1, "Arrival Time",
           186, STATUS_WIDTH-1, "Status",
           186);

    // Header separator
    printf("%c", 204);
    for(int i=0; i<SNO_WIDTH; i++) printf("%c", 205);
    printf("%c", 206);
    for(int i=0; i<TASKID_WIDTH; i++) printf("%c", 205);
    printf("%c", 206);
    for(int i=0; i<DESC_WIDTH; i++) printf("%c", 205);
    printf("%c", 206);
    for(int i=0; i<PRIO_WIDTH; i++) printf("%c", 205);
    printf("%c", 206);
    for(int i=0; i<TIME_WIDTH; i++) printf("%c", 205);
    printf("%c", 206);
    for(int i=0; i<STATUS_WIDTH; i++) printf("%c", 205);
    printf("%c\n", 185);

    Node* current = q->front;
    int serial = 1;
    char timeBuf[20];

    while(current != NULL) {
        // Format time
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M",
            localtime(&current->task.arrival_time));

        // Truncate description if needed
        char displayDesc[DESC_WIDTH+1];
        strncpy(displayDesc, current->task.description, DESC_WIDTH);
        displayDesc[DESC_WIDTH] = '\0';
        if(strlen(current->task.description) > DESC_WIDTH) {
            displayDesc[DESC_WIDTH-3] = '.';
            displayDesc[DESC_WIDTH-2] = '.';
            displayDesc[DESC_WIDTH-1] = '.';
        }

       #define VLINE 186

printf("%c%-*d %c%-*d %c%-*s %c%-*d %c%-*s %c%-*s %c\n",
    VLINE, SNO_WIDTH-1, serial++,
    VLINE, TASKID_WIDTH-1, current->task.id,
    VLINE, DESC_WIDTH-1, displayDesc,
    VLINE, PRIO_WIDTH-1, current->task.priority,
    VLINE, TIME_WIDTH-1, timeBuf,
    VLINE, STATUS_WIDTH-1, "Pending",
    VLINE);

        // Add row separator if not last item
        if(current->next != NULL) {
            printf("%c", 204);
            for(int i=0; i<SNO_WIDTH; i++) printf("%c", 205);
            printf("%c", 206);
            for(int i=0; i<TASKID_WIDTH; i++) printf("%c", 205);
            printf("%c", 206);
            for(int i=0; i<DESC_WIDTH; i++) printf("%c", 205);
            printf("%c", 206);
            for(int i=0; i<PRIO_WIDTH; i++) printf("%c", 205);
            printf("%c", 206);
            for(int i=0; i<TIME_WIDTH; i++) printf("%c", 205);
            printf("%c", 206);
            for(int i=0; i<STATUS_WIDTH; i++) printf("%c", 205);
            printf("%c\n", 185);
        }

        current = current->next;
    }

    // Bottom border
    printf("%c", 200);
    for(int i=0; i<SNO_WIDTH; i++) printf("%c", 205);
    printf("%c", 202);
    for(int i=0; i<TASKID_WIDTH; i++) printf("%c", 205);
    printf("%c", 202);
    for(int i=0; i<DESC_WIDTH; i++) printf("%c", 205);
    printf("%c", 202);
    for(int i=0; i<PRIO_WIDTH; i++) printf("%c", 205);
    printf("%c", 202);
    for(int i=0; i<TIME_WIDTH; i++) printf("%c", 205);
    printf("%c", 202);
    for(int i=0; i<STATUS_WIDTH; i++) printf("%c", 205);
    printf("%c\n", 188);

    printf("\n\t\t    Press 'ENTER' to continue...");
    getchar();
    getchar();
}

// File operations
void saveTasksToFile(ProcessedTasksList* list) {
    FILE* file = fopen(DATA_FILE, "wb");
    if(!file) {
        perror("Failed to save data");
        return;
    }
    fwrite(&list->size, sizeof(int), 1, file);
    fwrite(list->tasks, sizeof(Task), list->size, file);
    fclose(file);
}

void loadTasksFromFile(ProcessedTasksList* list) {
    FILE* file = fopen(DATA_FILE, "rb");
    if(!file) return;

    int size;
    if(fread(&size, sizeof(int), 1, file) != 1) {
        fclose(file);
        return;
    }

    Task* tasks = malloc(size * sizeof(Task));
    if(!tasks) {
        fclose(file);
        return;
    }

    if(fread(tasks, sizeof(Task), size, file) != size) {
        free(tasks);
        fclose(file);
        return;
    }

    list->tasks = tasks;
    list->size = size;
    list->capacity = size;
    fclose(file);
}

// Processed tasks management
void initializeProcessedList(ProcessedTasksList* list) {
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

void addToProcessedList(ProcessedTasksList* list, Task task) {
    if(list->size >= list->capacity) {
        int newCapacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        Task* newTasks = realloc(list->tasks, newCapacity * sizeof(Task));
        if(!newTasks) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        list->tasks = newTasks;
        list->capacity = newCapacity;
    }
    list->tasks[list->size++] = task;
}

// ID validation
int isIdUnique(ProcessedTasksList* list, PriorityQueue* q, int id) {
    for(int i=0; i<list->size; i++) {
        if(list->tasks[i].id == id) return 0;
    }

    Node* current = q->front;
    while(current != NULL) {
        if(current->task.id == id) return 0;
        current = current->next;
    }
    return 1;
}

// Report generation
void generateReport(ProcessedTasksList* processed) {
    clearScreen();
    if(processed->size == 0) {
        printf("No tasks processed yet!\n");
        return;
    }

    printf("\n=== TASK PROCESSING REPORT ===\n");
    printf("%-5s %-20s %-8s %-12s %-12s %-12s\n",
           "ID", "Description", "Priority", "Wait Time", "Proc Time", "Total Time");
    printf("------------------------------------------------------------\n");

    double totalWait = 0, totalProc = 0;
    for(int i=0; i<processed->size; i++) {
        Task t = processed->tasks[i];
        printf("%-5d %-20.20s %-8d %-12.2f %-12.2f %-12.2f\n",
               t.id, t.description, t.priority,
               t.wait_time, t.processing_time,
               t.wait_time + t.processing_time);

        totalWait += t.wait_time;
        totalProc += t.processing_time;
    }

    printf("\nSummary:\n");
    printf("Total tasks processed: %d\n", processed->size);
    printf("Average wait time: %.2f seconds\n", totalWait / processed->size);
    printf("Average processing time: %.2f seconds\n", totalProc / processed->size);
    printf("Total system time: %.2f seconds\n", totalWait + totalProc);
}

// Menu system
void showMenu() {
    printf("\n");
    for(int i=0; i<50; i++) printf("═");
    printf("\n");
    printf("        Task Scheduler Menu\n");
    for(int i=0; i<50; i++) printf("─");
    printf("\n");
    printf("1. Add New Task\n");
    printf("2. Process Next Task\n");
    printf("3. View Current Queue\n");
    printf("4. Generate Report\n");
    printf("5. Remove Task\n");
    printf("6. Exit\n");
    for(int i=0; i<50; i++) printf("─");
    printf("\nEnter your choice: ");
}

// Task removal functionality
void removeTask(PriorityQueue* q) {
    clearScreen();
    if(isEmpty(q)) {
        printf("Queue is empty!\n");
        return;
    }

    int id;
    printf("Enter Task ID to remove: ");
    while(scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid input! Enter numeric ID: ");
    }
    clearInputBuffer();

    Node *current = q->front, *prev = NULL;
    int found = 0;

    while(current != NULL) {
        if(current->task.id == id) {
            found = 1;
            printf("\nFound task:\nID: %d\nDescription: %s\nPriority: %d\n",
                   current->task.id, current->task.description, current->task.priority);

            char confirm;
            printf("\nConfirm deletion (y/n)? ");
            scanf(" %c", &confirm);
            clearInputBuffer();

            if(tolower(confirm) == 'y') {
                if(prev == NULL) {
                    q->front = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                q->size--;
                printf("Task removed successfully!\n");
            } else {
                printf("Deletion cancelled.\n");
            }
            break;
        }
        prev = current;
        current = current->next;
    }

    if(!found) {
        printf("Task with ID %d not found!\n", id);
    }
}

// Memory cleanup
void freeQueue(PriorityQueue* q) {
    while(!isEmpty(q)) {
        dequeue(q);
    }
}

void freeProcessedList(ProcessedTasksList* list) {
    free(list->tasks);
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Main scheduler logic
void simulateScheduler() {
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);

    ProcessedTasksList processed;
    initializeProcessedList(&processed);
    loadTasksFromFile(&processed);

    int choice;
    while(1) {
        clearScreen();
        displayLogo();
        showMenu();

        if(scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input! Please enter a number.\n");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            continue;
        }
        clearInputBuffer();

        switch(choice) {
            case 1: {
                clearScreen();
                Task newTask;
                printf("\n=== Add New Task ===\n");

                // ID Input
                int valid = 0;
                while(!valid) {
                    printf("Enter task ID: ");
                    if(scanf("%d", &newTask.id) == 1) {
                        if(isIdUnique(&processed, &taskQueue, newTask.id)) {
                            valid = 1;
                        } else {
                            printf("ID already exists! Try again.\n");
                        }
                    } else {
                        printf("Invalid ID! Please enter a number.\n");
                        clearInputBuffer();
                    }
                }
                clearInputBuffer();

                // Description Input
                printf("Enter task description: ");
                fgets(newTask.description, sizeof(newTask.description), stdin);
                newTask.description[strcspn(newTask.description, "\n")] = '\0';

                // Priority Input
                valid = 0;
                while(!valid) {
                    printf("Enter priority (1-10): ");
                    if(scanf("%d", &newTask.priority) == 1) {
                        if(newTask.priority >=1 && newTask.priority <=10) {
                            valid = 1;
                        } else {
                            printf("Priority must be between 1-10!\n");
                        }
                    } else {
                        printf("Invalid input! Please enter a number.\n");
                        clearInputBuffer();
                    }
                }
                clearInputBuffer();

                newTask.arrival_time = time(NULL);
                enqueue(&taskQueue, newTask);
                printf("\nTask added successfully!\n");
                break;
            }

            case 2: {
                clearScreen();
                if(isEmpty(&taskQueue)) {
                    printf("No tasks in queue!\n");
                } else {
                    Task current = dequeue(&taskQueue);
                    current.start_time = time(NULL);
                    current.wait_time = difftime(current.start_time, current.arrival_time);

                    printf("\nProcessing Task %d: %s\n", current.id, current.description);
                    printf("Priority: %d\n", current.priority);
                    printf("Waiting time: %.2f seconds\n", current.wait_time);

                    printf("\nPress Enter to complete processing...");
                    getchar();
                    current.completion_time = time(NULL);
                    current.processing_time = difftime(current.completion_time, current.start_time);

                    addToProcessedList(&processed, current);
                    saveTasksToFile(&processed);
                    printf("\nTask completed in %.2f seconds!\n", current.processing_time);
                }
                break;
            }

            case 3:
                displayQueue(&taskQueue);
                break;

            case 4:
                generateReport(&processed);
                break;

            case 5:
                removeTask(&taskQueue);
                break;

            case 6:
                freeQueue(&taskQueue);
                freeProcessedList(&processed);
                printf("Exiting system. Goodbye!\n");
                exit(0);

            default:
                printf("Invalid choice! Try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

int main() {
    clearScreen();
    displayLogo();
    simulateScheduler();
    return 0;
}
