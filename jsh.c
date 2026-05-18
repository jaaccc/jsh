#include <stdio.h>
#include <stdlib.h>
#define READ_BUFFER_SIZE 256

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
