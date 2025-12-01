/* my_cp.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define COLOR_RED    "\x1b[31m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("cp: source open error");
        return 1;
    }

    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("cp: dest open error");
        close(src_fd);
        return 1;
    }

    char buf[4096];
    ssize_t nread;
    while ((nread = read(src_fd, buf, sizeof(buf))) > 0) {
        if (write(dst_fd, buf, nread) != nread) {
            perror("cp: write error");
            break;
        }
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}
