/*
 * bcsh.h - Header file for the bcsh shell implementation
 * Copyright (C) 2025 Bryan Candiliere
 */

#ifndef BCSH_H
#define BCSH_H

#define _GNU_SOURCE // Enables POSIX/GNU extensions like getline()
#define HOSTNAME_MAX 64

#include <limits.h> // For PATH_MAX

#ifndef PATH_MAX
#define PATH_MAX 4096 // Fallback definition if not defined
#endif

// Function prototypes
void print_prompt();
char *read_line();
char **tokenize_command(char *line, int *background);
int handle_builtin_commands(char **args);
void execute_command(char **args, int background);
void trim(char *line);

#endif // BCSH_H
