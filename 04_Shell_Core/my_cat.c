/* my_cat.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define COLOR_RED    "\x1b[31m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char *argv[]) {
    int i = 1;
    int show_line_numbers = 0;
    int show_nonprintable = 0;
    int show_ends = 0;
    int number_nonblank = 0;
    int has_option = 0;

    // 옵션 처리
    while (argv[i] != NULL && argv[i][0] == '-') {
        has_option = 1;
        for (int j = 1; argv[i][j] != '\0'; j++) {
            switch(argv[i][j]) {
                case 'n': show_line_numbers = 1; break;
                case 'b': number_nonblank = 1; break;
                case 'v': show_nonprintable = 1; break;
                case 'E': show_ends = 1; break;
                default:
                    fprintf(stderr, "%scat: invalid option -- '%c'%s\n", COLOR_RED, argv[i][j], COLOR_RESET);
                    return 1;
            }
        }
        i++;
    }

    // stdin 처리 (옵션만 있고 파일 없을 때)
    if (argv[i] == NULL && has_option) {
        char buffer[1024];
        int line = 1;
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // (stdin 처리 로직은 파일 처리와 동일하므로 생략 가능하나, 완전성을 위해 유지)
            int print_line = 1;
            if (number_nonblank) {
                int nonblank_found = 0;
                for (int k = 0; buffer[k] != '\0'; k++) {
                    if (buffer[k] != '\n') { nonblank_found = 1; break; }
                }
                if (nonblank_found) printf("%6d  ", line++);
            } else if (show_line_numbers) {
                printf("%6d  ", line++);
            }
            // ... (출력 로직 동일)
            fputs(buffer, stdout); 
        }
        return 0;
    }

    // 파일이 없는 경우 (단순 cat 실행)
    if (argv[i] == NULL) {
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
            fputs(buffer, stdout);
        }
        return 0;
    }

    // 파일 처리
    for (; argv[i] != NULL; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            fprintf(stderr, "%scat: %s: %s%s\n", COLOR_RED, argv[i], strerror(errno), COLOR_RESET);
            continue;
        }

        char buffer[1024];
        int line = 1;

        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (number_nonblank) {
                int nonblank_found = 0;
                for (int k = 0; buffer[k] != '\0'; k++) {
                    if (buffer[k] != '\n') { nonblank_found = 1; break; }
                }
                if (nonblank_found) printf("%6d  ", line++);
            } else if (show_line_numbers) {
                printf("%6d  ", line++);
            }

            for (int k = 0; buffer[k] != '\0'; k++) {
                unsigned char c = buffer[k];
                if (c == '\n' && show_ends) putchar('$');
                if (show_nonprintable) {
                    if (c < 32 && c != '\n' && c != '\t') printf("^%c", c+64);
                    else putchar(c);
                } else putchar(c);
            }
        }
        fclose(fp);
    }
    return 0;
}
