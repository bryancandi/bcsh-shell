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
    char **args = NULL;
    int background = 0;

    while (1)
    {
        print_prompt();

        line = read_line();
        if (line == NULL)
        {
            continue;
        }

        args = tokenize_command(line, &background);
        if (args == NULL)
        { 
            free(line);
            continue;
        }

        int builtin_status = handle_builtin_commands(args);
        if (builtin_status == -1)
        {
            // Exit command
            free(args);
            free(line);
            break;
        }
        else if (builtin_status == 0)
        {
            // Built-in command executed
            free(args);
            free(line);
            continue;
        }

        execute_command(args, background);

        // Clean up allocated memory for next iteration
        if (args)
        {
            free(args);
        }
        if (line)
        {
            free(line);
        }
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

// Read user input line and handle comments and empty lines
char *read_line()
{
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t nread;

    nread = getline(&line, &buffer_size, stdin);
    if (nread == -1) // Error or EOF
    {
        if (feof(stdin))
        {
            free(line);
            return NULL; // EOF detected
        }
        perror("bcsh: getline failed");
        free(line);
        return NULL;
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
        free(line);
        return NULL;
    }

    return line;
}

// Tokenize the command line into arguments
char **tokenize_command(char *line, int *background)
{
    char **args = NULL;
    int argc = 0;
    char *token = strtok(line, " \t\n"); // Tokenize input line by spaces and tabs

    *background = 0; // Initialize background flag using pointer

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

    if (argc == 0)
    {
        free(args);
        return NULL; // No tokens found
    }

    args[argc] = NULL; // Null-terminate the argument list

    // Background execution check (&)
    // Check for '&' before NULL terminator (argc - 1)
    if (argc > 0 && strcmp(args[argc - 1], "&") == 0)
    {
        // Set background flag and remove '&' from arguments then decrease argc
        *background = 1; // Set background execution flag using pointer
        args[argc - 1] = NULL; // '&' is now NULL
    }

    return args;
}

// Handle built-in commands like 'cd' and 'exit'
int handle_builtin_commands(char **args)
{
    if (strcmp(args[0], "exit") == 0)
    {
        return -1; // Signal to exit shell
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        char *target_dir = args[1];
        if (target_dir == NULL)
        {
            target_dir = getenv("HOME");
            if (target_dir == NULL)
            {
                fprintf(stderr, "bcsh: cd: HOME environment variable not set\n");
                return 0;
            }
        }
        if (chdir(target_dir) != 0)
        {
            perror("bcsh: cd failed");
        }
        return 0;
    }

    return 1; // Not a built-in command
}

// Execute commands
void execute_command(char **args, int background)
{
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
