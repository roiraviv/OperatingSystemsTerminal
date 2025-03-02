/* myFunctionsShell.c */
#include "myFunctionsShell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *inputFromUser() {
    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    return input;
}

void loadingBar(int step, int totalSteps, char *header) {
    printf("%s\n", header);
    for (int i = 0; i < step; i++) {
        printf("#");
        fflush(stdout);
        usleep(50000);
    }
    printf("\n");
}

void printPixelArt() {
    printf("ASCII Pixel Art\n");
}

void welcome() {
    printf("Welcome to MyShell\n");
}

void logout() {
    printf("Logging out...\n");
}

void getLocation() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current Directory: %s\n", cwd);
}

void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument for cd\n");
    } else {
        chdir(args[1]);
    }
}

bool executeCommand(char *input) {
    char *args[64];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    if (args[0] == NULL) return true;
    if (strcmp(args[0], "exit") == 0) return false;
    
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("Error");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
    return true;
}
