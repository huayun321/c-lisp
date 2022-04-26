#include <stdio.h> //系统文件
#include <stdlib.h>
#include "mpc.h" //其他库文件

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}
void add_history(char* unused) {}
#else
#ifdef __linux__
#include <editline/readline.h>
#include <editline/history.h>
#endif
#endif

#ifdef __MACH__
#include <editline/readline.h>
#endif

int main(int argc, char** argv) {
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Yun      = mpc_new("yun");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                       \
        number   : /-?[0-9]+/ ;                             \
        operator : '+' | '-' | '*' | '/' ;                  \
        expr     : <number> | '(' <operator> <expr>+ ')' ;  \
        yun      : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Yun);

    puts("Yun Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1)
    {
        char* input = readline("Yun> ");
        add_history(input);
        
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Yun, &r)) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
    
    mpc_cleanup(4, Number, Operator, Expr, Yun);

    return 0;
}

// cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing