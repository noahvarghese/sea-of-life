#include "utils/std_utils.h"

void error(const char *message)
{
    const char *prefix = "[ ERROR ]: ";

    char *error_message;
    int length;
    length = strlen(prefix) + strlen(message) + 2; // account for newline and termination

    error_message = (char *)calloc(length, sizeof(char));
    if (error_message == NULL) {
        perror("[ ERROR ]: Unable to allocate memory for error message:");
        perror(message);
        exit(1);
    }

    sprintf(error_message, "[ ERROR ]: %s\n", message);
    perror(error_message);
    exit(1);
}