#include <stdlib.h>
#include <string.h>

#include "strutil.h"

char *
str_join(size_t n, char *strs[])
{
    size_t length = 0;
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            length++;
        }
        length += strlen(strs[i]);
    }
    char *str = malloc(length + 1);
    str[0] = '\0';
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            strcat(str, " ");
        }
        strcat(str, strs[i]);
    }
    return str;
}
