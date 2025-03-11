#include <stdbool.h>

/**

@brief Reads user input dynamically until a newline character is encountered.

This function dynamically allocates memory to store user input, expanding

as necessary. It terminates input collection when a newline character ('\n')

is encountered.


@warning The caller is responsible for freeing the allocated memory.


@return A dynamically allocated null-terminated string containing the user input.


@note If memory allocation fails, the function returns NULL.
*/
char *inputFromUser();



/**

@brief Displays a loading bar in the terminal.

This function prints a progress bar that visually represents the loading progress

based on the given step and total steps. It clears the terminal before updating

the display.


@param step The current step number in the loading process.

@param totalSteps The total number of steps to complete loading.

@param header A string header displayed above the loading bar.


@return void


@note The function uses ANSI escape sequences to clear the screen and change colors.

@note The function does not perform input validation; ensure valid values are provided.

*/
void loadingBar(int step, int totalSteps, char *header);



/**

@brief Prints ASCII pixel art to the terminal.

This function outputs a predefined set of ASCII characters to form pixel art.


@return void


@note The function uses ANSI escape codes to color the output.

@note The pixel art is predefined and cannot be customized dynamically.
*/
void printPixelArt();



/**

@brief Initializes and starts the interactive shell.

This function displays a loading bar, prints an ASCII art logo,

and starts an interactive shell session. It continuously reads

user input and executes commands until 'exit' is entered.


@return void
*/
void welcome();



/**

@brief Displays a logout loading animation.

This function visually represents the logout process using a loading bar.

It simulates the process with a delay before exiting.


@return void
*/
void logout();



/**

@brief Retrieves and displays the current user location.

This function fetches the current working directory, hostname,

and username, displaying them in a formatted shell prompt.


@return void

@note Uses ANSI escape codes for colored output.
*/
void getLocation();



/**

@brief Changes the current working directory.

This function attempts to change the working directory to the

path specified in the argument.


@param args A null-terminated array of strings where args[1] is the target directory.


@return void


@note If no argument is provided, an error message is displayed.

@note If multiple arguments are given, an error message is displayed.

@note If the directory change fails, an error message is printed.
*/
void cd(char **args);



/**

@brief Copies a file from source to destination.

This function reads the source file and writes its content to the destination.


@param src The source file path.

@param dest The destination file path.


@return void

@note If the source file does not exist or cannot be opened, an error is displayed.
*/
void copyFile(char *src,char *dest);



/**

@brief Recursively copies a directory and its contents.

This function creates a new directory at the destination and copies

all files and subdirectories from the source.


@param src The source directory path.

@param dest The destination directory path.


@return void


@note If the source directory does not exist, an error is displayed.
*/
void copyDirectory(char *src,char *dest);




/**

@brief Copies files or directories based on input arguments.

This function determines whether the source is a file or a directory

and calls the appropriate copy function.


@param args A null-terminated array of strings containing source and destination paths.


@return void

@note Displays an error if the source does not exist or if an unsupported file type is encountered.
*/
void cp(char **args);




/**

@brief Recursively deletes a file or directory.

This function removes all files and subdirectories before deleting the target directory.


@param path The file or directory path to delete.


@return void

@note If the specified path does not exist, an error is displayed.
*/
void recursiveDelete(char *path);




/**

@brief Deletes a file or directory based on input arguments.

This function determines whether the target is a file or directory

and performs the appropriate deletion.


@param args A null-terminated array of strings where args[1] is the target and args[2] is optional.


@return void

@note If '-r' is provided as args[1], recursive deletion is performed.
*/
void delete(char **args);




/**

@brief Executes two commands with a pipe between them.

This function creates a pipe and forks two child processes to execute

the given commands, passing the output of the first command as input

to the second.


@param argv1 A null-terminated array of strings representing the first command.

@param argv2 A null-terminated array of strings representing the second command.


@return void


@note If pipe creation or execution fails, an error message is displayed.
*/
void mypipe(char **argv1, char **argv2);




/**

@brief Moves or renames a file or directory.

This function renames a file or directory to a new location.


@param args A null-terminated array of strings where args[1] is the source and args[2] is the destination.


@return void


@note If renaming fails, an error message is displayed.
*/
void move(char **args);




/**

@brief Appends text to a file.

This function writes the given text to a file without overwriting existing content.


@param args A null-terminated array of strings where args[0] is the text and args[1] is the file name.


@return void


@note If the file cannot be opened, an error message is displayed.
*/
void echoppend(char **args);




/**

@brief Writes text to a file, overwriting existing content.

This function writes the given text to a file, replacing any previous content.


@param args A null-terminated array of strings where args[0] is the text and args[1] is the file name.


@return void


@note If the file cannot be opened, an error message is displayed.
*/
void echowrite(char **args);




/**

@brief Reads the content of a file and prints it to the standard output.

This function opens a file in read mode, reads its content character by character,

and prints it to the standard output. If the file cannot be opened, an error message is displayed.


@param args A NULL-terminated array of strings where:


   - args[1]: The file path to read.


@note If no filename is provided, an error message is printed.

@warning The function does not allocate memory dynamically, so no need to free resources.
*/
void _read(char **args);




/**

@brief Counts the number of lines or words in a file.

This function processes a given file and counts either the number of lines or words,

depending on the specified option (-l for lines, -w for words). It prints the result

to the standard output.


@param args A NULL-terminated array of strings where:


   - args[1]: The option (-l for line count, -w for word count).


   - args[2]: The file path to process.


@return Prints the count of lines or words to the standard output.

@note If an incorrect number of arguments is provided, an error message is displayed.

@warning The caller must ensure the file exists and is readable.
*/
void wordCount(char **args);




/**

@brief Splits an input string into tokens, considering quoted substrings as a single token.

This function tokenizes an input string based on spaces and tabs while keeping quoted

substrings intact. It dynamically allocates memory for an array of strings.


@param input A pointer to the string to be split.


@return A dynamically allocated NULL-terminated array of strings (tokens).


    Returns NULL if memory allocation fails.


@note The input string is modified during tokenization.

@warning The caller is responsible for freeing the allocated memory using free().
*/
char **splitInput(char *input);




/**

@brief Splits a command line input into two parts based on a pipe (|) and executes both commands.

This function searches for a pipe (|) in a tokenized command array, splits it into two

separate command arrays (before and after the pipe), and then executes both commands

using the mypipe function.


@param tokens A NULL-terminated array of strings representing parsed user input.


@note The function dynamically allocates memory for the command arrays.

@warning The caller is responsible for ensuring that memory is properly freed.
*/
void splitByPipe(char **args);




/**

@brief Handles redirection for the echo command.

This function processes the echo command, detecting redirection symbols ('>' or '>>'),

and redirects the output accordingly by calling the appropriate function.


@param args A NULL-terminated array of strings representing command arguments.


@note The function dynamically allocates memory for storing modified arguments.

@warning The caller is responsible for ensuring proper memory management.
*/
void echoSplit(char **args);




/**

@brief Parses and executes a user command.

This function takes a user input string, splits it into tokens, and determines

which command to execute. It handles built-in commands (e.g., cd, cp, rm, mv, read, wc),

as well as external commands using fork and exec.


@param input A pointer to the input string containing the command.


@return A boolean value:


    - false if the command is 'exit', indicating termination.


    - true otherwise, meaning execution should continue.


@note The function dynamically allocates memory for parsed tokens.

@warning The caller is responsible for ensuring proper memory deallocation.
*/
bool executeCommand(char *input);

