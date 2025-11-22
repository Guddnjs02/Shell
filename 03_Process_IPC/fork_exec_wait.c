/*
 * 8장 프로세스 제어
 * 파일 이름: fork_exec_wait.c
 * (교재의 forkexecls.c  기반)
 */
#include <stdio.h>
#include <stdlib.h>     // exit
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork, execl
#include <sys/wait.h>   // wait

int main()
{
    pid_t pid;

    // 1. 새로운 프로세스 생성
    pid = fork();

    if (pid < 0) {
        // --- fork 실패 ---
        perror("fork failed");
        exit(1);

    } else if (pid == 0) {
        // --- 2. 자식 프로세스 ---
        printf("--- Child process is running (ls -l) ---\n");
        
        // execl로 /bin/ls 프로그램을 실행
        execl("/bin/ls", "ls", "-l", (char *) 0);
        
        // execl이 실패한 경우에만 아래 코드가 실행됨
        perror("execl failed");
        exit(2);

    } else {
        // --- 3. 부모 프로세스 ---
        
        // wait()로 자식 프로세스가 끝날 때까지 대기
        wait((int *) 0); // status 값을 받지 않으려면 (int*)0 또는 NULL 전달
        
        printf("--- Child process finished ---\n");
        printf("Parent process is exiting.\n");
        exit(0);
    }
}
