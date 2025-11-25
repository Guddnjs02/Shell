#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    char cmd[100], path[200];
    while (1) {
        printf("cd_shell> ");
        scanf("%s", cmd);

        if (strcmp(cmd, "cd") == 0) {
            scanf("%s", path);
            if (chdir(path) == 0) {
                printf("Moved to: %s\n", path);
            } else {
                perror("cd error");
            }
        }
    }
    return 0;
}

