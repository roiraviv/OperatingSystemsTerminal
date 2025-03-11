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
// managing input from user terminal
// ----------------------------------------------

char *inputFromUser() {
    char ch;
    int len = 0;
    char *input = (char *)malloc(sizeof(char));
    input[0] = '\0';

    while ((ch = getchar()) != '\n') {
        input[len++] = ch;
        input = (char *)realloc(input, sizeof(char) * (len + 1));
    }
    input[len] = '\0';

    return input;
}


// ---------------------------------------------------------------------------------------------------------------------------------------
// initiation and stop functions
// ---------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------
// the loading bar effect on the start and at the end of the program
// ---------------------------------------

void loadingBar(int step, int totalSteps, char *header) {
    printf("\033[H\033[J");

    printf("\033[1;35m");
    printf("\n%s\n\n",header);
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
// the picel art displayed on the start of the program
// ---------------------------------------

void printPixelArt() {
    // Pixel art and greeting messages
    char *art[] = {
        "  RRRRR     OOOO   IIII  ",
        "  R    R   O    O   II    ",
        "  RRRRR    O    O   II    ",
        "  R    R   O    O   II    ",
        "  R     R   OOOO   IIII  ",
    };
    char *greetings[] = {
        "   =>    | \033[1;32mShell Started!\033", // Green color
        "   =>   | ",
        "   =>   | \033[1;36mType shell commands\033", // Cyan color
        "   =>   | \033[0;33mOr\033", // Yellow color
        "   =>    | \033[1;31mType 'exit' to quit.\033", // Red color
    };

    // Loop through the arrays and print the art and greetings
    for (int i = 0; i < 5; i++) {
        printf("\033[1;34m%s", art[i]);  // Blue color for art
        printf("%s\n", greetings[i]);
    }
    puts("\n");
}


// ---------------------------------------
// the initial function called when the program begins, calling the loadingBar function, the printPixelArt function and print few welcome lines
// ---------------------------------------

void welcome() {
    // the actual effect for the loading bar according to steps and sleep effect between each step
    int totalSteps = 100;
    for (int i = 1; i <= totalSteps; i++) {
        loadingBar(i, totalSteps, "Loading");
        usleep(20000);
    }
    printf("\n\n");
    // creates the pixel art
    printPixelArt();
    
    while(1){
        getLocation();

        char *input = inputFromUser();

        // => check if any of the arguments of the splited input is "exit" if so then ignoring the rest arguments and exiting program
        if(!executeCommand(input)){
            free(input);
            break;
        };
        free(input);
    }
}


// ---------------------------------------
// function that stops the program
// ---------------------------------------

void logout(){
    // closing the terminal - loading bar effect
    for (int i = 1; i <= 100; i++) {
        loadingBar(i, 100, "Loging out");
        usleep(20000);
    }
    printf("\n\n\n");
}

// ---------------------------------------
// function to get the current location of the program
// ---------------------------------------

void getLocation() {
    char location[256];
    char hostname[256];
    char *username;

    username = getenv("USER");//get the value of the user environment
    if (username == NULL) {
        username = "Guest";
        // if couldn't get to that environment ||
        // the user has not been saved to the environments ||
        // something made the user to be NULL =>
        // user would be Guest 
    }

    // gethostname() => retrives the computer name
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Error retrieving hostname");
        return;
    }
    
    // gethostname() => retrives the absolute path of the current working directory
    if (getcwd(location, sizeof(location)) == NULL) {
        perror("Error retrieving location");
        return;
    }

    // the line printed before the user inputs field
    printf("\033[1;32m");
    printf("%s@%s", username, hostname);
    printf("\033[0;30m");
    printf(":%s", location);
    printf("\033[0m$ ");

}

// ---------------------------------------------------------------------------------------------------------------------------------------
// commands
// ---------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------
// handle cd command - change directory
// ----------------------------

void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: No directory specified\n");
        return;
    } else if (args[2] != NULL){
        fprintf(stderr, "cd: too many arguments\n");
        return;
    }else {
        if (chdir(args[1]) != 0) {
            perror("Error - Can't change directory");
            return;
        }
    }
}

// ----------------------------
// for cp command - copy file handling
// ----------------------------

void copyFile(char *src, char *dest) {
    FILE *source = fopen(src, "rb");// => open file to read
    if (source == NULL) {
        perror("cp: source error");
        return;
    }

    FILE *destination = fopen(dest, "wb");// => open file to write
    if (destination == NULL) {
        perror("cp: destination error");
        fclose(source);//make sure to close src file
        return;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);// => the actual copy from the src to destination
    }

    // make sure of closing them both when done
    fclose(source);
    fclose(destination);
}

// ----------------------------
// for cp command - copy directory handling
// ----------------------------

void copyDirectory(char *src, char *dest) {
    struct stat st;// => system structure that stores information about a file (e.g., size, permissions, type, timestamps).
    if (stat(dest, &st) == -1) {// => set the pointer st to the destination path, if the stat of it is -1 then the path isn't exist
        if (mkdir(dest, 0755) != 0) {// create the destination path directory with permissions of: owner-read/write/execute | others-read/execute.
            perror("cp: mkdir error");// => if creation faild
            return;
        }
    }

    DIR *dir = opendir(src);// => open directory for reading
    if (!dir) {// => if src invalid/doesn't exist/no permission
        perror("cp: source directory error");
        return;
    }

    struct dirent *entry;// => set a pointer to a directory
    while ((entry = readdir(dir)) != NULL) {// read current directory pointer
        // ignore of the current and previous directories to avoid double check
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcPath[1024];
        char destPath[1024];

        snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);// => set the src/d_name string as the srcPath, prevents buffer overflow with sizeof
        snprintf(destPath, sizeof(destPath), "%s/%s", dest, entry->d_name);// => set the src/d_name string as the srcPath, ensure buffer safety by limiting size

        if (stat(srcPath, &st) == 0) { // make sure that the src file exists and can be accessed.
            if (S_ISDIR(st.st_mode)) { // => check if represent as directory
                copyDirectory(srcPath, destPath);
            } else if (S_ISREG(st.st_mode)) {// => check if represent as regular file
                copyFile(srcPath, destPath);
            }
        }
    }
    closedir(dir);// close directory when done
}

// ----------------------------
// handle cp command - copy
// ----------------------------

void cp(char **args) {
    // must include source and destination provided
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "cp: source and destination required\n");
        return;
    }

    struct stat st; // => system structure that stores information about a file (e.g., size, permissions, type, timestamps).
    if (stat(args[1], &st) != 0) { // make sure that file exists and can be accessed.
        perror("cp: source error");
        return;
    }

    if (S_ISDIR(st.st_mode)) {// => check if represent as directory
        copyDirectory(args[1], args[2]);
        printf("Directory copied successfully\n");
        return;
    } else if (S_ISREG(st.st_mode)) {// => check if represent as regular file
        copyFile(args[1], args[2]);
        printf("File copied successfully\n");
        return;
    } else {
        fprintf(stderr, "cp: unsupported file type\n");
        return;
    }
}

// ----------------------------
// handle rm -r command when a directory - remove recursively
// ----------------------------

void recursiveDelete(char *path) {
    
    struct stat st;
    // Check if the file/directory exists
    if (stat(path, &st) != 0) {
        perror("rm: Error accessing path");
        return;
    }

    // If it's a regular file or symbol link, remove it
    if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
        if (unlink(path) != 0) {
            perror("rm: Error deleting file");
        }
        return;
    }

    // If it's a directory, open it
    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            perror("rm: Error opening directory");
            return;
        }

        struct dirent *entry;
        char fullPath[1024];
        
        while ((entry = readdir(dir)) != NULL) {// => Read entries in the directory
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue; // Skip "." and ".."
            }
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

            // Recursively delete files and subdirectories
            recursiveDelete(fullPath);
        }

        closedir(dir);// => make sure to close directory when done

        if (rmdir(path) != 0) {// => Remove the empty directory
            perror("rm: Error removing directory");
        }
    }
}

// ----------------------------
// handle rm command - remove
// ----------------------------

void delete(char **args){
    // if the command is only "rm /path" then check if directory -> if so then return error, if a regular file then delete it
    if(args[2] == NULL){
        struct stat st; // => system structure that stores information about a file (e.g., size, permissions, type, timestamps).
        if (stat(args[1], &st) != 0) { // make sure that file exists and can be accessed.
            perror("rm: source error");
            return;
        }
        if (S_ISDIR(st.st_mode)) {// => check if represent as directory
            printf("Eror - rm: cannot remove %s: is a directory\n",args[1]);
            return;
        } else if (S_ISREG(st.st_mode)) {// => check if represent as regular file
            if (unlink(args[1]) != 0){
                printf("Error - delete: %s: No such file or directory\n", args[1]);
                return;
            }
        } else {
            fprintf(stderr, "rm: cannot remove file or directory\n");
            return;
        }
    }
    else{
        if(strcmp(args[1],"-r") == 0){// if a directory and want to recursively delete => cals the relevant function
            recursiveDelete(args[2]);
        }

    }
}

// ----------------------------
// handle pipe - example: the command "ls -l | grep g"
// creates two child processes, one runs "ls -l" and sends its output into a pipe, the other reads from that pipe and runs grep g
// ----------------------------

void mypipe(char **argv1, char **argv2){
    
    int fd[2];
    if(pipe(fd) == -1){ // => make sure pipe creation success
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0){// => creates first child process
        if (fork() == 0){// => creates second child process
            close(STDOUT_FILENO);// prevent the standart output of the grandchild process "ls -l"
            dup2(fd[1], STDOUT_FILENO);// redirects that output to the child process "grep g" => the pipe's write end
            // close both pipes ends
            close(fd[0]);
            close(fd[1]);
            if(execvp(argv1[0], argv1) == -1){// executing the "ls -l" command, if failed returns an error and exits
                perror("execvp error");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);// prevent that process from running unpredictably
        }

        close(STDIN_FILENO);// prevent the child process "grep g" from getting the standart input exits
        dup2(fd[0], STDIN_FILENO);//redirects the input from the grandchild process "ls -l" => the pipe's read end
        // close both pipes ends
        close(fd[1]);
        close(fd[0]);
        if(execvp(argv2[0], argv2) == -1){// executing the "grep g" command, if failed returns an error and exits
            perror("execvp error");
            exit(EXIT_FAILURE);
        };
        exit(EXIT_SUCCESS);// prevent that process from running unpredictably
    }
    // make sure to close both pipes ends and wait for both child processes to finish
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);
}

// ----------------------------
// function that moves a file/directory from src to dest path,
// its copying the src content to the dest position and then delete the src
// ----------------------------

void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {// => make sure to have src and dest paths
        fprintf(stderr, "Usage: mv <source> <destination>\n");
        return;
    }
    if (rename(args[1], args[2]) != 0) {// => copy and delete content from src to dest position, if fails returns an error
        perror("mv");
    }
}

// ----------------------------
// append a text inside a txt file without erasing the previous content
// ----------------------------

void echoppend(char **args) {
    if (args[0] == NULL || args[1] == NULL) {
        fprintf(stderr, "Usage: echo <text> >> <file>\n");
        return;
    }
    int fd = open(args[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("echo append error");
        return;
    }
    write(fd, args[0], strlen(args[0]));
    write(fd, "\n", 1);
    close(fd);
}

// ----------------------------
// write a text inside a txt file and erasing the previous content
// ----------------------------

void echowrite(char **args) {
    if (args[0] == NULL || args[1] == NULL) {
        fprintf(stderr, "Usage: echo <text> > <file>\n");
        return;
    }
    int fd = open(args[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("echo append error");
        return;
    }
    write(fd, args[0], strlen(args[0]));
    write(fd, "\n", 1);
    close(fd);
}

// ----------------------------
// read the file and print his content to the console
// ----------------------------

void _read(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: read <file>\n");
        return;
    }
    FILE *file = fopen(args[1], "r"); // => open the file for reading the content
    if (!file) { // if couldn't open file for reading then return error
        perror("read error");
        return;
    }
    
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        // => reads a file character by character using fgetc(file) until it reaches the end of the file (EOF)
        putchar(ch);// => prints the character to the console
    }
    fclose(file);
}

// ----------------------------
// count the words/ lines of the file
// ----------------------------

void wordCount(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Usage: wc -l/-w <file>\n");
        return;
    }
    FILE *file = fopen(args[2], "r"); // => open the file to read only
    if (!file) {
        perror("read error");
        return;
    }
    int count = 0;
    char ch;
    if (strcmp(args[1], "-l") == 0) {
        // as long as we didnt get to the end of the file, count the lines
        while ((ch = fgetc(file)) != EOF) {
            if (ch == '\n') {
                count++;
            }
        }
        printf("Lines: %d\n", count);
    } else if (strcmp(args[1], "-w") == 0) {
        int inWord = 0;
        // as long as we didnt get to the end of the file, count the words
        while ((ch = fgetc(file)) != EOF) {
            // if char is a space/line break/tab => don't count
            if (ch == ' ' || ch == '\n' || ch == '\t') {
                inWord = 0;
            } else if(inWord == 0){
                inWord = 1;
                count++;
            }
        }
        printf("Words: %d\n", count);
    } else {
        fprintf(stderr, "Invalid option: %s\n", args[1]);
    }
    fclose(file);
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Main execution for the commands
// ---------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------
// function that get the input from the user and split it into arguments
// ---------------------------------------

char **splitInput(char *input) {
    int bufsize = 64;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *)); // define an initial size of the input
    char *start = input;
    bool inQuotes = false;// flag of checking if the arguments is one string to manage cases like a directory name with spaces

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (*input) {
        if (*input == '"') {
            inQuotes = !inQuotes;
            if (!inQuotes) {
                // if finished the quotes,then set that current char which holds the closing quotes, as \0 in order to declare that the string stoped
                *input = '\0';
                // move to the next token and set it as the current input index
                tokens[position++] = start;
                // move the pointer of start from input so it won't hold an unnecessary space on memory
                start = NULL;
            } else {
                // if started the quotes then catch the first char after the quotes
                start = input + 1;
            }
        } else if ((*input == ' ' || *input == '\t') && !inQuotes) {
            // if the start pointer not yet null and the current input char is a space or tab and also the quote flag is false which means we out of the quote
            // then do the same as we wrote above on lines 134-136-138
            if (start) {
                *input = '\0';
                tokens[position++] = start;
                start = NULL;
            }
        } else if (!start) {
            // if start pointer has been nulled then point him to be the input current char
            start = input;
        }

        input++;// move to the next char at input
        if (position >= bufsize) {// if position of that input is bigger then the bufsize decalre at beggining then resize the bufsize and the tokens as well
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "Reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (start) {
        // => ensure the last token is added to tokens[] before returning the array
        tokens[position++] = start;
    }

    // set the last token as NULL to declare of finish the arguments array
    tokens[position] = NULL;
    return tokens;// return the arguments array
}

// ---------------------------------------
// if arguments containing a pipe symbol then split the arguments array into two arguments arrays
// ---------------------------------------

void splitByPipe(char **tokens) {
    char **leftArgs = NULL;
    char **rightArgs = NULL;
    int i = 0, leftCount = 0, rightCount = 0;
    bool foundPipe = false;

    while (tokens[i]) {
        if (strcmp(tokens[i], "|") == 0) {
            foundPipe = true;
            i++;
            continue;
        }
        // as long as pipe didnt found between the arguments => add the tokens one by one to the leftArgs array, whenever found the pipe, add the rest tokens to the rightArgs array
        if (!foundPipe) {
            leftArgs = realloc(leftArgs, (leftCount + 1) * sizeof(char *));
            leftArgs[leftCount++] = tokens[i];
        } else {
            rightArgs = realloc(rightArgs, (rightCount + 1) * sizeof(char *));
            rightArgs[rightCount++] = tokens[i];
        }
        i++;
    }

    // Null-terminate both arrays
    leftArgs = realloc(leftArgs, (leftCount + 1) * sizeof(char *));
    leftArgs[leftCount] = NULL;
    rightArgs = realloc(rightArgs, (rightCount + 1) * sizeof(char *));
    rightArgs[rightCount] = NULL;

    mypipe(leftArgs,rightArgs); // => send them both to mypipe function
    // free the memory allocated after execute the command
    free(leftArgs);
    free(rightArgs);
}

// ---------------------------------------
// if arguments containing the >> or > symbols then rearrange the arguments array to the form: [<text>,<path>]
// ---------------------------------------

void echoSplit(char **args){

    char *newArgs[3]; // Space for rearranged args
    int i = 1, textLength = 0, pos = 1;
    bool reWrite = false; // => true:echowrite || false:echoppend
    char *echoToken; // => catches the token ">" or ">>" accordingly

    //checks if send to echowrite or echoppend and catches token
    while(args[pos]){
        if(strcmp(args[pos], ">") == 0){
            reWrite = true;
            echoToken = args[pos];
            break;
        }
        else if(strcmp(args[pos], ">>") == 0){
            echoToken = args[pos];
            break;
        }
        pos++;
    }

    // Find the position of ">>" or ">"
    while (args[i] && strcmp(args[i], echoToken) != 0) {
        textLength += strlen(args[i]) + 1; // +1 for spaces
        i++;
    }

    // missing of src filename path
    if (!args[i] || !args[i + 1]) {
        fprintf(stderr, "Invalid syntax: missing filename after %s\n",echoToken);
        return;
    }

    // Allocate memory for the merged text
    newArgs[0] = malloc(textLength);
    if (!newArgs[0]) {
        perror("Memory allocation failed");
        return;
    }
    
    // Merge the words into a single string
    newArgs[0][0] = '\0';  // Initialize empty string
    for (int j = 1; j < i; j++) {
        strcat(newArgs[0], args[j]);
        if (j < i - 1) strcat(newArgs[0], " "); // Add space between words
    }

    // Set the filename
    newArgs[1] = args[i + 1];

    // Ensure NULL termination for execvp compatibility
    newArgs[2] = NULL;

    if(reWrite){
        echowrite(newArgs);
    }
    else if(!reWrite){
        echoppend(newArgs);
    }
    free(args);
    free(newArgs[0]);
}


// ---------------------------------------
// function that manage the commands according to the arguments
// ---------------------------------------

bool executeCommand(char *input) {
    char **args = splitInput(input);
    
    int count = 0;
    // => check if any of the arguments of the splited input is "exit" if so then ignoring the rest arguments and exiting program
    while(args[count] != NULL){
        if(strcmp(args[count], "exit") == 0){
            logout();
            free(args);
            return false;
        }
        else if(strcmp(args[count], "|") == 0){
            splitByPipe(args);
            return true;
        }
        else if(strcmp(args[count], ">>") == 0 || strcmp(args[count], ">") == 0){
            echoSplit(args);
            return true;
        }
        count++;
    }
    // otherwise execute command
    if(args[0] == NULL){
        free(args);
        return true;
    } else if (strcmp(args[0], "cd") == 0) {
        cd(args);
    } else if (strcmp(args[0], "cp") == 0) {
        cp(args);
    } else if (strcmp(args[0], "rm") == 0) {
        delete(args);
    } else if (strcmp(args[0], "mv") == 0) {
        move(args);
    } else if (strcmp(args[0], "read") == 0) {
        _read(args);
    } else if (strcmp(args[0], "wc") == 0) {
        wordCount(args);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("Error executing command");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Error forking");
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    
    free(args);
    return true;
}