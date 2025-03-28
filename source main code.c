#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

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
void showLoadingScreen();
void initializeQueue(PriorityQueue* q);
int isEmpty(PriorityQueue* q);
void enqueue(PriorityQueue* q, Task newTask);
Task dequeue(PriorityQueue* q);
void displayQueue(PriorityQueue* q);
void generateReport(ProcessedTasksList* processed);
void simulateScheduler();
void clearScreen();
void showMenu();
void initializeProcessedList(ProcessedTasksList* list);
void addToProcessedList(ProcessedTasksList* list, Task task);
void freeProcessedList(ProcessedTasksList* list);
void freeQueue(PriorityQueue* q);
void saveTasksToFile(ProcessedTasksList* list);
void loadTasksFromFile(ProcessedTasksList* list);
int isIdUnique(ProcessedTasksList* list, PriorityQueue* q, int id);
void removeTask(PriorityQueue* q);

void displayLogo() {
    printf(" __________            _      _____         _                    _\n");
    printf("|          |          | |    //____|       | |                  | |\n");
    printf(" ---| | ---           | |    ||            | |                  | |\n");
    printf("    | |   ____  _____ | | // ||___    ____ | |___   _____    ___| |\n");
    printf("    | |  //  || ||    | |//       \\\\ //__()| |___|//_____\\\\ /___| |    \n");
    printf("    | | ||   || ||--- | |\\\\  _____|| ||___ | |  ||||_____   |___| |\n");
    printf("    |_|  \\\\__\\\\ ___|| |_| \\\\ |____|| \\\\___/|_|  ||||_____|  |___|_|\n");
    printf("\n");
}

void showLoadingScreen() {
    clearScreen();
    printf("\n\nLoading Task Scheduler");
    fflush(stdout);

    for (int i = 0; i < 3; i++) {
        sleep(1);
        printf(".");
        fflush(stdout);
    }

    sleep(1);
    clearScreen();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    // First show the logo
    clearScreen();
    displayLogo();
    printf("\nPress Enter to start...");
    getchar();

    // Then show loading screen
    showLoadingScreen();

    // Finally start the scheduler
    simulateScheduler();
    return 0;
}

void showMenu() {
    printf("\nMenu:\n");
    printf("1. Add new task\n");
    printf("2. Process next task\n");
    printf("3. View current queue\n");
    printf("4. Generate report\n");
    printf("5. Remove task\n");
    printf("6. Exit\n");
    printf("Enter choice: ");
}

void initializeQueue(PriorityQueue* q) {
    q->front = NULL;
    q->size = 0;
}

void initializeProcessedList(ProcessedTasksList* list) {
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

int isIdUnique(ProcessedTasksList* list, PriorityQueue* q, int id) {
    for (int i = 0; i < list->size; i++) {
        if (list->tasks[i].id == id) {
            return 0;
        }
    }

    Node* current = q->front;
    while (current != NULL) {
        if (current->task.id == id) {
            return 0;
        }
        current = current->next;
    }

    return 1;
}

void addToProcessedList(ProcessedTasksList* list, Task task) {
    if (list->size >= list->capacity) {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->tasks = realloc(list->tasks, list->capacity * sizeof(Task));
        if (!list->tasks) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    list->tasks[list->size++] = task;
}

void freeProcessedList(ProcessedTasksList* list) {
    free(list->tasks);
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

void freeQueue(PriorityQueue* q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
}

int isEmpty(PriorityQueue* q) {
    return q->front == NULL;
}

void enqueue(PriorityQueue* q, Task newTask) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;
    newNode->next = NULL;

    if (isEmpty(q) || newTask.priority > q->front->task.priority) {
        newNode->next = q->front;
        q->front = newNode;
    } else {
        Node* current = q->front;
        while (current->next != NULL && current->next->task.priority >= newTask.priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    q->size++;
}

Task dequeue(PriorityQueue* q) {
    if (isEmpty(q)) {
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

void displayQueue(PriorityQueue* q) {
    clearScreen();
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return;
    }

    printf("\nCurrent Task Queue:\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Sr.No\tTask ID\tTask Name\t\tPriority\tArrived\t\t\tStatus\n");
    printf("-----------------------------------------------------------------------------\n");

    Node* current = q->front;
    int serial = 1;
    char timeBuf[20];

    while (current != NULL) {
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", localtime(&current->task.arrival_time));

        char displayDesc[24];
        strncpy(displayDesc, current->task.description, 20);
        if (strlen(current->task.description) > 20) {
            strcpy(displayDesc + 20, "...");
        } else {
            displayDesc[strlen(current->task.description)] = '\0';
        }

        printf("%d\t%d\t%-20s\t%d\t\t%s\tPending\n",
               serial++,
               current->task.id,
               displayDesc,
               current->task.priority,
               timeBuf);

        current = current->next;
    }
}

void generateReport(ProcessedTasksList* processed) {
    clearScreen();
    if (processed->size == 0) {
        printf("No tasks were processed.\n");
        return;
    }

    printf("\n=== TASK SCHEDULING REPORT ===\n");
    printf("Total tasks processed: %d\n", processed->size);

    double totalWaitTime = 0;
    double totalProcessingTime = 0;
    for (int i = 0; i < processed->size; i++) {
        totalWaitTime += processed->tasks[i].wait_time;
        totalProcessingTime += processed->tasks[i].processing_time;
    }
    printf("Average wait time: %.2f seconds\n", totalWaitTime / processed->size);
    printf("Average processing time: %.2f seconds\n", totalProcessingTime / processed->size);

    printf("\nTask Details:\n");
    printf("ID\tPriority\tWait Time\tProcessing Time\tDescription\n");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < processed->size; i++) {
        Task task = processed->tasks[i];
        printf("%d\t%d\t\t%.2f\t\t%.2f\t\t%s\n",
               task.id, task.priority, task.wait_time, task.processing_time, task.description);
    }

    printf("\n================================\n");
}

void saveTasksToFile(ProcessedTasksList* list) {
    FILE* file = fopen(DATA_FILE, "wb");
    if (!file) {
        perror("Failed to save data");
        return;
    }

    fwrite(&list->size, sizeof(int), 1, file);
    fwrite(list->tasks, sizeof(Task), list->size, file);
    fclose(file);
}

void loadTasksFromFile(ProcessedTasksList* list) {
    FILE* file = fopen(DATA_FILE, "rb");
    if (!file) return;

    int size;
    if (fread(&size, sizeof(int), 1, file) != 1) {
        fclose(file);
        return;
    }

    Task* tasks = malloc(size * sizeof(Task));
    if (!tasks) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    if (fread(tasks, sizeof(Task), size, file) != size) {
        free(tasks);
        fclose(file);
        return;
    }

    list->tasks = tasks;
    list->size = size;
    list->capacity = size;
    fclose(file);
}

void removeTask(PriorityQueue* q) {
    clearScreen();
    if (isEmpty(q)) {
        printf("Queue is empty! No tasks to remove.\n");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }

    int choice;
    int taskId;
    char taskDesc[100];
    char confirm;
    Node *current, *prev;
    int found = 0;

removal_start:
    printf("=== Remove Task ===\n");
    printf("Search by:\n");
    printf("1. Task ID\n");
    printf("2. Task Name\n");
    printf("Enter choice: ");

    while (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Invalid input! Please enter 1 or 2: ");
        while (getchar() != '\n');
    }
    getchar();

    if (choice == 1) {
        printf("Enter task ID to remove: ");
        while (scanf("%d", &taskId) != 1) {
            printf("Invalid input! Please enter a number: ");
            while (getchar() != '\n');
        }
        getchar();

        current = q->front;
        prev = NULL;

        while (current != NULL) {
            if (current->task.id == taskId) {
                found = 1;
                printf("\nFound task to remove:\n");
                printf("ID: %d\n", current->task.id);
                printf("Description: %s\n", current->task.description);
                printf("Priority: %d\n", current->task.priority);

                while (1) {
                    printf("\nAre you sure you want to remove this task? (y/n): ");
                    scanf(" %c", &confirm);
                    getchar();
                    confirm = tolower(confirm);

                    if (confirm == 'y') {
                        if (prev == NULL) {
                            q->front = current->next;
                        } else {
                            prev->next = current->next;
                        }
                        free(current);
                        q->size--;
                        printf("Task removed successfully.\n");
                        break;
                    }
                    else if (confirm == 'n') {
                        printf("Task removal cancelled.\n");
                        break;
                    }
                    else {
                        printf("Invalid input!\n");
                    }
                }
                break;
            }
            prev = current;
            current = current->next;
        }

        if (!found) {
            printf("No task found with ID %d.\n", taskId);
            do {
                printf("\nDo you want to try again? (y/n): ");
                if (scanf(" %c", &confirm) != 1) {
                    while (getchar() != '\n');
                    confirm = ' ';
                }
                getchar();
                confirm = tolower(confirm);

                if (confirm != 'y' && confirm != 'n')
                    printf("Invalid input! Please enter 'y' or 'n'.\n");
            } while (confirm != 'y' && confirm != 'n');

            if (confirm == 'y') {
                clearScreen();
                goto removal_start;
            }
            else {
                printf("Returning to main menu.\n");
                printf("\nPress Enter to continue...");
                getchar();
                return;
            }
        }
    }
    else {
        printf("Enter task name to remove: ");
        fgets(taskDesc, sizeof(taskDesc), stdin);
        taskDesc[strcspn(taskDesc, "\n")] = '\0';

        current = q->front;
        prev = NULL;

        while (current != NULL) {
            if (strcmp(current->task.description, taskDesc) == 0) {
                found = 1;
                printf("\nFound task to remove:\n");
                printf("ID: %d\n", current->task.id);
                printf("Description: %s\n", current->task.description);
                printf("Priority: %d\n", current->task.priority);

                while (1) {
                    printf("\nAre you sure you want to remove this task? (y/n): ");
                    scanf(" %c", &confirm);
                    getchar();
                    confirm = tolower(confirm);

                    if (confirm == 'y') {
                        if (prev == NULL) {
                            q->front = current->next;
                        } else {
                            prev->next = current->next;
                        }
                        free(current);
                        q->size--;
                        printf("Task removed successfully.\n");
                        break;
                    }
                    else if (confirm == 'n') {
                        printf("Task removal cancelled.\n");
                        break;
                    }
                    else {
                        printf("Invalid input!\n");
                    }
                }
                break;
            }
            prev = current;
            current = current->next;
        }

        if (!found) {
            printf("No task found with name '%s'.\n", taskDesc);
            do {
                printf("\nDo you want to try again? (y/n): ");
                if (scanf(" %c", &confirm) != 1) {
                    while (getchar() != '\n');
                    confirm = ' ';
                }
                getchar();
                confirm = tolower(confirm);

                if (confirm != 'y' && confirm != 'n')
                    printf("Invalid input! Please enter 'y' or 'n'.\n");
            } while (confirm != 'y' && confirm != 'n');

            if (confirm == 'y') {
                clearScreen();
                goto removal_start;
            }
            else {
                printf("Returning to main menu.\n");
                printf("\nPress Enter to continue...");
                getchar();
                return;
            }
        }
    }

    printf("\nPress Enter to continue...");
    getchar();
}

void simulateScheduler() {
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);

    ProcessedTasksList processedTasks;
    initializeProcessedList(&processedTasks);
    loadTasksFromFile(&processedTasks);

    int choice;

    while (1) {
        clearScreen();
        printf("=== Task Scheduling System ===\n");
        showMenu();

        while (scanf("%d", &choice) != 1 || choice < 1 || choice > 6) {
            printf("Invalid input! Please enter a number between 1-6: ");
            while (getchar() != '\n');
        }
        getchar();

        switch (choice) {
            case 1: {
                clearScreen();
                printf("=== Add New Task ===\n");
                Task newTask;
                newTask.processing_time = 0.0;

                printf("Enter task ID: ");
                while (1) {
                    if (scanf("%d", &newTask.id) == 1) {
                        if (isIdUnique(&processedTasks, &taskQueue, newTask.id)) {
                            break;
                        } else {
                            printf("ID already exists! Please enter a unique ID: ");
                        }
                    } else {
                        printf("Invalid ID! Please enter a number: ");
                        while (getchar() != '\n');
                    }
                }
                getchar();

                printf("Enter task description: ");
                fgets(newTask.description, sizeof(newTask.description), stdin);
                newTask.description[strcspn(newTask.description, "\n")] = '\0';

                printf("Enter priority (1-10, 10=highest): ");
                while (1) {
                    if (scanf("%d", &newTask.priority) == 1 && newTask.priority >= 1 && newTask.priority <= 10)
                        break;
                    printf("Invalid priority! Please enter a value between 1-10: ");
                    while (getchar() != '\n');
                }
                getchar();

                newTask.arrival_time = time(NULL);
                enqueue(&taskQueue, newTask);

                printf("\nTask added successfully! Press Enter to continue...");
                getchar();
                break;
            }
            case 2: {
                clearScreen();
                if (isEmpty(&taskQueue)) {
                    printf("No tasks in queue!\n");
                } else {
                    printf("=== Processing Task ===\n");
                    Task currentTask = dequeue(&taskQueue);
                    currentTask.start_time = time(NULL);
                    currentTask.wait_time = difftime(currentTask.start_time, currentTask.arrival_time);

                    printf("\nTask %d: %s (Priority: %d)\n",
                           currentTask.id, currentTask.description, currentTask.priority);
                    printf("Arrived at: %s", ctime(&currentTask.arrival_time));
                    printf("Started at: %s", ctime(&currentTask.start_time));
                    printf("Waited for: %.2f seconds\n", currentTask.wait_time);

                    printf("\nProcessing task... Press Enter when complete...");
                    time_t start_real = time(NULL);
                    getchar();
                    time_t end_real = time(NULL);
                    currentTask.processing_time = difftime(end_real, start_real);

                    currentTask.completion_time = time(NULL);
                    printf("\nTask completed at: %s", ctime(&currentTask.completion_time));
                    printf("Processing time: %.2f seconds\n", currentTask.processing_time);

                    addToProcessedList(&processedTasks, currentTask);
                    saveTasksToFile(&processedTasks);
                }
                printf("\nPress Enter to continue...");
                getchar();
                break;
            }
            case 3: {
                displayQueue(&taskQueue);
                printf("\nPress Enter to continue...");
                getchar();
                break;
            }
            case 4: {
                generateReport(&processedTasks);
                printf("\nPress Enter to continue...");
                getchar();
                break;
            }
            case 5: {
                removeTask(&taskQueue);
                break;
            }
            case 6: {
                freeQueue(&taskQueue);
                freeProcessedList(&processedTasks);
                printf("Exiting program. Goodbye!\n");
                 return;
            }
        }
    }
}
