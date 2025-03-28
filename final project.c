#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // for Sleep and cls
#define MAX_STUDENTS 100
#define FILENAME "student_records.txt"
#include "colour.h"
// Struct definition
typedef struct
{
    int id;
    char name[50];
    float marks;
    int attendance;
} Student;

// Global variables
Student students[MAX_STUDENTS];
int studentCount = 0;

// Function prototypes
void printGoodbye();
void addStudent();
void updateStudent(int index);
void deleteStudent();
void displayStudents();
void searchStudent();
void saveRecordsToFile();
void loadRecordsFromFile();
int findStudentById(int id);
int findStudentByName(const char *name);
void loadDemoStudents();
void menu();
void sortStudentsById();
void loadingBar();
void finalize();
// gotoxy function definition
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Main function //Emran
int main()
{
    loadRecordsFromFile();
    sortStudentsById();
    saveRecordsToFile();
    if (studentCount == 0)
    {
        loadDemoStudents();
    }
    loadingBar();
    menu();
    return 0;
}

// Loading bar implementation
void loadingBar()
{
    printf("\n\n\n\t\t\t\t\t\t Loading:-\n");

    char a = 45, b = 254;
    printf("\n\t\t\t\t\t");
    for (int i = 0; i < 26; i++)
        printf("%c", a);
    printf("\r\t\t\t\t\t");
    for (int i = 0; i < 26; i++)
    {
        printf("%c", b);
        Sleep(100);
    }
    printf("\n");
}

// Demo student records
void loadDemoStudents()
{
    students[0] = (Student)
    {
        72, "Ayesha", 85.5, 90
    };
    students[1] = (Student)
    {
        94, "Saiem", 78.0, 85
    };
    students[2] = (Student)
    {
        687, "Jawaid", 92.0, 95
    };
    students[3] = (Student)
    {
        303, "Shadman", 88.5, 89
    };
    students[4] = (Student)
    {
        884, "Imran", 99.8, 91
    };
    studentCount = 5;
    //printf("Demo students loaded successfully.\n");
}
// Function to sort students by ID (Bubble Sort)// Function to sort students by
// ID (Bubble Sort)
void sortStudentsById()
{
    for (int i = 0; i < studentCount - 1; i++)
    {
        int swapped = 0; // To optimize sorting if already sorted
        for (int j = 0; j < studentCount - 1 - i; j++)
        {
            if (students[j].id > students[j + 1].id)
            {
                // Swap the students
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
                swapped = 1; // Indicate a swap occurred
            }
        }
        if (!swapped)
        {
            break; // If no swaps occurred, the array is already sorted
        }
    }
}

// Add student implementation //A.B.M Saiem
void addStudent()
{
    if (studentCount >= MAX_STUDENTS)
    {
        printf("Student list is full. Cannot add more students.\n");
        return;
    }
    printf("\n\n\n\t\t\t\t\t\t   Add New Record  \n\n\t\t\t");

    for (int i = 0; i < 33; i++)
    {
        printf("%c", 205);
    }

    printf(" %c ", 111);
    for (int i = 0; i < 33; i++)
        printf("%c", 205);
    int id;
    printf("\n\n\t\t\tEnter student ID\t\t\t:\t");
    scanf("%d", &id);

    int index = findStudentById(id);
    if (index != -1)
    {
        printf(
            "\n\n\t\t\t\tStudent with ID %d already exists. Updating record.\n\n",
            id);
        updateStudent(index);
    }
    else
    {

        Student newStudent;
        newStudent.id = id;
        printf("\t\t\tEnter name\t\t\t\t:\t");
        scanf(" %[^\n]", newStudent.name);
        printf("\t\t\tEnter marks\t\t\t\t:\t");
        scanf("%f", &newStudent.marks);
        printf("\t\t\tEnter attendance (percentage) \t\t: \t");
        scanf("%d", &newStudent.attendance);
        students[studentCount++] = newStudent;
        printf("\n\n\n\t\t\t\t\tNew student added!\n\n\n");
        printf("\t\t\t\t\tReturning to menu...");
        Sleep(3000);
    }
    sortStudentsById();

    saveRecordsToFile();
}
// Update student implementation
void updateStudent(int index)
{
    printf("Enter new name: ");
    scanf(" %[^\n]", students[index].name);
    printf("Enter marks: ");
    scanf("%f", &students[index].marks);
    printf("Enter attendance (percentage): ");
    scanf("%d", &students[index].attendance);
    printf("\nStudent record updated.\n\n");
    printf("Returning to menu...");
    Sleep(2000);
    saveRecordsToFile();
}

// Delete student implementation    //abedin
void deleteStudent()
{
    int id;
    printf("Enter the ID of the student to delete: ");
    scanf("%d", &id);

    int index = findStudentById(id);
    if (index == -1)
    {
        printf("Student with ID %d not found.\n\n\n\n", id);
        printf("\n\t\t\t\t Returning to main menu...\n");
        Sleep(5000);
        return;
    }

    for (int i = index; i < studentCount - 1; i++)
    {
        students[i] = students[i + 1];
    }
    studentCount--;
    printf("Student with ID %d deleted successfully.\n", id);
    Sleep(2000);
}

// Display students
void displayStudents()
{
    if (studentCount == 0)
    {
        printf("No student records found.\n");
        printf("Returning...");
        Sleep(2000);
        return;
    }

    // Print header for the table
    printf(BGRN"\n\t\t\t   Student Records \n"reset);

    // Top border
    printf("%c", 201); // ┌
    for (int i = 0; i < 7; i++) printf("%c", 205); // ─
    printf("%c", 203); // ┬
    for (int i = 0; i < 9; i++) printf("%c", 205); // ─
    printf("%c", 203); // ┬
    for (int i = 0; i < 35; i++) printf("%c", 205); // ─
    printf("%c", 203); // ┬
    for (int i = 0; i < 10; i++) printf("%c", 205); // ─
    printf("%c", 203); // ┬
    for (int i = 0; i < 14; i++) printf("%c", 205); // ─
    printf("%c\n", 187); // ┐

    // Header row
    printf("%c %-5s %c %-7s %c %-33s %c %-8s %c %-12s %c\n", 186, "S.No", 186, "ID", 186, "Name", 186, "Marks", 186, "Attendance", 186);

    // Header separator
    printf("%c", 204); // ├
    for (int i = 0; i < 7; i++) printf("%c", 205); // ─
    printf("%c", 206); // ┼
    for (int i = 0; i < 9; i++) printf("%c", 205); // ─
    printf("%c", 206); // ┼
    for (int i = 0; i < 35; i++) printf("%c", 205); // ─
    printf("%c", 206); // ┼
    for (int i = 0; i < 10; i++) printf("%c", 205); // ─
    printf("%c", 206); // ┼
    for (int i = 0; i < 14; i++) printf("%c", 205); // ─
    printf("%c\n", 185); // ┤

    // Print each student's record with serial numbers
    for (int i = 0; i < studentCount; i++)
    {
        printf("%c %-5d %c %-7d %c %-33s %c %-8.2f %c %-12d %c\n",
               186, i + 1,       // Serial number
               186, students[i].id,
               186, students[i].name,
               186, students[i].marks,
               186, students[i].attendance,
               186);

        // Add a line separator between rows (except the last one)
        if (i < studentCount - 1)
        {
            printf("%c", 204); // ├
            for (int i = 0; i < 7; i++) printf("%c", 205); // ─
            printf("%c", 206); // ┼
            for (int i = 0; i < 9; i++) printf("%c", 205); // ─
            printf("%c", 206); // ┼
            for (int i = 0; i < 35; i++) printf("%c", 205); // ─
            printf("%c", 206); // ┼
            for (int i = 0; i < 10; i++) printf("%c", 205); // ─
            printf("%c", 206); // ┼
            for (int i = 0; i < 14; i++) printf("%c", 205); // ─
            printf("%c\n", 185); // ┤
        }
    }

    // Bottom border
    printf("%c", 200); // └
    for (int i = 0; i < 7; i++) printf("%c", 205); // ─
    printf("%c", 202); // ┴
    for (int i = 0; i < 9; i++) printf("%c", 205); // ─
    printf("%c", 202); // ┴
    for (int i = 0; i < 35; i++) printf("%c", 205); // ─
    printf("%c", 202); // ┴
    for (int i = 0; i < 10; i++) printf("%c", 205); // ─
    printf("%c", 202); // ┴
    for (int i = 0; i < 14; i++) printf("%c", 205); // ─
    printf("%c\n", 188); // ┘


    printf("\n\t\t    Press 'ENTER' to continue...");

    getchar();

    getchar();
}


// Search student//jawaid
void searchStudent()
{
    int choice, id;
    char name[50];

    printf("Search by:\n\n\t1.ID\n\t2. Name\n\nEnter choice: \t");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter ID: \t");
        scanf("%d", &id);
        int index = findStudentById(id);
        if (index != -1)
        {
            printf("\n\nName\t:\t%s\n", students[index].name);
            printf("ID\t\t\t:\t%d\n", students[index].id);
            printf("Marks\t\t:\t%.2f\n", students[index].marks);
            printf("Attendance\t:\t%d%%\n", students[index].attendance);
        }
        else
        {
            printf("Student not found.\n");
        }

    }
    else if (choice == 2)
    {
        printf("Enter Name: ");
        scanf(" %[^\n]", name);
        int index = findStudentByName(name);
        if (index != -1)
        {
            printf("\n\nName\t\t:\t%s\n", students[index].name);

            printf("ID\t\t:\t%d\n", students[index].id);

            printf("Marks\t\t:\t%.2f\n", students[index].marks);

            printf("Attendance\t:\t%d%%\n", students[index].attendance);
        }
        else
        {
            printf("Student not found.\n");
        }
    }
    else
    {
        printf("Invalid choice.\n");
    }

    printf("\n\n\nPress Enter to continue...");
    getchar();
    getchar();
}

// Save and load records
void saveRecordsToFile()
{
    FILE *file = fopen(FILENAME, "w");
    if (!file)
    {
        perror("Error saving records to file");
        return;
    }

    for (int i = 0; i < studentCount; i++)
    {
        fprintf(file, "%d,%s,%.2f,%d\n", students[i].id, students[i].name,
                students[i].marks, students[i].attendance);
    }
    fclose(file);
}

void loadRecordsFromFile()
{
    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        return;
    }

    while (fscanf(file, "%d,%49[^,],%f,%d\n", &students[studentCount].id,
                  students[studentCount].name, &students[studentCount].marks,
                  &students[studentCount].attendance) == 4)
    {
        studentCount++;
    }
    fclose(file);
}
// Menu with a border//shadman
void menu()
{
    int choice;
    char a = 254;
    do
    {
        system("cls");
        printf("\n");
        printf("\n\n\n");
        printf("\t\t\t\t  %c", 201);
        for (int i = 0; i < 44; i++)
            printf("%c", 205);
        printf("%c\n", 187);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf( "  \t\t\t\t  %c"BGRN         "         STUDENT RECORD SYSTEM MENU"reset"         %c\n", 186,
                186);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c", 204);
        for (int i = 0; i < 44; i++)
            printf("%c", 205);
        printf("%c\n", 185);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c    1. Add/Update Student                   %c\n", 186,
               186);
        printf("\t\t\t\t  %c    2. Delete Student                       %c\n", 186,
               186);
        printf("\t\t\t\t  %c    3. Display Students                     %c\n", 186,
               186);
        printf("\t\t\t\t  %c    4. Search Student                       %c\n", 186,
               186);
        printf("\t\t\t\t  %c    5. Save Records                         %c\n", 186,
               186);
        printf("\t\t\t\t  %c    6. Exit                                 %c\n", 186,
               186);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c                                            %c\n", 186,
               186);
        printf("\t\t\t\t  %c", 200);
        for (int i = 0; i < 44; i++)
            printf("%c", 205);
        printf("%c\n", 188);
        gotoxy(40, 18);
        printf("%c Enter your choice: ", a);
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("cls");
            addStudent();
            break;
        case 2:
            system("cls");
            deleteStudent();
            break;
        case 3:
            system("cls");
            displayStudents();
            break;
        case 4:
            system("cls");
            searchStudent();
            break;
        case 5:
            system("cls");
            printf("Saving records and returning...");
            Sleep(2000);
            saveRecordsToFile();
            break;
        case 6:
            system("cls");
            saveRecordsToFile();
            printGoodbye();
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    while (choice != 6);
}
// Goodbye message
// void printLine(char *line) { printf("%s\n", line); }


void printGoodbye()
{


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
        BCYN "\n\n\n\t\t\t  TTTTT   H   H   AAA   N   N  K   K     Y   Y   OOO   U   "
        "U\n"
        "\t\t\t    T     H   H  A   A  NN  N  K  K       Y Y   O   O  U   U\n"
        "\t\t\t    T     HHHHH  AAAAA  N N N  KKK         Y    O   O  U   U\n"
        "\t\t\t    T     H   H  A   A  N  NN  K  K        Y    O   O  U   U\n"
        "\t\t\t    T     H   H  A   A  N   N  K   K       Y     OOO    UUU\n"reset;


    printf("%s\n", lines);

    Sleep(4000);
}

// Helper functions
int findStudentById(int id)
{

    for (int i = 0; i < studentCount; i++)
    {
        if (students[i].id == id)
        {
            return i;
        }
    }
    return -1;
}


int findStudentByName(const char *name)
{

    for (int i = 0; i < studentCount; i++)
    {
        if (strcmp(students[i].name, name) == 0)
        {
            return i;
        }
    }

    return -1;
}
