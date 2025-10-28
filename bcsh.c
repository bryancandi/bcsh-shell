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
    // Allow kernel to clean up background processes automatically
    signal(SIGCHLD, SIG_IGN);

    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t nread;

    while (1)
    {
        print_prompt();

        nread = getline(&line, &buffer_size, stdin);
        if (nread == -1) // Error or EOF
        {
            if (feof(stdin))
            {
                break; // EOF detected
            }
            perror("bcsh: getline failed");
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
                perror("bcsh: realloc failed");
                exit(EXIT_FAILURE);
            }
            args[argc++] = token;
            token = strtok(NULL, " \t\n"); // Continue tokenizing
        }
        args[argc] = NULL; // Null-terminate the argument list

        // Background execution check (&)
        // Check for '&' before NULL terminator (argc - 1)
        int background = 0;
        if (argc > 0 && strcmp(args[argc - 1], "&") == 0)
        {
            // Set background flag and remove '&' from arguments then decrease argc
            background = 1;
            args[argc - 1] = NULL; // '&' is now NULL
            argc--;
        }

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
                    fprintf(stderr, "bcsh: cd: HOME environment variable not set\n");
                    if (args)
                    {
                        free(args);
                    }
                    continue;
                }
            }
            if (chdir(target_dir) != 0)
            {
                perror("bcsh: cd failed");
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
            perror("bcsh: execvp failed");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            if (background == 0)
            {
                // Foreground execution: wait for the child process to finish
                wait(NULL);
            }
            else
            {
                // Background execution: do not wait for the child process
                printf("bcsh: Background job [%s] pid [%d] started\n", args[0], pid);
            }
        }
        else
        {
            perror("bcsh: fork failed");
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

// Print the shell prompt
void print_prompt()
{
    char dir[PATH_MAX];
    char *cwd = getcwd(dir, sizeof(dir));
    if (!cwd)
    {
        cwd = "?";
    }
    printf("%s@bcsh:%s $ ", getenv("USER") ?: "user", cwd);
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
