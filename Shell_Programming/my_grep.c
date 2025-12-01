/* my_grep.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define COLOR_RED    "\x1b[31m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char *argv[]) {
    int i = 1;
    int ignore_case = 0;
    int show_line_numbers = 0;
    int invert_match = 0;
    int count_only = 0;
    int list_files = 0;

    // 옵션 처리
    while (argv[i] != NULL && argv[i][0] == '-') {
        for (int j = 1; argv[i][j] != '\0'; j++) {
            switch (argv[i][j]) {
                case 'i': ignore_case = 1; break;
                case 'n': show_line_numbers = 1; break;
                case 'v': invert_match = 1; break;
                case 'c': count_only = 1; break;
                case 'l': list_files = 1; break;
                default:
                    fprintf(stderr, "%sgrep: invalid option -- '%c'%s\n", COLOR_RED, argv[i][j], COLOR_RESET);
                    return 1;
            }
        }
        i++;
    }

    if (argv[i] == NULL) {
        fprintf(stderr, "%sgrep: missing search pattern%s\n", COLOR_RED, COLOR_RESET);
        return 1;
    }

    char *pattern = argv[i++];

    // 파일 처리 루프
    for (; argv[i] != NULL; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            fprintf(stderr, "%sgrep: %s: %s%s\n", COLOR_RED, argv[i], strerror(errno), COLOR_RESET);
            continue;
        }

        char buffer[1024];
        int line = 1;
        int match_count = 0;
        int file_matched = 0;

        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            int match = 0;
            
            if (ignore_case) {
                // 대소문자 무시 검색 구현 (임시 버퍼 사용)
                char line_lower[1024], pat_lower[256];
                strcpy(line_lower, buffer);
                strcpy(pat_lower, pattern);
                for(int k=0; line_lower[k]; k++) line_lower[k] = tolower(line_lower[k]);
                for(int k=0; pat_lower[k]; k++) pat_lower[k] = tolower(pat_lower[k]);
                if (strstr(line_lower, pat_lower)) match = 1;
            } else {
                if (strstr(buffer, pattern)) match = 1;
            }

            if (invert_match) match = !match;

            if (match) {
                match_count++;
                file_matched = 1;
                if (!count_only && !list_files) {
                    if (show_line_numbers) printf("%6d: ", line);
                    fputs(buffer, stdout);
                }
            }
            line++;
        }

        if (count_only) printf("%d\n", match_count);
        if (list_files && file_matched) printf("%s\n", argv[i]);

        fclose(fp);
    }
    return 0;
}
