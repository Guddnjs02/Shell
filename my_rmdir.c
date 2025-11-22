/* my_rmdir.c */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "사용법: %s <디렉토리명>\n", argv[0]);
        return 1;
    }
    // 디렉토리 삭제 (비어있어야 함)
    if (rmdir(argv[1]) == -1) {
        perror("rmdir");
        return 1;
    }
    return 0;
}
