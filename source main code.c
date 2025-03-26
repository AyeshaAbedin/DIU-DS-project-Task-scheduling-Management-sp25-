#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define DATA_FILE "task_records.dat"

// Task structure
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

// Node for priority queue
typedef struct Node {
    Task task;
    struct Node* next;
} Node;

// Priority Queue
typedef struct {
    Node* front;
    int size;
} PriorityQueue;

// Processed tasks list
typedef struct {
    Task* tasks;
    int size;
    int capacity;
} ProcessedTasksList;

// Function prototypes
void initializeQueue(PriorityQueue* q);
int isEmpty(PriorityQueue* q);
void enqueue(PriorityQueue* q, Task newTask);
Task dequeue(PriorityQueue* q);
void displayQueue(PriorityQueue* q);
void generateReport(ProcessedTasksList* processed);
void simulateScheduler(ProcessedTasksList* processedTasks);
void clearScreen();
void showMenu();
void initializeProcessedList(ProcessedTasksList* list);
void addToProcessedList(ProcessedTasksList* list, Task task);
void freeProcessedList(ProcessedTasksList* list);
void freeQueue(PriorityQueue* q);
void saveTasksToFile(ProcessedTasksList* list);
void loadTasksFromFile(ProcessedTasksList* list);
int getNextId(ProcessedTasksList* list);

int main() {
    ProcessedTasksList processedTasks;
    initializeProcessedList(&processedTasks);
    loadTasksFromFile(&processedTasks);

    simulateScheduler(&processedTasks);

    saveTasksToFile(&processedTasks);
    freeProcessedList(&processedTasks);
    return 0;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void showMenu() {
    printf("\nMenu:\n");
    printf("1. Add new task\n");
    printf("2. Process next task\n");
    printf("3. View current queue\n");
    printf("4. Generate report\n");
    printf("5. Exit\n");
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

int getNextId(ProcessedTasksList* list) {
    int max_id = 0;
    for (int i = 0; i < list->size; i++) {
        if (list->tasks[i].id > max_id) {
            max_id = list->tasks[i].id;
        }
    }
    return max_id + 1;
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
    printf("ID\tPriority\tDescription\n");
    printf("--------------------------------\n");

    Node* current = q->front;
    while (current != NULL) {
        printf("%d\t%d\t\t%s\n", current->task.id, current->task.priority, current->task.description);
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

void simulateScheduler(ProcessedTasksList* processedTasks) {
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);

    int choice;
    srand(time(NULL));

    while (1) {
        clearScreen();
        printf("=== Task Scheduling System ===\n");
        showMenu();

        while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
            printf("Invalid input! Please enter a number between 1-5: ");
            while (getchar() != '\n');
        }
        getchar();

        switch (choice) {
            case 1: {
                clearScreen();
                printf("=== Add New Task ===\n");
                Task newTask;
                newTask.id = getNextId(processedTasks);
                newTask.processing_time = 0.0;

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
                    clock_t start = clock();
                    getchar();
                    clock_t end = clock();
                    currentTask.processing_time = (double)(end - start) / CLOCKS_PER_SEC;

                    currentTask.completion_time = time(NULL);
                    printf("\nTask completed at: %s", ctime(&currentTask.completion_time));
                    printf("Processing time: %.2f seconds\n", currentTask.processing_time);

                    addToProcessedList(processedTasks, currentTask);
                    saveTasksToFile(processedTasks); // Auto-save after each task
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
                generateReport(processedTasks);
                printf("\nPress Enter to continue...");
                getchar();
                break;
            }
            case 5: {
                freeQueue(&taskQueue);
                printf("Exiting program. Goodbye!\n");
                 return;
            }
        }
    }
}
