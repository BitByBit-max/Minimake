#include "minimake.h"

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
        struct command *c = r->command;
        while (c)
        {
            struct command *remc = c;
            c = c->next;
            free(remc->name);
            free(remc);
        }
        struct rule *del = r;
        r = r->next;
        free(del->name);
        free(del);
    }
}

void free_var(struct var *v)
{
    while (v)
    {
        struct var *del = v;
        v = v->next;
        free(del->name);
        free(del->value);
        free(del);
    }
}

void free_makefile(struct makefile *m)
{
    if (m->rule)
    {
        free_rule(m->rule);
    }
    if (m->var)
    {
        free_var(m->var);
    }
    free(m);
}

void call_print(char *path, int *err)
{
    struct makefile *m = calloc(1, sizeof(struct makefile));
    if (!m)
    {
        *err = 2;
        return;
    }
    FILE *makefile = fopen(path, "r");
    if (!makefile)
    {
        *err = 2;
        fprintf(stderr, "minimake: %s: No such file or directory\n", path);
        free(m);
        return;
    }
    int pres = parsing(makefile, m, err);
    if (*err || pres == 2)
    {
        fclose(makefile);
        free_makefile(m);
        return;
    }
    pretty_print(m);
    fclose(makefile);
    if (m)
    {
        free_makefile(m);
    }
}

void call_build(char *path, struct rule *rule_arr, int *err)
{
    struct makefile *m = calloc(1, sizeof(struct makefile));
    if (!m)
    {
        *err = 2;
        return;
    }
    FILE *makefile = fopen(path, "r");
    if (!makefile)
    {
        *err = 2;
        fprintf(stderr, "minimake: %s: No such file or directory\n", path);
        free(m);
        return;
    }
    int pres = parsing(makefile, m, err);
    if (*err || pres == 2)
    {
        fclose(makefile);
        free_makefile(m);
        return;
    }
    int build = building(m, rule_arr, err);
    if (*err || build == 2)
    {
        fclose(makefile);
        free_makefile(m);
        return;
    }
    fclose(makefile);
    if (m)
    {
        free_makefile(m);
    }
}

static char *makefile_selec(struct rule *rule_arr, int *err)
{
    char *path = "";
    if (access("makefile", F_OK) == 0)
    {
        path = "makefile";
    }
    else if (access("Makefile", F_OK) == 0)
    {
        path = "Makefile";
    }
    else
    {
        if (rule_arr)
        {
            fprintf(stderr,
                    "minimake: *** No rule to make target 'target'.  Stop.\n");
        }
        else
        {
            fprintf(stderr,
                    "minimake: *** No targets specified and no makefile found. "
                    " Stop.\n");
        }
        free_rule(rule_arr);
        *err = 2;
    }
    return path;
}

struct rule *push_rule(char *s, struct rule *rule_arr)
{
    char *name = strdup(s);
    struct rule *newr = init_rule(name);
    if (rule_arr)
    {
        struct rule *t = rule_arr;
        while (t->next)
        {
            t = t->next;
        }
        t->next = newr;
    }
    else
    {
        rule_arr = newr;
    }
    return rule_arr;
}

int main(int argc, char *argv[])
{
    int err = 0;
    char *path = NULL;
    struct rule *rule_arr = NULL;
    int print = 0;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h"))
        {
            printf("This is a help message.\n");
            free_rule(rule_arr);
            return 0;
        }
        else if (!strcmp(argv[i], "-p"))
        {
            print = 1;
        }
        else if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 >= argc)
            {
                free_rule(rule_arr);
                return 2;
            }
            path = argv[i + 1];
            i++;
        }
        else
        {
            if (*argv[i] == '\0')
            {
                fprintf(stderr,
                        "minimake: *** empty string invalid as file name.  "
                        "Stop.\n");
                free_rule(rule_arr);
                return 2;
            }
            rule_arr = push_rule(argv[i], rule_arr);
        }
    }
    if (!path)
    {
        path = makefile_selec(rule_arr, &err);
        if (err)
        {
            return 2;
        }
    }
    if (print)
    {
        free_rule(rule_arr);
        call_print(path, &err);
        if (err)
        {
            return 2;
        }
        return 0;
    }
    call_build(path, rule_arr, &err);
    free_rule(rule_arr);
    if (err)
    {
        return 2;
    }
    return 0;
}
