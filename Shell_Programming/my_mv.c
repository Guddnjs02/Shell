/* my_mv.c */
#include <stdio.h>
#include <unistd.h>

#define COLOR_RED    "\x1b[31m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%smv: missing file operand%s\n", COLOR_RED, COLOR_RESET);
        return 1;
    }
    if (rename(argv[1], argv[2]) < 0) {
        perror("mv");
        return 1;
    }
    return 0;
}
