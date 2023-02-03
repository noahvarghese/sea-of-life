#include "utils/string_utils.h"

const char *str_concat(const char *str1, const char *str2)
{
    size_t string_length = strlen(str1) + strlen(str2) + 1;
    char *new_str = calloc(string_length, sizeof(char));

    if (new_str == NULL)
    {
        fprintf(stderr, "[ ERROR ]: failed to allocate %zu bytes.\n", string_length);
        exit(1);
    }

    for (int i = 0; i < strlen(str1); i++)
    {
        new_str[i] = str1[i];
    }

    for (int i = 0; i < strlen(str2); i++)
    {
        new_str[strlen(str1) + i] = str2[i];
    }

    return new_str;
}