#ifndef MINIMAKE_H
#define MINIMAKE_H

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct dep
{
    char *name;
    struct dep *next;
};

struct command
{
    char *name;
    struct command *next;
};

struct rule
{
    char *name;
    struct rule *next;
    struct dep *dep;
    struct command *command;
};

struct var
{
    char *name;
    char *value;
    struct var *next;
};

struct makefile
{
    // should it have a name ? there should only be one right ?
    struct var *var;
    struct rule *rule;
};

char *var_sub(char *w, struct var *var, int *err);
int parsing(FILE *makefile, struct makefile *m, int *err);
void pretty_print(struct makefile *m);
struct rule *init_rule(char *word);
int building(struct makefile *m, struct rule *rule_arr, int *err);

#endif /* !MINIMAKE_H */
