/* my_rm.c */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define COLOR_RED    "\x1b[31m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "%srm: missing operand%s\n", COLOR_RED, COLOR_RESET);
        return 1;
    }

    int i = 1;
    while (argv[i] != NULL) {
        if (unlink(argv[i]) < 0) {
            fprintf(stderr, "rm: cannot remove '%s': %s\n", argv[i], strerror(errno));
        }
        i++;
    }
    return 0;
}
