#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // open, O_RDONLY, O_RDWR, O_CREAT, O_TRUNC
#include <unistd.h>     // close, ftruncate
#include <sys/stat.h>   // fstat, struct stat
#include <sys/mman.h>   // mmap, munmap, PROT_READ, PROT_WRITE, MAP_SHARED
#include <string.h>     // memcpy

int main(int argc, char *argv[]) {
    int src_fd, dest_fd;
    char *src_addr, *dest_addr;
    struct stat stat_buf;
    off_t file_size;

    // 1. 인자 개수 확인
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <원본파일> <대상파일>\n", argv[0]);
        exit(1);
    }

    // 2. 원본 파일 열기 (읽기 전용)
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("원본 파일 열기 실패");
        exit(1);
    }

    // 3. 대상 파일 열기 (읽기/쓰기, 없으면 생성, 있으면 내용 지움)
    dest_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("대상 파일 열기 실패");
        close(src_fd);
        exit(1);
    }

    // 4. 원본 파일의 크기 가져오기 (fstat 사용)
    if (fstat(src_fd, &stat_buf) == -1) {
        perror("파일 상태 읽기 실패");
        exit(1);
    }
    file_size = stat_buf.st_size;

    // 5. 대상 파일의 크기를 원본 파일과 같게 설정 (매우 중요)
    // mmap은 파일의 실제 크기만큼만 매핑할 수 있으므로, 대상 파일의 크기를 미리 늘려줘야 합니다.
    if (ftruncate(dest_fd, file_size) == -1) {
        perror("대상 파일 크기 설정 실패");
        exit(1);
    }

    // 6. 원본 파일을 메모리에 매핑 (읽기 전용)
    src_addr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_addr == MAP_FAILED) {
        perror("원본 파일 mmap 실패");
        exit(1);
    }

    // 7. 대상 파일을 메모리에 매핑 (쓰기 가능)
    dest_addr = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, dest_fd, 0);
    if (dest_addr == MAP_FAILED) {
        perror("대상 파일 mmap 실패");
        munmap(src_addr, file_size); // 이미 매핑한 원본은 해제
        exit(1);
    }

    // 8. 메모리 복사 (핵심 작업)
    // (파일에서 읽고 쓰는 대신, 메모리에서 메모리로 직접 복사)
    memcpy(dest_addr, src_addr, file_size);

    printf("'%s' 파일을 '%s' 파일로 (mmap) 복사 완료. (총 %ld 바이트)\n", 
           argv[1], argv[2], file_size);

    // 9. 자원 해제
    munmap(src_addr, file_size);
    munmap(dest_addr, file_size);
    close(src_fd);
    close(dest_fd);

    return 0;
}
