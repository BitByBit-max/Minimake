#define _POSIX_C_SOURCE 200809L

#include "micromake.h"

typedef int (*classifier_f)(int);

void free_rule(struct rule *r)
{
    while (r)
    {
        struct dep *d = r->dep;
        while (d)
        {
            struct dep *rem = d;
            d = d->next;
            free(rem->name);
            free(rem);
        }
        struct rule *del = r;
        r = r->next;
        free(del->name);
        free(del);
    }
}

void rule_print(struct rule *r)
{
    while (r)
    {
        printf("(%s):", r->name);
        struct dep *d = r->dep;
        while (d)
        {
            printf(" [%s]", d->name);
            d = d->next;
        }
        printf("\n");
        r = r->next;
    }
}

static void skip_class(classifier_f classifier, char **cursor)
{
    while ((*classifier)(**cursor))
    {
        (*cursor)++;
    }
}

int iswordchar(int c)
{
    if ((c != '\0') && !isspace(c) && (c != ':') && (c != '='))
    {
        return 1;
    }
    return 0;
}

int isforbidden(char *word, char *forbidden)
{
    for (int i = 0; word[i] != '\0'; i++)
    {
        for (int j = 0; forbidden[j] != '\0'; j++)
        {
            if (word[i] == forbidden[j])
            {
                return 1;
            }
        }
    }
    return 0;
}

char *parseword(char **l)
{
    skip_class(isspace, l);
    const char *word_1_start = *l;
    skip_class(iswordchar, l);
    const char *word_1_end = *l;
    int word_1_length = word_1_end - word_1_start;
    skip_class(isspace, l);
    char *word = malloc(sizeof(char) * word_1_length + 1);
    for (int i = 0; i < word_1_length; i++)
    {
        word[i] = *word_1_start;
        word_1_start++;
    }
    word[word_1_length] = '\0';
    return word;
}

struct rule *init_rule(char *word)
{
    struct rule *newr = malloc(sizeof(struct rule) * 1);
    newr->name = word;
    newr->dep = NULL;
    newr->next = NULL;
    return newr;
}

struct rule *parseline(char *l, struct rule *rule, int *err)
{
    struct rule *r = rule;
    if (*l == '\t')
    {
        *err = 2;
        return r;
    }
    char *word = parseword(&l);
    if (*word == '\0')
    {
        free(word);
        return r;
    }
    if (isforbidden(word, " =") || *l != ':')
    {
        *err = 2;
        free(word);
        return r;
    }
    struct rule *newr = init_rule(word);
    if (r)
    {
        struct rule *t = r;
        while (t->next)
        {
            t = t->next;
        }
        t->next = newr;
    }
    else
    {
        r = newr;
    }
    l++;
    skip_class(isspace, &l);
    if (isforbidden(word, "=:"))
    {
        *err = 2;
        return r;
    }
    while (iswordchar(*l))
    {
        word = parseword(&l);
        struct dep *newd = malloc(sizeof(struct dep) * 1);
        newd->name = word;
        newd->next = NULL;
        if (newr->dep)
        {
            struct dep *tmp = newr->dep;
            while (tmp->next)
            {
                tmp = tmp->next;
            }
            tmp->next = newd;
        }
        else
        {
            newr->dep = newd;
        }
    }
    return r;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 2;
    }
    FILE *makefile = fopen(argv[1], "r");
    ;
    if (!makefile)
    {
        return 2;
    }
    char *line = NULL;
    struct rule *r = NULL;
    size_t i = 0;
    ssize_t l = getline(&line, &i, makefile);
    while (l != -1)
    {
        int err = 0;
        r = parseline(line, r, &err);
        if (err)
        {
            free(line);
            free_rule(r);
            fclose(makefile);
            return 2;
        }
        l = getline(&line, &i, makefile);
    }
    rule_print(r);
    fclose(makefile);
    if (line)
    {
        free(line);
    }
    free_rule(r);
    return 0;
}
