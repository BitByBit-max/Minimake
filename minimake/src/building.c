#include "minimake.h"

static void com_exe(struct command *command, struct var *v, int *err)
{
    struct command *c = command;
    while (c)
    {
        char *com_name = c->name;
        int boool = *c->name == '@';
        if (boool)
        {
            com_name = c->name + 1;
        }
        com_name = var_sub(com_name, v, err);
        if (*err)
        {
            free(com_name);
            return;
        }
        if (!boool)
        {
            printf("%s\n", com_name);
        }
        int pid = fork();
        if (pid == -1)
        {
            *err = 2;
            return;
            // err(1, "error in the fork");
        }
        if (pid == 0)
        {
            execl("/bin/sh", "minimake", "-c", com_name, NULL);
        }
        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0);
        if (WEXITSTATUS(wstatus) != 0)
        {
            *err = 2;
            fprintf(stderr, "not found\n");
            return;
        }
        if (child_pid == -1)
        {
            *err = 2;
            return;
            // err(1, "wait child failed");
        }
        free(com_name);
        c = c->next;
    }
}

int building(struct makefile *m, struct rule *rule_arr, int *err)
{
    if (!m->rule)
    {
        *err = 2;
        fprintf(stderr, "minimake: *** No targets.  Stop.\n");
        return 2;
    }
    if (!rule_arr)
    {
        com_exe(m->rule->command, m->var, err);
        if (*err)
        {
            return 2;
        }
        return 0;
    }
    struct rule *r = rule_arr;
    while (r)
    {
        struct rule *mr = m->rule;
        while (mr)
        {
            if (!strcmp(mr->name, r->name))
            {
                com_exe(mr->command, m->var, err);
                if (*err)
                {
                    return 2;
                }
                break;
            }
            mr = mr->next;
        }
        if (!mr)
        {
            *err = 2;
            fprintf(stderr,
                    "minimake: *** No rule to make target '%s'.  Stop.\n",
                    r->name);
            return 2;
        }
        r = r->next;
    }
    return 0;
}
