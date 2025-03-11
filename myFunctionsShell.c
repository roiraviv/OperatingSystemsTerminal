#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include "myFunctionsShell.h"


// ----------------------------------------------
// Managing input from user terminal
// ----------------------------------------------

char *getUserInput() {
    char ch;
    int length = 0;
    char *input = (char *)malloc(sizeof(char));
    input[0] = '\0';

    while ((ch = getchar()) != '\n') {
        input[length++] = ch;
        input = (char *)realloc(input, sizeof(char) * (length + 1));
    }
    input[length] = '\0';

    return input;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Initialization and stop functions
// ---------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------
// Loading bar effect for program start and stop
// ---------------------------------------

void displayLoadingBar(int step, int totalSteps, char *header) {
    printf("\033[H\033[J");

    printf("\033[1;35m");
    printf("\n%s\n\n", header);
    printf("\033[0;32m[");

    int loaded = (step * 100) / totalSteps;
    int totalHashes = 40;

    int numHashes = (loaded * totalHashes) / 100;
    for (int i = 0; i < totalHashes; i++) {
        if (i < numHashes) {
            printf("#");
        } else {
            printf(".");
        }
    }

    printf("]\033[0m");
    printf(" %d%%\n", loaded);
}

// ---------------------------------------
// Display pixel art on program startup
// ---------------------------------------

void displayPixelArt() {
    char *art[] = {
        "  RRRRR      OOOO    IIIIII  ",
        "  R    R    O    O     II    ",
        "  RRRRR     O    O     II    ",
        "  R    R    O    O     II    ",
        "  R     R    OOOO    IIIIII  ",
    };
    char *greetings[] = {
        "   =>   | \033[1;35mShell Started!\033",
        "   =>   | ",
        "   =>   | \033[1;34mYou can type any shell command\033",
        "   =>   | \033[0;33m  or\033",
        "   =>   | \033[1;36mType 'exit' to quit.\033",
    };
    int rows = sizeof(art) / sizeof(art[0]);
    for (int i = 0; i < rows; i++) {
        printf("\033[1;31m%s", art[i]);
        printf("%s\n", greetings[i]);
    }
    puts("\n");
}

// ---------------------------------------
// Welcome function displaying loading bar and pixel art
// ---------------------------------------

void welcome() {
    int totalSteps = 100;
    for (int i = 1; i <= totalSteps; i++) {
        displayLoadingBar(i, totalSteps, "Loading");
        usleep(20000);
    }
    printf("\n\n");
    displayPixelArt();

    while(1) {
        showCurrentLocation();

        char *input = getUserInput();

        if(!processCommand(input)){
            free(input);
            break;
        };
        free(input);
    }
}

// Define the processCommand function
bool processCommand(char *input) {
    // Add your logic for parsing and executing commands here
    // For now, let's just check for 'exit' and return false to stop
    if (strcmp(input, "exit") == 0) {
        return false;  // Exit the shell
    }
    
    // If the input is not "exit", just print it for now
    printf("Command received: %s\n", input);
    return true;  // Continue running the shell
}

// ---------------------------------------
// Logout function with a loading bar
// ---------------------------------------

void logout() {
    for (int i = 1; i <= 100; i++) {
        displayLoadingBar(i, 100, "Logging out");
        usleep(20000);
    }
    printf("\n\n\n");
}

// ---------------------------------------
// Function to show current location in the shell
// ---------------------------------------

void showCurrentLocation() {
    char location[256];
    char hostname[256];
    char *username;

    username = getenv("USER");
    if (username == NULL) {
        username = "Guest";
    }

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Error retrieving hostname");
        return;
    }
    
    if (getcwd(location, sizeof(location)) == NULL) {
        perror("Error retrieving location");
        return;
    }

    printf("\033[1;32m");
    printf("%s@%s", username, hostname);
    printf("\033[0;30m");
    printf(":%s", location);
    printf("\033[0m$ ");
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------
// Handle "cd" command - change directory
// ----------------------------

void changeDirectory(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: No directory specified\n");
        return;
    } else if (args[2] != NULL){
        fprintf(stderr, "cd: too many arguments\n");
        return;
    } else {
        if (chdir(args[1]) != 0) {
            perror("Error - Can't change directory");
            return;
        }
    }
}

// ----------------------------
// Handle "cp" command - copy file
// ----------------------------

void copyFile(char *src, char *dest) {
    FILE *source = fopen(src, "rb");
    if (source == NULL) {
        perror("cp: source error");
        return;
    }

    FILE *destination = fopen(dest, "wb");
    if (destination == NULL) {
        perror("cp: destination error");
        fclose(source);
        return;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);
}

// ----------------------------
// Handle "cp" command - copy directory
// ----------------------------

void copyDirectory(char *src, char *dest) {
    struct stat st;
    if (stat(dest, &st) == -1) {
        if (mkdir(dest, 0755) != 0) {
            perror("cp: mkdir error");
            return;
        }
    }

    DIR *dir = opendir(src);
    if (!dir) {
        perror("cp: source directory error");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcPath[1024];
        char destPath[1024];

        snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", dest, entry->d_name);

        if (stat(srcPath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                copyDirectory(srcPath, destPath);
            } else if (S_ISREG(st.st_mode)) {
                copyFile(srcPath, destPath);
            }
        }
    }
    closedir(dir);
}

// ----------------------------
// Handle "cp" command - copy
// ----------------------------

void copy(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "cp: source and destination required\n");
        return;
    }

    struct stat st;
    if (stat(args[1], &st) != 0) {
        perror("cp: source error");
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        copyDirectory(args[1], args[2]);
        printf("Directory copied successfully\n");
        return;
    } else if (S_ISREG(st.st_mode)) {
        copyFile(args[1], args[2]);
        printf("File copied successfully\n");
        return;
    } else {
        fprintf(stderr, "cp: unsupported file type\n");
        return;
    }
}

// ----------------------------
// Handle "rm -r" command - remove directory recursively
// ----------------------------

void recursiveDelete(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("rm: Error accessing path");
        return;
    }

    if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
        if (unlink(path) != 0) {
            perror("rm: Error deleting file");
        }
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            perror("rm: Error opening directory");
            return;
        }

        struct dirent *entry;
        char fullPath[1024];
        
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

            recursiveDelete(fullPath);
        }

        closedir(dir);

        if (rmdir(path) != 0) {
            perror("rm: Error removing directory");
        }
    }
}

// ----------------------------
// Handle "rm" command - remove
// ----------------------------

void delete(char **args) {
    if (args[2] == NULL) {
        struct stat st;
        if (stat(args[1], &st) != 0) {
            perror("rm: source error");
            return;
        }
        if (S_ISDIR(st.st_mode)) {
            printf("Error - rm: cannot remove %s: is a directory\n", args[1]);
            return;
        } else if (S_ISREG(st.st_mode)) {
            if (unlink(args[1]) != 0) {
                perror("Error deleting file");
                return;
            }
        } else {
            fprintf(stderr, "rm: cannot remove file or directory\n");
            return;
        }
    } else {
        if (strcmp(args[1], "-r") == 0) {
            recursiveDelete(args[2]);
        }
    }
}

// ----------------------------
// Handle "pipe" command
// ----------------------------

void handlePipe(char **argv1, char **argv2) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        if (fork() == 0) {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            if (execvp(argv1[0], argv1) == -1) {
                perror("execvp error");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }

        close(STDIN_FILENO);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        if (execvp(argv2[0], argv2) == -1) {
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);
}
