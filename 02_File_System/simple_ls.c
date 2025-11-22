#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>     // opendir, readdir, closedir
#include <sys/stat.h>   // lstat, struct stat
#include <unistd.h>     // lstat
#include <string.h>     // strcmp

/*
 * 현재 디렉토리의 파일/디렉토리 목록과 그 유형을 출력하는 프로그램
 */
int main() {
    DIR *dir_p;
    struct dirent *entry; // 디렉토리 항목 정보를 담을 구조체
    struct stat stat_buf; // 파일 상태 정보를 담을 구조체

    // 1. 현재 디렉토리(".") 열기
    dir_p = opendir(".");
    if (dir_p == NULL) {
        perror("opendir");
        return 1;
    }

    printf("--- 현재 디렉토리 목록 ---\n");

    // 2. 디렉토리 항목을 하나씩 읽기
    while ((entry = readdir(dir_p)) != NULL) {
        // 2-1. "." (현재) 와 ".." (부모) 디렉토리는 건너뜀
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 2-2. lstat으로 파일 정보 가져오기 (심볼릭 링크 자체를 확인)
        if (lstat(entry->d_name, &stat_buf) == -1) {
            perror("lstat");
            continue;
        }

        // 2-3. 파일 유형 판별
        if (S_ISDIR(stat_buf.st_mode)) {
            printf("[DIR] \t%s\n", entry->d_name);
        } else if (S_ISREG(stat_buf.st_mode)) {
            printf("[FILE]\t%s\n", entry->d_name);
        } else if (S_ISLNK(stat_buf.st_mode)) {
            printf("[LNK] \t%s\n", entry->d_name);
        } else {
            printf("[OTHER]\t%s\n", entry->d_name);
        }
    }

    // 3. 디렉토리 스트림 닫기
    closedir(dir_p);
    return 0;
}
