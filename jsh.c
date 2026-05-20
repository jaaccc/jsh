#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 256
#define TOKEN_BUFFER_SIZE 16
#define TOKEN_DELIMITER " \t\r\n\a"

int jsh_launch(char **args) {
    pid_t pid, wpid;
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
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
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
    char *buffer = malloc(sizeof(char) * buffer_size);
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
        args = jsh_split();
        status = jsh_execute();

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    jsh_loop();

    return 0;
}
