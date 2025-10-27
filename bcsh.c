#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024

int main(void)
{
    char line[MAX_LENGTH];

    while (1) {
        printf("$ ");
        if (!fgets(line, MAX_LENGTH, stdin))
        {
            break;
        }

        if (strcmp(line, "exit\n") == 0)
        {
            break;
        }
        else
        {
            // TODO: replace this with actual command execution logic
            system(line);
        }
    }

    return 0;
}
