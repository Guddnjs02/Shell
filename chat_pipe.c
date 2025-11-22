#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // pipe, fork, read, write, STDIN_FILENO
#include <sys/select.h> // select, fd_set
#include <string.h>     // strlen

#define BUFSIZE 256

// 채팅 루프 함수
// read_fd: 상대방으로부터 메시지를 읽어올 파이프
// write_fd: 상대방에게 메시지를 보낼 파이프
void chat_loop(int read_fd, int write_fd) {
    char buf[BUFSIZE];
    int nread;
    fd_set read_set; // select()가 감시할 파일 디스크립터 집합
    
    printf("채팅 시작 (종료는 Ctrl+D):\n");

    while (1) {
        // 1. 감시할 대상 설정
        FD_ZERO(&read_set);          // 집합 초기화
        FD_SET(STDIN_FILENO, &read_set); // 0번 (키보드) 감시
        FD_SET(read_fd, &read_set);      // 파이프(읽기) 감시

        // 2. select() 호출: 키보드 또는 파이프에 데이터가 올 때까지 대기
        if (select(read_fd + 1, &read_set, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        // 3. 키보드에 입력이 발생했는지 확인
        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            // 키보드에서 읽어서
            nread = read(STDIN_FILENO, buf, BUFSIZE);
            if (nread <= 0) { // Ctrl+D (EOF) 또는 오류
                printf("채팅을 종료합니다.\n");
                break;
            }
            // 파이프로 쓴다 (상대방에게 전송)
            write(write_fd, buf, nread);
        }

        // 4. 파이프에 입력이 발생했는지 확인
        if (FD_ISSET(read_fd, &read_set)) {
            // 파이프에서 읽어서
            nread = read(read_fd, buf, BUFSIZE);
            if (nread <= 0) { // 상대방이 연결을 끊음
                printf("상대방이 채팅을 종료했습니다.\n");
                break;
            }
            // 화면(표준 출력)에 쓴다
            write(STDOUT_FILENO, "[상대방] ", strlen("[상대방] "));
            write(STDOUT_FILENO, buf, nread);
        }
    }
}

int main() {
    int pipe_p_to_c[2]; // 부모 -> 자식
    int pipe_c_to_p[2]; // 자식 -> 부모
    pid_t pid;

    // 1. 파이프 2개 생성
    if (pipe(pipe_p_to_c) == -1) {
        perror("pipe_p_to_c"); exit(1);
    }
    if (pipe(pipe_c_to_p) == -1) {
        perror("pipe_c_to_p"); exit(1);
    }

    // 2. 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork"); exit(1);
    }

    if (pid == 0) {
        /* --- 자식 프로세스 --- */
        // 사용할 파이프 정리
        close(pipe_p_to_c[1]); // P->C 쓰기 (X)
        close(pipe_c_to_p[0]); // C->P 읽기 (X)
        
        // 자식은 부모의 P->C 파이프로부터 읽고,
        // C->P 파이프로 쓴다.
        chat_loop(pipe_p_to_c[0], pipe_c_to_p[1]);
        
        // 종료 시 파이프 닫기
        close(pipe_p_to_c[0]);
        close(pipe_c_to_p[1]);
        
    } else {
        /* --- 부모 프로세스 --- */
        // 사용할 파이프 정리
        close(pipe_p_to_c[0]); // P->C 읽기 (X)
        close(pipe_c_to_p[1]); // C->P 쓰기 (X)
        
        // 부모는 자식의 C->P 파이프로부터 읽고,
        // P->C 파이프로 쓴다.
        chat_loop(pipe_c_to_p[0], pipe_p_to_c[1]);
        
        // 종료 시 파이프 닫기
        close(pipe_c_to_p[1]);
        close(pipe_c_to_p[0]);
    }

    return 0;
}
