#include "minicc.h"

typedef struct Macro Macro;
struct Macro {
    Macro *next;
    char *name;
    bool is_objlike;  // true for #define FOO 1, false for #define FOO(a,b) ...
    char **params;
    int num_params;
    char *body;
};

static Macro *macros;

static void add_macro(char *name, bool is_objlike, char **params, int num_params, char *body) {
    Macro *m = calloc(1, sizeof(Macro));
    m->name = name;
    m->is_objlike = is_objlike;
    m->params = params;
    m->num_params = num_params;
    m->body = body;
    m->next = macros;
    macros = m;
}

static Macro *find_macro(char *name) {
    for (Macro *m = macros; m; m = m->next)
        if (!strcmp(m->name, name))
            return m;
    return NULL;
}

static char *read_file_content(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

static char *get_builtin_header(char *name) {
    if (!strcmp(name, "stdio.h")) {
        return "typedef struct FILE FILE;\n"
               "extern FILE *stdin, *stdout, *stderr;\n"
               "int printf(const char *fmt, ...);\n"
               "int sprintf(char *str, const char *fmt, ...);\n"
               "int fprintf(FILE *stream, const char *fmt, ...);\n"
               "int puts(const char *s);\n"
               "int putchar(int c);\n";
    }
    if (!strcmp(name, "stdlib.h")) {
        return "void *malloc(unsigned long size);\n"
               "void *calloc(unsigned long nmemb, unsigned long size);\n"
               "void *realloc(void *ptr, unsigned long size);\n"
               "void free(void *ptr);\n"
               "void exit(int status);\n"
               "int atoi(const char *nptr);\n";
    }
    if (!strcmp(name, "stdbool.h")) {
        return "#define bool _Bool\n"
               "#define true 1\n"
               "#define false 0\n"
               "#define __bool_true_false_are_defined 1\n";
    }
    if (!strcmp(name, "stdarg.h")) {
        return "typedef void *va_list;\n"
               "#define va_start(ap, last) ((ap) = (void*)&(last) + 8)\n"
               "#define va_arg(ap, type) (*(type*)((ap) += 8, (ap) - 8))\n"
               "#define va_end(ap) ((void)0)\n";
    }
    return NULL;
}

typedef struct CondStack CondStack;
struct CondStack {
    CondStack *next;
    bool active;    // Whether this branch is active
    bool was_true;  // Whether any branch in this #if block was true
};

static CondStack *cond_stack;

static void push_cond(bool cond) {
    CondStack *cs = calloc(1, sizeof(CondStack));
    bool parent_active = (!cond_stack || cond_stack->active);
    cs->active = parent_active && cond;
    cs->was_true = cond;
    cs->next = cond_stack;
    cond_stack = cs;
}

static void handle_else(void) {
    if (!cond_stack) error("stray #else");
    bool parent_active = (!cond_stack->next || cond_stack->next->active);
    cond_stack->active = parent_active && !cond_stack->was_true;
}

static void handle_endif(void) {
    if (!cond_stack) error("stray #endif");
    cond_stack = cond_stack->next;
}

static bool is_cond_active(void) {
    return !cond_stack || cond_stack->active;
}

// Substitute parameters in macro body with actual arguments
static char *substitute_func_macro(Macro *m, char **args) {
    size_t cap = strlen(m->body) * 2 + 256;
    char *out = malloc(cap);
    out[0] = '\0';
    size_t out_len = 0;

    char *p = m->body;
    while (*p) {
        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_') {
            char *start = p;
            while (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') ||
                   ('0' <= *p && *p <= '9') || *p == '_')
                p++;
            char *ident = strndup(start, p - start);

            int param_idx = -1;
            for (int i = 0; i < m->num_params; i++) {
                if (!strcmp(m->params[i], ident)) {
                    param_idx = i;
                    break;
                }
            }

            if (param_idx != -1) {
                char *arg = args[param_idx];
                size_t arg_len = strlen(arg);
                if (out_len + arg_len + 1 >= cap) {
                    cap = (out_len + arg_len + 1) * 2;
                    out = realloc(out, cap);
                }
                strcpy(out + out_len, arg);
                out_len += arg_len;
            } else {
                size_t id_len = strlen(ident);
                if (out_len + id_len + 1 >= cap) {
                    cap = (out_len + id_len + 1) * 2;
                    out = realloc(out, cap);
                }
                strcpy(out + out_len, ident);
                out_len += id_len;
            }
            free(ident);
        } else {
            if (out_len + 2 >= cap) {
                cap = (out_len + 2) * 2;
                out = realloc(out, cap);
            }
            out[out_len++] = *p++;
            out[out_len] = '\0';
        }
    }
    return out;
}

char *preprocess(char *input) {
    size_t out_cap = strlen(input) * 2 + 1024;
    size_t out_len = 0;
    char *out = malloc(out_cap);
    out[0] = '\0';

    char *p = input;
    while (*p) {
        // Line processing
        char *line_start = p;
        while (*p && *p != '\n') p++;
        size_t line_len = p - line_start;
        if (*p == '\n') p++;

        char *line = strndup(line_start, line_len);

        // Trim leading space for directive check
        char *start = line;
        while (*start == ' ' || *start == '\t') start++;

        if (*start == '#') {
            start++; // skip '#'
            while (*start == ' ' || *start == '\t') start++;

            if (!strncmp(start, "ifdef", 5) && (isspace(start[5]) || !start[5])) {
                start += 5;
                while (*start == ' ' || *start == '\t') start++;
                char *mname = start;
                while (*start && !isspace(*start)) start++;
                *start = '\0';
                push_cond(find_macro(mname) != NULL);
                free(line);
                continue;
            }

            if (!strncmp(start, "ifndef", 6) && (isspace(start[6]) || !start[6])) {
                start += 6;
                while (*start == ' ' || *start == '\t') start++;
                char *mname = start;
                while (*start && !isspace(*start)) start++;
                *start = '\0';
                push_cond(find_macro(mname) == NULL);
                free(line);
                continue;
            }

            if (!strncmp(start, "else", 4)) {
                handle_else();
                free(line);
                continue;
            }

            if (!strncmp(start, "endif", 5)) {
                handle_endif();
                free(line);
                continue;
            }

            if (!is_cond_active()) {
                free(line);
                continue;
            }

            if (!strncmp(start, "define", 6) && (isspace(start[6]) || !start[6])) {
                start += 6;
                while (*start == ' ' || *start == '\t') start++;
                char *mname = start;
                while (*start && (('a' <= *start && *start <= 'z') || ('A' <= *start && *start <= 'Z') ||
                                  ('0' <= *start && *start <= '9') || *start == '_'))
                    start++;

                char *name_str = strndup(mname, start - mname);

                bool is_objlike = true;
                char **params = NULL;
                int num_params = 0;

                if (*start == '(') {
                    is_objlike = false;
                    start++; // skip '('
                    char *p_start = start;
                    int p_cap = 4;
                    params = malloc(p_cap * sizeof(char*));

                    while (*start && *start != ')') {
                        while (*start == ' ' || *start == '\t') start++;
                        char *param_id_start = start;
                        while (*start && (('a' <= *start && *start <= 'z') || ('A' <= *start && *start <= 'Z') ||
                                          ('0' <= *start && *start <= '9') || *start == '_'))
                            start++;
                        if (start > param_id_start) {
                            if (num_params >= p_cap) {
                                p_cap *= 2;
                                params = realloc(params, p_cap * sizeof(char*));
                            }
                            params[num_params++] = strndup(param_id_start, start - param_id_start);
                        }
                        while (*start == ' ' || *start == '\t') start++;
                        if (*start == ',') start++;
                    }
                    if (*start == ')') start++;
                }

                while (*start == ' ' || *start == '\t') start++;
                char *mbody = strdup(start);
                add_macro(name_str, is_objlike, params, num_params, mbody);
                free(line);
                continue;
            }

            if (!strncmp(start, "include", 7) && (isspace(start[7]) || !start[7])) {
                start += 7;
                while (*start == ' ' || *start == '\t') start++;
                char quote = *start;
                if (quote == '"' || quote == '<') {
                    char end_quote = (quote == '"') ? '"' : '>';
                    char *hname = start + 1;
                    char *end_h = strchr(hname, end_quote);
                    if (end_h) {
                        *end_h = '\0';
                        char *content = NULL;
                        if (quote == '"') content = read_file_content(hname);
                        if (!content) content = get_builtin_header(hname);

                        if (content) {
                            char *sub_out = preprocess(content);
                            size_t sub_len = strlen(sub_out);
                            if (out_len + sub_len + 2 >= out_cap) {
                                out_cap = (out_len + sub_len + 2) * 2;
                                out = realloc(out, out_cap);
                            }
                            strcpy(out + out_len, sub_out);
                            out_len += sub_len;
                            out[out_len++] = '\n';
                            out[out_len] = '\0';
                            free(sub_out);
                        }
                    }
                }
                free(line);
                continue;
            }
        }

        if (!is_cond_active()) {
            free(line);
            continue;
        }

        // Macro expansion in line
        char *expanded_line = malloc(strlen(line) * 4 + 1024);
        expanded_line[0] = '\0';
        size_t exp_len = 0;

        char *lp = line;
        while (*lp) {
            if (('a' <= *lp && *lp <= 'z') || ('A' <= *lp && *lp <= 'Z') || *lp == '_') {
                char *ident_start = lp;
                while (('a' <= *lp && *lp <= 'z') || ('A' <= *lp && *lp <= 'Z') ||
                       ('0' <= *lp && *lp <= '9') || *lp == '_')
                    lp++;
                char *ident = strndup(ident_start, lp - ident_start);
                Macro *m = find_macro(ident);

                if (m) {
                    if (m->is_objlike) {
                        strcpy(expanded_line + exp_len, m->body);
                        exp_len += strlen(m->body);
                    } else {
                        // Function-like macro expansion: NAME(arg1, arg2)
                        while (*lp == ' ' || *lp == '\t') lp++;
                        if (*lp == '(') {
                            lp++; // skip '('
                            char **args = malloc((m->num_params + 1) * sizeof(char*));
                            int arg_cnt = 0;
                            int depth = 1;
                            char *arg_start = lp;

                            while (*lp && depth > 0) {
                                if (*lp == '(') depth++;
                                else if (*lp == ')') {
                                    depth--;
                                    if (depth == 0) break;
                                } else if (*lp == ',' && depth == 1) {
                                    args[arg_cnt++] = strndup(arg_start, lp - arg_start);
                                    lp++;
                                    arg_start = lp;
                                    continue;
                                }
                                lp++;
                            }
                            if (depth == 0) {
                                args[arg_cnt++] = strndup(arg_start, lp - arg_start);
                                if (*lp == ')') lp++;
                            }

                            char *subst = substitute_func_macro(m, args);
                            strcpy(expanded_line + exp_len, subst);
                            exp_len += strlen(subst);
                            free(subst);
                            for (int i = 0; i < arg_cnt; i++) free(args[i]);
                            free(args);
                        } else {
                            strcpy(expanded_line + exp_len, ident);
                            exp_len += strlen(ident);
                        }
                    }
                } else {
                    strcpy(expanded_line + exp_len, ident);
                    exp_len += strlen(ident);
                }
                free(ident);
            } else {
                expanded_line[exp_len++] = *lp++;
                expanded_line[exp_len] = '\0';
            }
        }

        if (out_len + exp_len + 2 >= out_cap) {
            out_cap = (out_len + exp_len + 2) * 2;
            out = realloc(out, out_cap);
        }
        strcpy(out + out_len, expanded_line);
        out_len += exp_len;
        out[out_len++] = '\n';
        out[out_len] = '\0';

        free(expanded_line);
        free(line);
    }

    return out;
}
