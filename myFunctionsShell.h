/* myFunctionsShell.h */
#include <stdbool.h>

char *inputFromUser();
void loadingBar(int step, int totalSteps, char *header);
void printPixelArt();
void welcome();
void logout();
void getLocation();
void cd(char **args);
bool executeCommand(char *input);
