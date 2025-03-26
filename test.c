#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // For sleep()

// Task structure
typedef struct
{
    int id;
    char description[100];
    int priority;       // Higher number = higher priority
    time_t arrival_time;
    time_t start_time;
    time_t completion_time;
    double wait_time;   // Added to track wait time
} Task;

// Node for the priority queue
typedef struct Node
{
    Task task;
    struct Node* next;
} Node;

// Priority Queue structure
typedef struct
{
    Node* front;
    int size;
} PriorityQueue;

// List of processed tasks
typedef struct
{
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
void simulateScheduler();
void clearScreen();
void showMenu();
void initializeProcessedList(ProcessedTasksList* list);
void addToProcessedList(ProcessedTasksList* list, Task task);
void freeProcessedList(ProcessedTasksList* list);
void freeQueue(PriorityQueue* q);

int main()
{
    simulateScheduler();
    return 0;
}

// Clear screen function
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Show menu function
void showMenu()
{
    printf("\nMenu:\n");
    printf("1. Add new task\n");
    printf("2. Process next task\n");
    printf("3. View current queue\n");
    printf("4. Generate report\n");
    printf("5. Exit\n");
    printf("Enter choice: ");
}

// Initialize the priority queue
void initializeQueue(PriorityQueue* q)
{
    q->front = NULL;
    q->size = 0;
}

// Initialize processed tasks list
void initializeProcessedList(ProcessedTasksList* list)
{
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Add task to processed list
void addToProcessedList(ProcessedTasksList* list, Task task)
{
    if (list->size >= list->capacity)
    {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->tasks = realloc(list->tasks, list->capacity * sizeof(Task));
        if (!list->tasks)
        {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    list->tasks[list->size++] = task;
}

// Free processed list memory
void freeProcessedList(ProcessedTasksList* list)
{
    free(list->tasks);
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Free queue memory
void freeQueue(PriorityQueue* q)
{
    while (!isEmpty(q))
    {
        dequeue(q);
    }
}

// Check if queue is empty
int isEmpty(PriorityQueue* q)
{
    return q->front == NULL;
}

// Enqueue a task with priority handling
void enqueue(PriorityQueue* q, Task newTask)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;
    newNode->next = NULL;

    // If queue is empty or new task has higher priority than front
    if (isEmpty(q) || newTask.priority > q->front->task.priority)
    {
        newNode->next = q->front;
        q->front = newNode;
    }
    else
    {
        // Find the correct position based on priority
        Node* current = q->front;
        while (current->next != NULL && current->next->task.priority >= newTask.priority)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    q->size++;
}

// Dequeue the highest priority task
Task dequeue(PriorityQueue* q)
{
    if (isEmpty(q))
    {
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

// Display all tasks in the queue
void displayQueue(PriorityQueue* q)
{
    clearScreen();
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return;
    }
    printf("\nCurrent Task Queue:\n");
    printf("ID\tPriority\tDescription\n");
    printf("--------------------------------\n");

    Node* current = q->front;
    while (current != NULL)
    {
        printf("%d\t%d\t\t%s\n", current->task.id, current->task.priority, current->task.description);
        current = current->next;
    }
}

// Generate final report
void generateReport(ProcessedTasksList* processed)
{
    clearScreen();
    if (processed->size == 0)
    {
        printf("No tasks were processed.\n");
        return;
    }

    printf("\n=== TASK SCHEDULING REPORT ===\n");
    printf("Total tasks processed: %d\n", processed->size);

    // Calculate average wait time
    double totalWaitTime = 0;
    for (int i = 0; i < processed->size; i++)
    {
        totalWaitTime += processed->tasks[i].wait_time;
    }
    printf("Average wait time: %.2f seconds\n", totalWaitTime / processed->size);

    printf("\nTask Details:\n");
    printf("ID\tPriority\tWait Time\tDescription\n");
    printf("------------------------------------------------\n");

    for (int i = 0; i < processed->size; i++)
    {
        Task task = processed->tasks[i];
        printf("%d\t%d\t\t%.2f\t\t%s\n",
               task.id, task.priority, task.wait_time, task.description);
    }

    printf("\n================================\n");
}

// Main scheduler simulation
void simulateScheduler()
{
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);

    ProcessedTasksList processedTasks;
    initializeProcessedList(&processedTasks);

    int taskId = 1;
    int choice;

    srand(time(NULL)); // Seed for random processing time

    while (1)
    {
        clearScreen();
        printf("=== Task Scheduling System ===\n");
        showMenu();

        // Input validation for menu choice
        while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5)
        {
            printf("Invalid input! Please enter a number between 1-5: ");
            while (getchar() != '\n'); // Clear input buffer
        }
        getchar(); // Consume newline

        switch (choice)
        {
        case 1: // Add new task
        {
            clearScreen();
            printf("=== Add New Task ===\n");
            Task newTask;
            newTask.id = taskId++;

            printf("Enter task description: ");
            fgets(newTask.description, sizeof(newTask.description), stdin);
            newTask.description[strcspn(newTask.description, "\n")] = '\0';

            // Priority input validation
            printf("Enter priority (1-10, 10=highest): ");
            while (1)
            {
                if (scanf("%d", &newTask.priority) == 1 && newTask.priority >= 1 && newTask.priority <= 10)
                    break;
                printf("Invalid priority! Please enter a value between 1-10: ");
                while (getchar() != '\n'); // Clear input buffer
            }
            getchar(); // Consume newline

            newTask.arrival_time = time(NULL);
            enqueue(&taskQueue, newTask);

            printf("\nTask added successfully! Press Enter to continue...");
            getchar(); // Wait for user to press Enter
            break;
        }
        case 2: // Process next task
        {
            clearScreen();
            if (isEmpty(&taskQueue))
            {
                printf("No tasks in queue!\n");
            }
            else
            {
                printf("=== Processing Task ===\n");
                Task currentTask = dequeue(&taskQueue);
                currentTask.start_time = time(NULL);

                // Calculate wait time
                currentTask.wait_time = difftime(currentTask.start_time, currentTask.arrival_time);

                printf("\nTask %d: %s (Priority: %d)\n",
                       currentTask.id, currentTask.description, currentTask.priority);
                printf("Arrived at: %s", ctime(&currentTask.arrival_time));
                printf("Started at: %s", ctime(&currentTask.start_time));
                printf("Waited for: %.2f seconds\n", currentTask.wait_time);

                int processingTime = 1 + rand() % 5; // 1-5 seconds processing time
                printf("Processing for %d seconds...\n", processingTime);
                sleep(processingTime);

                currentTask.completion_time = time(NULL);
                printf("\nTask completed at: %s", ctime(&currentTask.completion_time));

                // Add to processed list
                addToProcessedList(&processedTasks, currentTask);
            }
            printf("\nPress Enter to continue...");
            getchar(); // Wait for user to press Enter
            break;
        }
        case 3: // View current queue
        {
            displayQueue(&taskQueue);
            printf("\nPress Enter to continue...");
            getchar(); // Wait for user to press Enter
            break;
        }
        case 4: // Generate report
        {
            generateReport(&processedTasks);
            printf("\nPress Enter to continue...");
            getchar(); // Wait for user to press Enter
            break;
        }
        case 5: // Exit
        {
            // Free allocated memory
            freeQueue(&taskQueue);
            freeProcessedList(&processedTasks);
            return;
        }
        }
    }
}
