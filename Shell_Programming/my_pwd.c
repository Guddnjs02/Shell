/* my_pwd.c */
#include <stdio.h>
#include <unistd.h>

#define MAX_PATH 1024

int main() {
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
        return 1;
    }
    return 0;
}
