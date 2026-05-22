#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 256
#define TOKEN_BUFFER_SIZE 16
#define TOKEN_DELIMITER " \t\r\n\a"

int jsh_cd(char **args);
int jsh_help(char **args);
int jsh_exit(char **args);

char *builtin_list[] = {"cd", "help", "exit"};
int (*builtin_function[])(char **) = {&jsh_cd, &jsh_help, &jsh_exit};
int jsh_builtin_count() { return sizeof(builtin_list) / sizeof(char *); }

int jsh_cd(char **args) {
    if (args[1] == NULL)
        fprintf(stderr, "jsh: expected argument to \"cd\"\n");
    else if (chdir(args[1]) != 0)
        perror("jsh");
    return 1;
}

int jsh_help(char **args) {
    int i;
    printf("jaaccc's shell\n");
    printf("type function name followed by arguments, and hit enter\n");
    printf("the following functions are built in:\n");

    for (i = 0; i < jsh_builtin_count(); i++)
        printf("    %s\n", builtin_list[i]);

    printf("use the man function for more information on each function\n");
    return 1;
}

int jsh_exit(char **args) { return 0; }

int jsh_launch(char **args) {
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1)
            perror("jsh");
        exit(1);
    } else if (pid < 0) {
        perror("jsh");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int jsh_execute(char **args) {
    int i;

    if (args[0] == NULL)
        return 1;

    for (i = 0; i < jsh_builtin_count(); i++)
        if (strcmp(args[0], builtin_list[i]) == 0)
            return (*builtin_function[i])(args);

    return jsh_launch(args);
}

char **jsh_split(char *line) {
    int buffer_size = TOKEN_BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "jsh: allocation error\n");
        exit(1);
    }

    token = strtok(line, TOKEN_DELIMITER);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "jsh: allocation error\n");
                exit(1);
            }
        }

        token = strtok(NULL, TOKEN_DELIMITER);
    }
    tokens[position] = NULL;
    return tokens;
}

char *jsh_read(void) {
    int buffer_size = READ_BUFFER_SIZE;
    int position = 0;
    char *buffer = malloc(buffer_size);
    int character;

    if (!buffer) {
        fprintf(stderr, "jsh: allocation error\n");
        exit(1);
    }

    while (1) {
        character = getchar();

        if (character == EOF || character == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = character;
        }
        position++;

        if (position >= buffer_size) {
            buffer_size += READ_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);

            if (!buffer) {
                fprintf(stderr, "jsh: allocation error\n");
                exit(1);
            }
        }
    }
}

void jsh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf(". ");
        line = jsh_read();
        args = jsh_split(line);
        status = jsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    jsh_loop();

    return 0;
}
