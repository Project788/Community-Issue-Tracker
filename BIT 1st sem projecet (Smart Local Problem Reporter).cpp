#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct issue {
    int id;
    char name[50];
    char location[50];
    char problem[100];
    char status[20];
    char date[15];
};

/* generate id */
int generateID() {
    FILE *fp;
    struct issue temp;
    int lastId = 100;

    fp = fopen("issues.dat", "rb");

    if (fp == NULL)
        return 101;

    while (fread(&temp, sizeof(temp), 1, fp)) {
        lastId = temp.id;
    }

    fclose(fp);
    return lastId + 1;
}

/* report issue */
void addIssue() {

    FILE *fp;
    struct issue newIssue;

    fp = fopen("issues.dat", "ab");

    if (fp == NULL) {
        printf("File error\n");
        return;
    }

    newIssue.id = generateID();

    getchar();

    printf("\nEnter Your Name: ");
    fgets(newIssue.name, 50, stdin);
    newIssue.name[strcspn(newIssue.name, "\n")] = 0;

    printf("Enter Location: ");
    fgets(newIssue.location, 50, stdin);
    newIssue.location[strcspn(newIssue.location, "\n")] = 0;

    printf("Enter Problem: ");
    fgets(newIssue.problem, 100, stdin);
    newIssue.problem[strcspn(newIssue.problem, "\n")] = 0;

    printf("Enter Date (DD/MM/YYYY): ");
    fgets(newIssue.date, 15, stdin);
    newIssue.date[strcspn(newIssue.date, "\n")] = 0;

    strcpy(newIssue.status, "Pending");

    fwrite(&newIssue, sizeof(newIssue), 1, fp);

    fclose(fp);

    printf("\nIssue reported successfully\n");
    printf("Your Issue ID is %d\n", newIssue.id);
}

/* view issues */
void viewIssues() {
    FILE *fp;
    struct issue temp;

    fp = fopen("issues.dat", "rb");
    if (fp == NULL) {
        printf("No issues found\n");
        return;
    }

    printf("\n---------------------------------------------------------------------------------------------------\n");
    printf("ID\tDate\t\tName\t\t\tLocation\t\t\tProblem\t\t\tStatus\n");
    printf("---------------------------------------------------------------------------------------------------\n");

    while (fread(&temp, sizeof(struct issue), 1, fp)) {
        // truncate strings if too long for table view
        char name[21], loc[21], prob[21];
        strncpy(name, temp.name, 20); name[20] = '\0';
        strncpy(loc, temp.location, 20); loc[20] = '\0';
        strncpy(prob, temp.problem, 20); prob[20] = '\0';

        printf("%-4d\t%-10s\t%-20s\t%-20s\t%-20s\t%-10s\n",
               temp.id, temp.date, name, loc, prob, temp.status);
    }

    fclose(fp);
}

/* search issue */
void searchIssue() {

    FILE *fp;
    struct issue temp;
    int id;
    int found = 0;

    fp = fopen("issues.dat", "rb");

    if (fp == NULL) {
        printf("No issues available\n");
        return;
    }

    printf("Enter Issue ID: ");
    scanf("%d", &id);

    while (fread(&temp, sizeof(temp), 1, fp)) {

        if (temp.id == id) {

            printf("\nIssue Found\n");
            printf("ID: %d\n", temp.id);
            printf("Name: %s\n", temp.name);
            printf("Location: %s\n", temp.location);
            printf("Problem: %s\n", temp.problem);
            printf("Date: %s\n", temp.date);
            printf("Status: %s\n", temp.status);

            found = 1;
            break;
        }
    }

    if (!found)
        printf("Issue not found\n");

    fclose(fp);
}

/* update status */
void updateStatus() {

    FILE *fp;
    struct issue temp;
    int id;
    int found = 0;

    fp = fopen("issues.dat", "rb+");

    if (fp == NULL) {
        printf("No issues available\n");
        return;
    }

    printf("Enter Issue ID to update: ");
    scanf("%d", &id);

    while (fread(&temp, sizeof(temp), 1, fp)) {

        if (temp.id == id) {

            printf("Current Status: %s\n", temp.status);

            getchar();

            printf("Enter New Status (Pending / In Progress / Solved): ");
            fgets(temp.status, 20, stdin);
            temp.status[strcspn(temp.status, "\n")] = 0;

            fseek(fp, -(long)sizeof(temp), SEEK_CUR);

            fwrite(&temp, sizeof(temp), 1, fp);

            printf("Status updated\n");

            found = 1;
            break;
        }
    }

    if (!found)
        printf("Issue not found\n");

    fclose(fp);
}

/* delete issue */
void deleteIssue() {

    FILE *fp, *tempFile;
    struct issue temp;
    int id;
    int found = 0;

    fp = fopen("issues.dat", "rb");
    tempFile = fopen("temp.dat", "wb");

    if (fp == NULL) {
        printf("No issues available\n");
        return;
    }

    printf("Enter Issue ID to delete: ");
    scanf("%d", &id);

    while (fread(&temp, sizeof(temp), 1, fp)) {

        if (temp.id != id)
            fwrite(&temp, sizeof(temp), 1, tempFile);
        else
            found = 1;
    }

    fclose(fp);
    fclose(tempFile);

    remove("issues.dat");
    rename("temp.dat", "issues.dat");

    if (found)
        printf("Issue deleted\n");
    else
        printf("Issue not found\n");
}

/* statistics */
void statistics() {

    FILE *fp;
    struct issue temp;

    int total = 0, pending = 0, progress = 0, solved = 0;

    char problems[100][100];
    int count[100];
    int size = 0;

    fp = fopen("issues.dat", "rb");

    if (fp == NULL) {
        printf("No issues available\n");
        return;
    }

    while (fread(&temp, sizeof(temp), 1, fp)) {

        total++;

        if (strcmp(temp.status, "Pending") == 0)
            pending++;

        else if (strcmp(temp.status, "In Progress") == 0)
            progress++;

        else if (strcmp(temp.status, "Solved") == 0)
            solved++;

        int found = 0;

        for (int i = 0; i < size; i++) {

            if (strcmp(problems[i], temp.problem) == 0) {
                count[i]++;
                found = 1;
                break;
            }
        }

        if (!found) {
            strcpy(problems[size], temp.problem);
            count[size] = 1;
            size++;
        }
    }

    fclose(fp);

    if (size == 0) {
        printf("No issues available\n");
        return;
    }

    int maxIndex = 0;

    for (int i = 1; i < size; i++) {
        if (count[i] > count[maxIndex])
            maxIndex = i;
    }

    printf("\n--- Issue Statistics ---\n");
    printf("Total Issues : %d\n", total);
    printf("Pending      : %d\n", pending);
    printf("In Progress  : %d\n", progress);
    printf("Solved       : %d\n", solved);
    printf("Most Common Problem : %s\n", problems[maxIndex]);
}

/* admin login */
int adminLogin() {

    char password[20];

    printf("Enter Admin Password: ");
    scanf("%s", password);

    if (strcmp(password, "admin123") == 0) {
        printf("Login successful\n");
        return 1;
    }

    printf("Wrong password\n");
    return 0;
}

/* reporter menu */
void reporterMenu() {

    int choice;

    do {

        printf("\n--- Reporter Menu ---\n");
        printf("1. Report Issue\n");
        printf("2. View Issues\n");
        printf("3. Search Issue\n");
        printf("4. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1: addIssue(); break;
            case 2: viewIssues(); break;
            case 3: searchIssue(); break;

            case 4: break;

            default: printf("Invalid choice\n");
        }

    } while (choice != 4);
}

/* admin menu */
void adminMenu() {

    int choice;

    do {

        printf("\n--- Admin Menu ---\n");
        printf("1. View Issues\n");
        printf("2. Update Status\n");
        printf("3. Delete Issue\n");
        printf("4. Statistics\n");
        printf("5. Logout\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1: viewIssues(); break;
            case 2: updateStatus(); break;
            case 3: deleteIssue(); break;
            case 4: statistics(); break;

            case 5: break;

            default: printf("Invalid choice\n");
        }

    } while (choice != 5);
}

/* main */
int main() {

    int choice;

    printf("\nSmart Local Problem Reporter System\n");
    printf("-----------------------------------\n");

    do {

        printf("\n===== Main Menu =====\n");
        printf("1. Reporter\n");
        printf("2. Admin\n");
        printf("3. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                reporterMenu();
                break;

            case 2:
                if (adminLogin())
                    adminMenu();
                break;

            case 3:
                printf("Program closed\n");
                break;

            default:
                printf("Invalid choice\n");
        }

    } while (choice != 3);

    return 0;
}
