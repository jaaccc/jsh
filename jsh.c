#include <stdio.h>

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