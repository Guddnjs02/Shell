#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>     // opendir, readdir, closedir
#include <sys/stat.h>   // lstat
#include <unistd.h>     // lstat

/*
 * 디렉토리 경로를 받아 재귀적으로 탐색하는 함수
 * base_path: 현재 탐색 중인 디렉토리 경로 (예: ".", "./test_dir")
 * indent_level: 출력을 예쁘게 하기 위한 들여쓰기 레벨
 */
void list_dir_recursive(const char *base_path, int indent_level) {
    DIR *dir_p;
    struct dirent *entry;
    struct stat stat_buf;
    
    // 1. 디렉토리 열기
    dir_p = opendir(base_path);
    if (dir_p == NULL) {
        // 권한이 없거나 파일이 아닌 경우
        fprintf(stderr, "Error opening directory '%s'\n", base_path);
        return;
    }

    // 2. 디렉토리 내부 순회
    while ((entry = readdir(dir_p)) != NULL) {
        // 2-1. 현재(.) 및 부모(..) 디렉토리는 건너뜀
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 2-2. 파일/디렉토리의 전체 경로 생성
        // (예: "test_dir" + "/" + "another_file.txt" = "test_dir/another_file.txt")
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);

        // 2-3. lstat으로 파일 정보 가져오기 (심볼릭 링크는 따라가지 않음)
        if (lstat(full_path, &stat_buf) == -1) {
            perror("lstat");
            continue;
        }

        // 2-4. 들여쓰기 적용하여 이름 출력
        for (int i = 0; i < indent_level; i++) {
            printf("  "); // 들여쓰기
        }

        // 2-5. 파일 유형 판별 및 재귀 호출
        if (S_ISDIR(stat_buf.st_mode)) {
            // [디렉토리]인 경우
            printf("[DIR] \t%s/\n", entry->d_name);
            // 재귀 호출: 이 함수를 다시 호출하여 하위 디렉토리로 들어감
            list_dir_recursive(full_path, indent_level + 1);
        } else if (S_ISREG(stat_buf.st_mode)) {
            // [파일]인 경우
            printf("[FILE]\t%s\n", entry->d_name);
        } else if (S_ISLNK(stat_buf.st_mode)) {
            // [링크]인 경우
            printf("[LNK] \t%s\n", entry->d_name);
        } else {
            // [기타]
            printf("[OTHER]\t%s\n", entry->d_name);
        }
    }

    // 3. 디렉토리 닫기
    closedir(dir_p);
}

int main() {
    printf("--- Recursive Listing from '.' ---\n");
    // 현재 디렉토리(".")에서부터 탐색 시작, 들여쓰기 레벨 0
    list_dir_recursive(".", 0);
    return 0;
}
