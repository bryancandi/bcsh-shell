/*
 * bcsh.c - A simple Unix-like command-line shell implementation in C
 * Copyright (C) 2025 Bryan Candiliere
 */

#include "bcsh.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    char dir[PATH_MAX];
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t nread;

    while (1)
    {
        char *cwd = getcwd(dir, sizeof(dir));
        if (!cwd)
        {
            cwd = "?";
        }
        printf("bcsh:%s $ ", cwd);
        nread = getline(&line, &buffer_size, stdin);
        if (nread == -1) // Error or EOF
        {
            if (feof(stdin))
            {
                break; // EOF detected
            }
            perror("getline failed");
            break;
        }

        // Handle comments
        char *comment_start = strchr(line, '#');
        if (comment_start)
        {
            *comment_start = '\0'; // Truncate line at comment
        }

        trim(line);

        // Skip empty lines
        if (strlen(line) == 0)
        {
            continue;
        }

        // Tokenize the input line into arguments
        char **args = NULL;
        int argc = 0;
        char *token = strtok(line, " \t\n"); // Tokenize input line by spaces and tabs
        while (token != NULL)
        {
            args = realloc(args, sizeof(char*) * (argc + 2)); // +1 for token, +1 for NULL terminator
            if (!args)
            {
                perror("realloc failed");
                exit(EXIT_FAILURE);
            }
            args[argc++] = token;
            token = strtok(NULL, " \t\n"); // Continue tokenizing
        }
        args[argc] = NULL; // Null-terminate the argument list

        // Built-in shell exit command
        if (strcmp(args[0], "exit") == 0)
        {
            if (args)
            {
                free(args);
            }
            break;
        }

        // Built-in shell cd command
        if (strcmp(args[0], "cd") == 0)
        {
            char *target_dir = args[1];
            if (target_dir == NULL)
            {
                target_dir = getenv("HOME");
                if (target_dir == NULL)
                {
                    fprintf(stderr, "cd: HOME environment variable not set\n");
                    if (args)
                    {
                        free(args);
                    }
                    continue;
                }
            }
            if (chdir(target_dir) != 0)
            {
                perror("cd failed");
            }
            if (args)
            {
                free(args);
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
            // Wait for the child process to finish
            wait(NULL);
        }
        else
        {
            perror("fork failed");
        }
        if (args)
        {
            free(args);
        }
    }

    if (line) // Final cleanup
    {
        free(line);
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
    while (start_index + i <= end_index)
    {
        line[i] = line[start_index + i];
        i++;
    }

    line[i] = '\0';
}
