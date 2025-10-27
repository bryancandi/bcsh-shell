#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LENGTH 1024
#define MAX_ARGS_LENGTH 1024
#define WORKING_DIR_LENGTH 1024

void trim(char *line);

int main(void)
{
    char line[MAX_LENGTH];
    char dir[WORKING_DIR_LENGTH];

    while (1) {
        printf("bcsh:%s $ ", getcwd(dir, sizeof(dir)));
        if (!fgets(line, MAX_LENGTH, stdin))
        {
            break;
        }

        trim(line);

        if (strlen(line) == 0)
        {
            continue; // Skip empty lines
        }

        // Tokenize the input line into arguments
        char *args[MAX_ARGS_LENGTH];
        char *token = strtok(line, " \t\n"); // Tokenize input line by spaces and tabs
        int i = 0;
        while (token != NULL && i < MAX_ARGS_LENGTH - 1) {
            args[i++] = token; // Store each token in args array
            token = strtok(NULL, " \t\n"); // Continue tokenizing
        }
        args[i] = NULL; // Null-terminate the argument list

        if (args[0] == NULL)
        {
            continue; // No tokens/args found
        }

        // Built-in shell exit command
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }
       
        // Built in shell cd command
        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                fprintf(stderr, "cd: expected argument\n");
            }
            else if (chdir(args[1]) != 0)
            {
                perror("cd failed");
            }
            continue;
        }

        // Fork a child process to execute the command
        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            wait(NULL); // Parent process waits for child to finish
        }
        else
        {
            perror("fork failed");
        }
    }

    return 0;
}

// Function to trim leading and trailing whitespace from a string
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
