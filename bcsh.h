/*
 * bcsh.h - Header file for the bcsh shell implementation
 * Copyright (C) 2025 Bryan Candiliere
 */

#ifndef BCSH_H
#define BCSH_H

#define _GNU_SOURCE // Enables POSIX/GNU extensions like getline()

#include <limits.h> // For PATH_MAX

#ifndef PATH_MAX
#define PATH_MAX 4096 // Fallback definition if not defined
#endif

// Function prototypes
void trim(char *line);

#endif // BCSH_H
