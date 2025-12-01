/* my_ls.c */
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

int main() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // 숨김 파일 제외
            printf("%s  ", entry->d_name);
        }
    }
    printf("\n");
    closedir(dir);
    return 0;
}
