#ifndef MICROMAKE_H
#define MICROMAKE_H

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dep
{
    char *name;
    struct dep *next;
};

struct rule
{
    char *name;
    struct rule *next;
    struct dep *dep;
};

#endif /* !MICROMAKE_H */
