#include "minimake.h"

typedef int (*classifier_f)(int);

static void skip_class(classifier_f classifier, char **cursor)
{
    while ((*classifier)(**cursor))
    {
        (*cursor)++;
    }
}

static size_t len(char *s)
{
    size_t count = 0;
    for (int i = 0; s[i] != '\0'; i++)
    {
        count++;
    }
    return count;
}

static int iswordchar(int c)
{
    if ((c != '\0') && !isspace(c) && (c != ':') && (c != '='))
    {
        return 1;
    }
    return 0;
}

static int noparenthesis(int c)
{
    return (c != ')' && c != '}' && c != '\0') ? 1 : 0;
}

static int isforbidden(char *word, char *forbidden)
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

static char *find_var_sub(char *s, struct var *var)
{
    struct var *v = var;
    char *res = "";
    while (v)
    {
        if (!strcmp(v->name, s))
        {
            res = v->value;
            break;
        }
        v = v->next;
    }
    return res;
}

static char *parseword(char **l)
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
    newr->command = NULL;
    return newr;
}

char *var_sub(char *w, struct var *var, int *err)
{
    char *word = w;
    size_t word_len = len(word);
    char *neww = malloc(sizeof(char) * (word_len + 1));
    int widx = 0;
    while (*word != '\0')
    {
        if (*word == '$' && ((word[1] == '(') || (word[1] == '{')))
        {
            char par = word[1];
            word += 2;
            char *startw = word;
            skip_class(noparenthesis, &word);
            if (*word == '\0' || (par == '(' && *word == '}')
                || (par == '{' && *word == ')'))
            {
                *err = 2;
                neww[0] = '\0';
                fprintf(stderr, "Unterminated variable\n");
                return neww;
            }
            char *rest = word + 1;
            *word = '\0';
            char *sub = find_var_sub(startw, var);
            word_len += len(sub) - len(startw) - 3;
            neww = realloc(neww, sizeof(char) * (word_len + 1));
            for (; *sub != '\0'; sub++, widx++)
            {
                neww[widx] = *sub;
            }
            word = rest;
        }
        else if (*word == '$')
        {
            word++;
            char startw[2] = { *word, 0 };
            char *sub = find_var_sub(startw, var);
            word_len += len(sub) - 2;
            neww = realloc(neww, sizeof(char) * (word_len + 1));
            for (; *sub != '\0'; sub++, widx++)
            {
                neww[widx] = *sub;
            }
        }
        else
        {
            neww[widx] = *word;
            word++;
            widx++;
        }
    }
    neww[widx] = '\0';
    return neww;
}

static void add_dep(char *word, struct rule *newr)
{
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

static struct rule *parserule(char *l, struct makefile *m, int *err,
                              struct rule **curr)
{
    struct var *var = m->var;
    struct rule *r = m->rule;
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
    char *neww = var_sub(word, var, err);
    free(word);
    if (*err)
    {
        return r;
    }
    struct rule *newr = init_rule(neww);
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
    *curr = newr;
    l++;
    skip_class(isspace, &l);
    if (isforbidden(neww, "=:"))
    {
        *err = 2;
        return r;
    }
    char *line = var_sub(l, var, err);
    l = line;
    while (iswordchar(*l))
    {
        word = parseword(&l);
        if (*err)
        {
            free(line);
            return r;
        }
        add_dep(word, newr);
    }
    free(line);
    return r;
}

static char *getvalue(char *l)
{
    skip_class(isspace, &l);
    if (*l == '=')
    {
        l++;
    }
    skip_class(isspace, &l);
    char *tmp = l;
    while (*tmp != '\0')
    {
        tmp++;
    }
    tmp--;
    while (isspace(*tmp))
    {
        tmp--;
    }
    int length = tmp - l;
    char *word = malloc(sizeof(char) * length + 2);
    for (int i = 0; i < length + 1; i++)
    {
        word[i] = *l;
        l++;
    }
    word[length + 1] = '\0';
    return word;
}

static struct var *parsevar(char *l, struct var *var, int *err)
{
    struct var *v = var;
    char *word = parseword(&l);
    if (*word == '\0')
    {
        free(word);
        return v;
    }
    if (isforbidden(word, " :") || *l != '=')
    {
        *err = 2;
        free(word);
        return v;
    }
    char *neww = var_sub(word, var, err);
    free(word);
    if (*err)
    {
        return v;
    }
    struct var *newv = malloc(sizeof(struct var) * 1);
    newv->name = neww;
    newv->value = getvalue(l);
    newv->next = NULL;
    if (v)
    {
        struct var *t = v;
        while (t->next)
        {
            t = t->next;
        }
        t->next = newv;
    }
    else
    {
        v = newv;
    }
    return v;
}

struct command *add_command(struct command *c, char *l)
{
    struct command *newc = malloc(sizeof(struct command) * 1);
    skip_class(isspace, &l);
    char *word = getvalue(l);
    newc->name = word;
    newc->next = NULL;
    if (!c)
    {
        return newc;
    }
    struct command *tmp = c;
    while (tmp->next)
    {
        tmp = tmp->next;
    }
    tmp->next = newc;
    return c;
}

static int isrule(char *l)
{
    for (size_t i = 0; l[i] != '\0'; i++)
    {
        if (l[i] == ':')
        {
            return 1;
        }
    }
    return 0;
}

static int isvar(char *l)
{
    for (size_t i = 0; l[i] != '\0'; i++)
    {
        if (l[i] == '=')
        {
            return 1;
        }
    }
    return 0;
}

int parsing(FILE *makefile, struct makefile *m, int *error)
{
    if (!makefile)
    {
        return 2;
    }
    char *line = NULL;
    struct rule *curr = NULL;
    struct command *c = NULL;
    size_t i = 0;
    ssize_t l = 0;
    int foundr = 0;
    while ((l = getline(&line, &i, makefile)) != -1)
    {
        int err = 0;
        if (*line != '\t' && c && *line != '\n')
        {
            curr->command = c;
            c = NULL;
        }
        if (*line == '\t' && foundr)
        {
            c = add_command(c, line);
        }
        else if (isrule(line))
        {
            m->rule = parserule(line, m, &err, &curr);
            foundr = 1;
        }
        else if (isvar(line))
        {
            foundr = 0;
            m->var = parsevar(line, m->var, &err);
        }
        if (err || (*line == '\t' && !foundr))
        {
            *error = 2;
            fprintf(stderr, "%s: command outside of rule\n", line + 1);
            free(line);
            return 2;
        }
    }
    if (c)
    {
        curr->command = c;
        c = NULL;
    }
    free(line);
    return 0;
}
