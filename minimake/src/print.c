#include "minimake.h"

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
        struct command *c = r->command;
        while (c)
        {
            printf("\t\'%s\'\n", c->name);
            c = c->next;
        }
        r = r->next;
    }
}

void var_print(struct var *v)
{
    while (v)
    {
        printf("\'%s\' = \'%s\'\n", v->name, v->value);
        v = v->next;
    }
}

void pretty_print(struct makefile *m)
{
    if (!m)
    {
        return;
    }
    printf("# variables\n");
    var_print(m->var);
    // might need printf("\n");
    printf("# rules\n");
    rule_print(m->rule);
}
