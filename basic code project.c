#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct
{
    int id;
    char description[100];
    int priority;       // Higher number = higher priority
    time_t arrival_time;
    time_t start_time;
    time_t completion_time;
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

// Function prototypes
void initializeQueue(PriorityQueue* q);
int isEmpty(PriorityQueue* q);
void enqueue(PriorityQueue* q, Task newTask);
Task dequeue(PriorityQueue* q);
void displayQueue(PriorityQueue* q);
void generateReport(PriorityQueue* q, int totalTasks);
void simulateScheduler();

int main()
{
    simulateScheduler();
    return 0;
}

// Initialize the priority queue
void initializeQueue(PriorityQueue* q)
{
     q->front = NULL;
    q->size = 0;
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
    printf("\n");
}

// Generate final report
void generateReport(PriorityQueue* q, int totalTasks)
{
    if (totalTasks == 0)
    {
        printf("No tasks were processed.\n");
        return;
    }

    printf("\n=== TASK SCHEDULING REPORT ===\n");
    printf("Total tasks processed: %d\n", totalTasks);

    // Calculate average wait time (you would need to track this in your simulation)
    // This is just a placeholder - you'd need to implement proper tracking
    printf("Average wait time: %.2f seconds\n", 0.0);
    printf("================================\n");
}

// Main scheduler simulation
void simulateScheduler()
{
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);
    int taskId = 1;
    int choice;
    int totalTasksProcessed = 0;

    printf("=== Task Scheduling System ===\n");

    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Add new task\n");
        printf("2. Process next task\n");
        printf("3. View current queue\n");
        printf("4. Generate report and exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice)
        {
        case 1:
        {
            // Add new task
            Task newTask;
            newTask.id = taskId++;
            printf("Enter task description: ");
            fgets(newTask.description, sizeof(newTask.description), stdin);
            newTask.description[strcspn(newTask.description, "\n")] = '\0'; // Remove newline
            printf("Enter priority (1-10, 10=highest): ");
            scanf("%d", &newTask.priority);
            newTask.arrival_time = time(NULL);

            enqueue(&taskQueue, newTask);
            printf("Task added successfully!\n");
            break;
        }
        case 2:
        {
            // Process next task
            if (isEmpty(&taskQueue))
            {
                printf("No tasks in queue!\n");
            }
            else
            {
                Task currentTask = dequeue(&taskQueue);
                currentTask.start_time = time(NULL);

                printf("\nProcessing Task %d: %s (Priority: %d)\n",
                       currentTask.id, currentTask.description, currentTask.priority);
                printf("Task started at: %s", ctime(&currentTask.start_time));

                // Simulate task processing (sleep for random time)
                int processingTime = 1 + rand() % 5;
                printf("Processing for %d seconds...\n", processingTime);
                sleep(processingTime);

                currentTask.completion_time = time(NULL);
                printf("Task completed at: %s", ctime(&currentTask.completion_time));

                totalTasksProcessed++;
            }
            break;
        }

        case 3:
        {
            // View queue
            displayQueue(&taskQueue);
            break;
        }
        case 4:
        {
            // Generate report and exit
            generateReport(&taskQueue, totalTasksProcessed);
            return;
        }
        default:
        {
            printf("Invalid choice! Try again.\n");
        }
        }
    }
}

