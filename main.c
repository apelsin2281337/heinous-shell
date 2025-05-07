#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT_SIZE 256

void calc(double a, double b, char op) {
    double c;
    switch (op) {
    case '+':
        c = a + b;
        printf("Ответ: %.2lf\n", c);
        break;
    case '-':
        c = a - b;
        printf("Ответ: %.2lf\n", c);
        break;
    case '*':
        c = a * b;
        printf("Ответ: %.2lf\n", c);
        break;
    case '/':
        if (b == 0) {
            printf("Ну давай давай ты же такой умный\n");
            break;
        }
        c = a / b;
        printf("Ответ: %.2lf\n", c);
        break;
    default:
        printf("Ну давай давай ты же такой умный\n");
        break;
    }
    fflush(stdout);
}

int launch_command(char **args) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            printf("Процесс %d убит сигналом %d\n", pid, WTERMSIG(status));
            return -1;
        }
    }
    return 0;
}

int builtin_commands(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        
        const char* message = "fuckoff\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "calc") == 0) {
        if(args[1] == NULL || args[2] == NULL || args[3] == NULL) {
            printf("СКОТИНА ЕБАНАЯ АРГУМЕНТЫ ДАЙ!\n");
            return 1;
        }
        
        calc(atof(args[1]), atof(args[2]), args[3][0]);
        return 1;
    }

    if (strcmp(args[0], "help") == 0){
        printf("there is no help and no escape. cope LOL\n\0");
        sleep(2);
        printf("jk lmao\n\n\0");
        printf("exit to exit\n\0");
        printf("help to help\n\0");
        printf("calc <num1> <num2> <operator> to calc basically\n\0");
        printf("i am a moron\n\0");
        return 1;
    }

    if (strcmp(args[0], "history") == 0) {
        HISTORY_STATE *hist_state = history_get_history_state();
        HIST_ENTRY **history_lists = history_list();
    
        if (history_list) {
            for (int i = 0; i < hist_state->length; i++) {
                printf("%d: %s\n", i + 1, history_lists[i]->line);
            }
        } else {
            printf("No commands in history.\n");
        }
        return 1;
    }

    

    return 0;
}

void execute_command(char* command) {
    char *args[64];
    int i = 0;

    char command_copy[MAX_INPUT_SIZE];
    strcpy(command_copy, command);

    char *token = strtok(command_copy, " ");
    while (token != NULL && i < 63) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (i == 0) {
        return;
    }

    if (builtin_commands(args)) {
        return;
    }
    
    int ret = launch_command(args);
}

void clear_screen() {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    else if (pid == 0) {
        execlp("clear", "clear", NULL);
        exit(EXIT_FAILURE);
    }
    else {
        waitpid(pid, NULL, 0);
    }
}

int main() {
    clear_screen();
    using_history();
    read_history(".lmao");
    char input[MAX_INPUT_SIZE];
    printf("\033[33mWelcome to\033[0m\n");
    printf("Whatever the \033[31mfuck\033[0m this is\n");
    printf("\033[36mhelp\033[0m is for help (i guess)\n\n");

    while (1) {
        write_history(".lmao");
        char *input = readline("shalava > ");
        if (!input) {  
            printf("\n");
            exit(EXIT_SUCCESS);

        }
        if (strlen(input) > 0) {
            add_history(input);
        }


        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        
        execute_command(input);
        free(input);
    }
    return 0;
}