#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024

void trim(char *line);

int main(void)
{
    char line[MAX_LENGTH];

    while (1) {
        printf("$ ");
        if (!fgets(line, MAX_LENGTH, stdin))
        {
            break;
        }

        trim(line);

        if (strcmp(line, "exit") == 0)
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

void trim(char *line)
{
    int start_index = 0;
    int end_index = strlen(line) - 1;

    if (line == NULL)
    {
        return;
    }

    while (isspace(line[start_index]))
    {
        start_index++;
    }

    while (end_index >= start_index && isspace(line[end_index]))
    {
        end_index--;
    }

    int i = 0;
    while (start_index + i <= end_index) {
        line[i] = line[start_index + i];
        i++;
    }

    line[i] = '\0';
}
