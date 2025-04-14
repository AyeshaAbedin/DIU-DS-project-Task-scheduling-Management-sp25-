#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <windows.h> // for Sleep and cls
#include<conio.h>
#include"colour.h"
#define DATA_FILE "task_records.dat"

typedef struct
{
    int id;
    char description[100];
    int priority;
    time_t arrival_time;
    time_t start_time;
    time_t completion_time;
    double wait_time;
    double processing_time;
} Task;

typedef struct Node
{
    Task task;
    struct Node* next;
} Node;

typedef struct
{
    Node* front;
    int size;
} PriorityQueue;

typedef struct
{
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
void loadingBar();

int main()
{
    // First show the logo
    clearScreen();
    displayLogo();
    loadingAnimation();

    simulateScheduler();
    return 0;
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void displayLogo()
{
    printf("\n\n\n");


    printf("\t\t\t"BHCYN    " __________            _      _____         _                    _\n");
    printf("\t\t\t|          |          | |    //____|       | |                  | |\n");
    printf("\t\t\t ---| | ---           | |    ||            | |                  | |\n");
    printf("\t\t\t    | |   ____  _____ | | // ||___    ____ | |___   _____    ___| |\n");
    printf("\t\t\t    | |  //  || ||    | |//       \\\\ //__()| |___|//_____\\\\ /___| |    \n");
    printf("\t\t\t    | | ||   || ||--- | |\\\\  _____|| ||___ | |  ||||_____   |___| |\n");
    printf("\t\t\t    |_|  \\\\__\\\\ ___|| |_| \\\\ |____|| \\\\___/|_|  ||||_____|  |___|_|"reset"\n");
    printf("\n");
    loadingBar();
}
void loadingBar()
{
    printf("\n\n\n\t\t\t\t\t\t Loading:-\n ");

    char a = 196, b = 219;
    printf("\n\t\t\t\t\t");
    for (int i = 0; i < 26; i++)
        printf("%c",a);
    printf("\r\t\t\t\t\t");
    for (int i = 0; i < 26; i++)
    {
        printf(""BHCYN"%c"reset"", b );
        Sleep(80);
    }
    printf("\n");
}

void showLoadingScreen()
{
    clearScreen();
    printf("\n\nLoading Task Scheduler");
    fflush(stdout);

    for (int i = 0; i < 3; i++)
    {
        sleep(1);
        printf(".");
        fflush(stdout);
    }

    sleep(1);
    clearScreen();
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void showMenu()
{
    system("cls");
    printf("\n");
    printf("\n\n\n");
    printf("\t\t\t\t  %c", 201);
    for (int i = 0; i < 60; i++)
        printf("%c", 205);
    printf("%c\n", 187);
    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);
    printf("\t\t\t\t  %c"BHCYN "                TASK SCHEDULING SYSTEM                "reset"      %c\n", 186, 186);
    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);
    printf("\t\t\t\t  %c", 204);
    for (int i = 0; i < 60; i++)
        printf("%c", 205);
    printf("%c\n", 185);

    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);
    printf("\t\t\t\t  %c    1. Add new task                                         %c\n", 186, 186);
    printf("\t\t\t\t  %c    2. Process next task                                    %c\n", 186, 186);
    printf("\t\t\t\t  %c    3. View current queue                                   %c\n", 186, 186);
    printf("\t\t\t\t  %c    4. Generate report                                      %c\n", 186, 186);
    printf("\t\t\t\t  %c    5. Remove task                                          %c\n", 186, 186);
    printf("\t\t\t\t  %c    6. Exit                                                 %c\n", 186, 186);
    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);
    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);
    printf("\t\t\t\t  %c   "BHCYN"  Enter your choice:                                "reset"     %c\n", 186, 186);
    printf("\t\t\t\t  %c                                                            %c\n", 186, 186);  // Added extra line
    printf("\t\t\t\t  %c", 200);
    for (int i = 0; i < 60; i++)
        printf("%c", 205);
    printf("%c\n", 188);
    gotoxy(40, 21);  // Changed from 18 to 21 to account for extra lines



}


void drawBox(int x, int y, int width, int height)
{
    gotoxy(x, y);
    printf("%c", 201);

    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);

    printf("%c", 187);

    for (int i = 0; i < height - 2; i++)
    {
        gotoxy(x, y + i + 1);
        printf("%c", 186);
        gotoxy(x + width - 1, y + i + 1);
        printf("%c", 186);
    }

    gotoxy(x, y + height - 1);
    printf("%c", 200);

    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);

    printf("%c", 188);
}

//Function to draw a thin box
void thinbox(int x, int y, int width, int height)
{
    gotoxy(x, y);
    printf("%c", 218);

    for (int i = 0; i < width - 2; i++)
        printf("%c", 196);

    printf("%c", 191);

    for (int i = 0; i < height - 2; i++)
    {
        gotoxy(x, y + i + 1);
        printf("%c", 179);
        gotoxy(x + width - 1, y + i + 1);
        printf("%c", 179);
    }

    gotoxy(x, y + height - 1);
    printf("%c", 192);

    for (int i = 0; i < width - 2; i++)
        printf("%c", 196);

    printf("%c", 217);
}

// Function to draw the main box
void mainbox()
{

    drawBox(10, 2, 100, 26);
    gotoxy(50, 5);
    printf(BHCYN "TASK MANAGEMENT SYSTEM" reset);
    gotoxy(45, 6);
    printf("================================");
}
void loadingAnimation()
{
    system("cls");

    mainbox();

    thinbox(40, 16, 40, 3);
    gotoxy(41, 15);
    printf(BHWHT "Password" reset);

    char passInput[32];
    char ch;
    int i = 0;
    gotoxy(43, 17);

    while (1)
    {
        ch = getch();
        if (ch == 13)
        {
            passInput[i] = '\0';
            break;
        }
        else if (ch == 8)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else
        {
            passInput[i] = ch;
            i++;
            printf("*");
        }
    }

    while (strcmp(passInput, "12345") != 0)
    {
        gotoxy(40, 20);
        printf(RED "Wrong password. Try again!" reset);
        gotoxy(43, 17);
        printf("                    ");
        gotoxy(43, 17);

        i = 0;
        while (1)
        {
            ch = getch();
            if (ch == 13)
            {
                passInput[i] = '\0';
                break;
            }
            else if (ch == 8)
            {
                if (i > 0)
                {
                    i--;
                    printf("\b \b");
                }
            }
            else
            {
                passInput[i] = ch;
                i++;
                printf("*");
            }
        }


        system("cls");

    }

    gotoxy(58, 20);
    printf(BGRN "Ready!" reset);
    Sleep(500);
}
void initializeQueue(PriorityQueue* q)
{
    q->front = NULL;
    q->size = 0;
}

void initializeProcessedList(ProcessedTasksList* list)
{
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

int isIdUnique(ProcessedTasksList* list, PriorityQueue* q, int id)
{
    for (int i = 0; i < list->size; i++)
    {
        if (list->tasks[i].id == id)
        {
            return 0;
        }
    }

    Node* current = q->front;
    while (current != NULL)
    {
        if (current->task.id == id)
        {
            return 0;
        }
        current = current->next;
    }

    return 1;
}

void addToProcessedList(ProcessedTasksList* list, Task task)
{
    if (list->size >= list->capacity)
    {
        list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->tasks = realloc(list->tasks, list->capacity * sizeof(Task));
        if (!list->tasks)
        {
            perror(""RED"Failed to allocate memory"reset);
            exit(EXIT_FAILURE);
        }
    }
    list->tasks[list->size++] = task;
}

void freeProcessedList(ProcessedTasksList* list)
{
    free(list->tasks);
    list->tasks = NULL;
    list->size = 0;
    list->capacity = 0;
}

void freeQueue(PriorityQueue* q)
{
    while (!isEmpty(q))
    {
        dequeue(q);
    }
}

int isEmpty(PriorityQueue* q)
{
    return q->front == NULL;
}

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

    if (isEmpty(q) || newTask.priority > q->front->task.priority)
    {
        newNode->next = q->front;
        q->front = newNode;
    }
    else
    {
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

Task dequeue(PriorityQueue* q)
{
    if (isEmpty(q))
    {
        gotoxy(45,5);
        printf("\t\t\t\tQueue is empty!\n");
        getchar();
    }
    Node* temp = q->front;
    Task task = temp->task;
    q->front = q->front->next;
    free(temp);
    q->size--;
    return task;
}

void displayQueue(PriorityQueue* q)
{
    clearScreen();
    if(isEmpty(q))
    {
        gotoxy(45,5);
        printf("Queue is empty!\n");

        return;
    }

    // Column width definitions
#define SNO_WIDTH    7
#define TASKID_WIDTH 9
#define DESC_WIDTH   35
#define PRIO_WIDTH   10
#define TIME_WIDTH   19
#define STATUS_WIDTH 14


    printf(BHCYN"\n\t\t\t\t\t   Current Task Queue \n"reset);
    gotoxy( 0,3);
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
           186, SNO_WIDTH-1, ""BHCYN" S.No "reset,
           186, TASKID_WIDTH-1, ""BHCYN" Task ID"reset,
           186, DESC_WIDTH-1, ""BHCYN"            Description           "reset,
           186, PRIO_WIDTH-1, ""BHCYN" Priority"reset,
           186, TIME_WIDTH-1, ""BHCYN"   Arrival Time   "reset,
           186, STATUS_WIDTH-1, ""BHCYN"   Status    "reset,
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

    while(current != NULL)
    {

        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M",
                 localtime(&current->task.arrival_time));


        char displayDesc[DESC_WIDTH+1];
        strncpy(displayDesc, current->task.description, DESC_WIDTH);
        displayDesc[DESC_WIDTH] = '\0';
        if(strlen(current->task.description) > DESC_WIDTH)
        {
            displayDesc[DESC_WIDTH-3] = '.';
            displayDesc[DESC_WIDTH-2] = '.';
            displayDesc[DESC_WIDTH-1] = '.';
        }

#define VLINE 186

        printf("%c %-*d %c %-*d %c\t      %-*s %c   %-*d %c  %-*s %c% -*s %c\n",
               VLINE, SNO_WIDTH-2,  serial++,
               VLINE, TASKID_WIDTH-2,current->task.id,
               VLINE, DESC_WIDTH-12,displayDesc,
               VLINE, PRIO_WIDTH-4,current->task.priority,
               VLINE, TIME_WIDTH-3,  timeBuf,
               VLINE, STATUS_WIDTH-1,   YEL"   Pending   "reset,
               VLINE);

        // Add row separator if not last item
        if(current->next != NULL)
        {
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



}
getchar();
void generateReport(ProcessedTasksList* processed)
{
    clearScreen();
    if (processed->size == 0)
    {
        gotoxy(45,5);
        printf(""RED"No tasks were processed.\n"reset);
        return;
    }

    // Column width definitions
#define ID_WIDTH 6
#define PRIO_WIDTH 9
#define WAIT_WIDTH 11
#define PROC_WIDTH 16
#define DESC_WIDTH 24
#define TOTAL_WIDTH (ID_WIDTH+PRIO_WIDTH+WAIT_WIDTH+PROC_WIDTH+DESC_WIDTH+13)



    gotoxy(45, 3);
    printf(BHCYN"=== TASK SCHEDULING REPORT ==="reset);


    gotoxy(7,5);

    printf("\t\t\t  %c", 201);
    for(int i=0; i<TOTAL_WIDTH-2; i++) printf("%c", 205);
    printf("%c\n", 187);

    printf("\t\t\t  %c Total tasks processed    :%-*d%c\n",
           186, TOTAL_WIDTH-29, processed->size, 186);

    double totalWait = 0, totalProc = 0;
    for(int i=0; i<processed->size; i++)
    {
        totalWait += processed->tasks[i].wait_time;
        totalProc += processed->tasks[i].processing_time;
    }

    printf("\t\t\t  %c Average wait time        :%-*.2f %c\n",
           186, TOTAL_WIDTH-30, totalWait/processed->size, 186);
    printf("\t\t\t  %c Average processing time  :%-*.2f%c\n",
           186, TOTAL_WIDTH-29, totalProc/processed->size, 186);


    printf("\t\t\t  %c", 204);
    for(int i=0; i<TOTAL_WIDTH-2; i++) printf("%c", 205);
    printf("%c\n", 185);

    // Column headers
    printf("\t\t\t  %c"BHCYN, 186);
    printf("  %-*s    %-*s     %-*s     %-*s         %-*s  "reset"%c\n",
           ID_WIDTH-2, "ID",
           PRIO_WIDTH-4, "Priority",
           WAIT_WIDTH-5, "Wait Time",
           PROC_WIDTH-5, "Processing Time",
           DESC_WIDTH-10, "Description", 186);

    // Header separator
    printf("\t\t\t  %c", 204);
    for(int i=0; i<TOTAL_WIDTH-2; i++) printf("%c", 205);
    printf("%c\n", 185);

    // Task rows
    for(int i=0; i<processed->size; i++)
    {
        Task t = processed->tasks[i];
        printf("\t\t\t  %c  %-*d  \t%-*d        %-*.2f     \t%-*.2f           \t %-*s  %c\n", 186,
               ID_WIDTH-2, t.id,
               PRIO_WIDTH-6, t.priority,
               WAIT_WIDTH-16, t.wait_time,
               PROC_WIDTH-11, t.processing_time,
               DESC_WIDTH-11, t.description, 186);
    }

    // Bottom border
    printf("\t\t\t  %c", 200);
    for(int i=0; i<TOTAL_WIDTH-2; i++) printf("%c", 205);
    printf("%c\n", 188);

}

void saveTasksToFile(ProcessedTasksList* list)
{
    FILE* file = fopen(DATA_FILE, "wb");
    if (!file)
    {
        perror(""RED"Failed to save data"reset);
        return;
    }

    fwrite(&list->size, sizeof(int), 1, file);
    fwrite(list->tasks, sizeof(Task), list->size, file);
    fclose(file);
}

void loadTasksFromFile(ProcessedTasksList* list)
{
    FILE* file = fopen(DATA_FILE, "rb");
    if (!file) return;

    int size;
    if (fread(&size, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return;
    }

    Task* tasks = malloc(size * sizeof(Task));
    if (!tasks)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    if (fread(tasks, sizeof(Task), size, file) != size)
    {
        free(tasks);
        fclose(file);
        return;
    }

    list->tasks = tasks;
    list->size = size;
    list->capacity = size;
    fclose(file);
}

void removeTask(PriorityQueue* q)
{
    clearScreen();
    if (isEmpty(q))
    {

        printf("\n\n\n\n\t\t\t\t\t"RED"Queue is empty! No tasks to remove."reset"\n");

        printf("\n\n\t\t\t\t\t   "GRN"Press Enter to continue "reset);
        for(int i=0; i<3; i++)
        {
            printf(".");
            sleep(1);
        }
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
    gotoxy(45,3);
    printf(""BHCYN"\t      == Remove Task ==\n"reset"");
    gotoxy(28,5);
    for (int i = 0; i < 33; i++)
    {
        printf("%c", 205);
    }
    gotoxy(28+34,5);
    printf(" %c ", 111);
    for (int i = 0; i < 33; i++)
        printf("%c", 205);
    printf("\n\n\n\t\t\t\tSearch by:\n");
    printf("\t\t\t\t1. Task ID\n");
    printf("\t\t\t\t2. Task Name\n\n\n");
    printf("\t\t\t\tEnter choice:");

    while (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2))
    {
        printf("\n\t\t\t\t"RED"Invalid input!"reset" "GRN"Please enter"reset ""BWHT" 1"reset" or "BWHT"2"reset" \n");
        while (getchar() != '\n');
    }
    getchar();

    if (choice == 1)
    {
        printf("\t\t\t\tEnter task ID to remove: ");
        while (scanf("%d", &taskId) != 1)
        {
            printf("\t\t\t\t"RED"Invalid input! Please enter a number: "reset);
            while (getchar() != '\n');
        }
        getchar();

        current = q->front;
        prev = NULL;

        while (current != NULL)
        {
            if (current->task.id == taskId)
            {
                found = 1;
                printf("\n\t\t\t\tFound task to remove:\n");
                printf("\t\t\t\tID         : %d\n", current->task.id);
                printf("\t\t\t\tDescription: %s\n", current->task.description);
                printf("\t\t\t\tPriority   : %d\n", current->task.priority);

                while (1)
                {
                    printf("\n\t\t\t\tAre you sure you want to remove this task? ("GRN"y"reset"/"RED"n"reset"): ");
                    scanf(" %c", &confirm);
                    getchar();
                    confirm = tolower(confirm);

                    if (confirm == 'y')
                    {
                        if (prev == NULL)
                        {
                            q->front = current->next;
                        }
                        else
                        {
                            prev->next = current->next;
                        }
                        free(current);
                        q->size--;
                        printf("\n\n\t\t\t\tTask removed successfully.\n");
                        break;
                    }
                    else if (confirm == 'n')
                    {
                        printf("\n\t\t\t\tTask removal cancelled.\n");
                        break;
                    }
                    else
                    {
                        printf("\t\t\t\t"RED"Invalid input!\n"reset);
                    }
                }
                break;
            }
            prev = current;
            current = current->next;
        }

        if (!found)
        {
            printf("\n\t\t\t\tNo task found with ID %d.\n", taskId);
            do
            {
                printf("\n\t\n\t\t\t\t"GRN"Do you want to try again? (y/n): "reset);
                if (scanf(" %c", &confirm) != 1)
                {
                    while (getchar() != '\n');
                    confirm = '     ';
                }
                getchar();
                confirm = tolower(confirm);

                if (confirm != 'y' && confirm != 'n')
                    printf("\n\t\t\t\t"RED"Invalid input!"reset" "GRN"Please enter"reset ""BWHT" 'y'"reset" or "BWHT"'n'"reset" \n");
            }
            while (confirm != 'y' && confirm != 'n');

            if (confirm == 'y')
            {
                clearScreen();
                goto removal_start;
            }
            else
            {
                printf("\n\t\t\t\t\tReturning to main menu.\n");
                printf("\n\t\t\t\t\t"GRN"Press Enter to continue..."reset);
                getchar();
                return;
            }
        }
    }
    else
    {
        printf("\t\t\t\tEnter task name to remove: ");
        fgets(taskDesc, sizeof(taskDesc), stdin);
        taskDesc[strcspn(taskDesc, "\n")] = '\0';

        current = q->front;
        prev = NULL;

        while (current != NULL)
        {
            if (strcmp(current->task.description, taskDesc) == 0)
            {
                found = 1;
                printf("\n\t\t\t\tFound task to remove:\n");
                printf("\t\t\t\tID         : %d\n", current->task.id);
                printf("\t\t\t\tDescription: %s\n", current->task.description);
                printf("\t\t\t\tPriority   : %d\n", current->task.priority);

                while (1)
                {
                    printf("\n\t\t\t\tAre you sure you want to remove this task? ("GRN"y"reset"/"RED"n"reset"): ");
                    scanf(" %c", &confirm);
                    getchar();
                    confirm = tolower(confirm);

                    if (confirm == 'y')
                    {
                        if (prev == NULL)
                        {
                            q->front = current->next;
                        }
                        else
                        {
                            prev->next = current->next;
                        }
                        free(current);
                        q->size--;
                        printf("\t\t\t\tTask removed successfully.\n");
                        break;
                    }
                    else if (confirm == 'n')
                    {
                        printf("\t\t\t\tTask removal cancelled.\n");
                        break;
                    }
                    else
                    {
                        printf("\t\t\t\t"RED"Invalid input!\n"reset);
                    }
                }
                break;
            }
            prev = current;
            current = current->next;
        }

        if (!found)
        {
            printf("\n\t\t\t\tNo task found with Name %s\n", taskDesc);
            do
            {
                printf("\n\t\n\t\t\t\t"GRN"Do you want to try again? (y/n): "reset);
                if (scanf(" %c", &confirm) != 1)
                {
                    while (getchar() != '\n');
                    confirm = '     ';
                }
                getchar();
                confirm = tolower(confirm);

                if (confirm != 'y' && confirm != 'n')
                    printf("\n\t\t\t\t"RED"Invalid input!"reset" "GRN"Please enter"reset ""BWHT" 'y'"reset" or "BWHT"'n'"reset" \n");
            }
            while (confirm != 'y' && confirm != 'n');

            if (confirm == 'y')
            {
                clearScreen();
                goto removal_start;
            }
            else
            {
                printf("\t\t\t\tReturning to main menu.\n");
                printf("\n\t\t\t\t"GRN"Press Enter to continue..."reset);
                getchar();
                return;
            }
        }
    }

    printf("\n\t\t\t\t"GRN"Press Enter to continue..."reset);
    getchar();
}

void simulateScheduler()
{
    PriorityQueue taskQueue;
    initializeQueue(&taskQueue);

    ProcessedTasksList processedTasks;
    initializeProcessedList(&processedTasks);
    loadTasksFromFile(&processedTasks);

    int choice;

    while (1)
    {
        clearScreen();
        gotoxy(45,2);
        printf("=== Task Scheduling System ===\n");
        showMenu();
        gotoxy(62,18);
        while (scanf("%d", &choice) != 1 || choice < 1 || choice > 6)
        {
            gotoxy(37,18);

            printf("\r\t\t\t\t\t"RED"Invalid input! Please enter a number between 1-6: "reset);
            while (getchar() != '\n');
        }
        getchar();
        switch (choice)
        {
case 1:
        {
            clearScreen();
            gotoxy(45,2);
            printf("    Add New Task \n");
            gotoxy(20,4);
            for (int i = 0; i < 33; i++)
            {
                printf(""BHCYN"%c"reset"", 205);
            }

            printf(" %c ", 111);
            for (int i = 0; i < 33; i++)
                printf(""BHCYN"%c"reset"", 205);
            Task newTask;
            newTask.processing_time = 0.0;
            gotoxy(40, 6);
            printf("Enter task ID                    : ");
            while (1)
            {
                if (scanf("%d", &newTask.id) == 1)
                {
                    if (isIdUnique(&processedTasks, &taskQueue, newTask.id))
                    {
                        break;
                    }
                    else
                    {

                        printf("\n\t\t\t\t"RED"ID already exists! Please enter a unique ID: "reset);
                    }
                }
                else
                {

                    printf("\t\t\t\t\t"RED"Invalid ID! Please enter a number: "reset);
                    while (getchar() != '\n');
                }
            }
            getchar();

            printf("\t\t\t\t\tEnter task description           : ");
            fgets(newTask.description, sizeof(newTask.description), stdin);
            newTask.description[strcspn(newTask.description, "\n")] = '\0';

            printf("\t\t\t\t\tEnter priority (1-10, 10=highest): ");
            while (1)
            {
                if (scanf("%d", &newTask.priority) == 1 && newTask.priority >= 1 && newTask.priority <= 10)
                    break;
                gotoxy(40,14);
                printf(""RED"Invalid priority! Please enter a value between 1-10: "reset);
                while (getchar() != '\n');
            }
            getchar();

            newTask.arrival_time = time(NULL);
            enqueue(&taskQueue, newTask);

            printf("\n\n\n\t\t\t\t    "GRN" Task added successfully! Press Enter to continue..."reset);
            getchar();
            break;
        }
case 2:
        {
            clearScreen();
            if (isEmpty(&taskQueue))
            {
                gotoxy(45,10);
                printf(""BRED"No tasks in queue!\n"reset);
            }
            else
            {
                // Processing Box
                int yPos = 2;
                gotoxy(30, yPos++);
                printf("%c", 201);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 187);
                gotoxy(30, yPos++);
                printf("%c"BHCYN"                       PROCESSING TASK                      "reset"%c\n", 186, 186);

                gotoxy(30, yPos++);
                printf("%c", 204);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 185);

                Task currentTask = dequeue(&taskQueue);
                currentTask.start_time = time(NULL);
                currentTask.wait_time = difftime(currentTask.start_time, currentTask.arrival_time);

                // Task Details
                gotoxy(30, yPos++);
                printf("%c Task ID    :   %-43d %c\n", 186, currentTask.id, 186);
                gotoxy(30, yPos++);
                printf("%c Description:   %-44s%c\n", 186, currentTask.description, 186);
                gotoxy(30, yPos++);
                printf("%c Priority   :   %-43d %c\n", 186, currentTask.priority, 186);
                gotoxy(30, yPos++);
                printf("%c Arrival    :   %-43.24s %c\n", 186, ctime(&currentTask.arrival_time), 186);
                gotoxy(30, yPos++);
                printf("%c Start Time :   %-43.24s %c\n", 186, ctime(&currentTask.start_time), 186);
                gotoxy(30, yPos++);
                printf("%c Wait Time  :   %-43.2f %c\n", 186, currentTask.wait_time, 186);

                gotoxy(30, yPos++);
                printf("%c", 200);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 188);

                gotoxy(30, yPos++);

                printf(""GRN" Press Enter when complete..."reset);

                // Wait for completion
                time_t start_real = time(NULL);
                getchar();
                time_t end_real = time(NULL);
                currentTask.processing_time = difftime(end_real, start_real);
                currentTask.completion_time = time(NULL);

                // Completion Box (appears below processing box)
                yPos += 1; // Add space between boxes
                gotoxy(30, yPos++);
                printf("%c", 201);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 187);

                gotoxy(30, yPos++);
                printf("%c"BHCYN"                      TASK COMPLETED                       "reset" %c\n", 186, 186);

                gotoxy(30, yPos++);
                printf("%c", 204);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 185);

                gotoxy(30, yPos++);
                printf("%c Completed at:%-45.24s %c\n", 186, ctime(&currentTask.completion_time), 186);
                gotoxy(30, yPos++);
                printf("%c Processing  :%-45.2f %c\n", 186, currentTask.processing_time, 186);

                gotoxy(30, yPos++);
                printf("%c", 200);
                for(int i = 0; i < 60; i++) printf("%c", 205);
                printf("%c\n", 188);

                addToProcessedList(&processedTasks, currentTask);
                saveTasksToFile(&processedTasks);
            }
            gotoxy(42,18);
            printf("\n\n\t\t\t\t\t  "GRN"Press Enter to continue..."reset);
            getchar();
            break;
        }
        case 3:
        {
            displayQueue(&taskQueue);

            printf("\n\n\t\t\t\t\t"GRN"Press Enter to continue..."reset);
            getchar();
            break;
        }
        case 4:
        {
            generateReport(&processedTasks);
            printf("\n\n\t\t\t\t\t    "GRN"Press Enter to continue..."reset);
            getchar();
            break;
        }
        case 5:
        {
            removeTask(&taskQueue);
            break;
        }
        case 6:
        {
            system("cls");
            freeQueue(&taskQueue);
            freeProcessedList(&processedTasks);

            gotoxy(37, 10);

            printf("\t\tFinalizing...    \t");

            Sleep(500); // Sleep for 1 second (1000 milliseconds)

            for(int i=0; i<=100; i++)

            {
                gotoxy(65,10);

                printf("%d",i);

                printf("%%\n");

                Sleep(20);
            }

            Sleep(800);

            printf("\n");

            gotoxy(45, 12);

            printf("TASK COMPLETED SUCCESSFULLY!");

            Sleep(1500);

            system("cls");



            char lines[] =
                "\n\n\n\t\t\t  TTTTT   H   H   AAA   N   N  K   K     Y   Y   OOO   U   "
                "U\n"
                "\t\t\t    T     H   H  A   A  NN  N  K  K       Y Y   O   O  U   U\n"
                "\t\t\t    T     HHHHH  AAAAA  N N N  KKK         Y    O   O  U   U\n"
                "\t\t\t    T     H   H  A   A  N  NN  K  K        Y    O   O  U   U\n"
                "\t\t\t    T     H   H  A   A  N   N  K   K       Y     OOO    UUU\n";


            printf(""BHCYN"%s\n"reset"", lines);

            Sleep(4000);
            return;
        }
        }
    }
}
