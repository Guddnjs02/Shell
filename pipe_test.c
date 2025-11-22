/*
 * 9장 프로세스간 통신
 * 파일 이름: pipe_test.c (교재 pipetest.c 예제 )
 * 기능: fork()와 pipe()를 이용한 부모-자식 간 통신
 */
#include <stdio.h>
#include <stdlib.h>     // exit
#include <unistd.h>     // pipe, fork, read, write, close
#include <string.h>     // sprintf

#define MSGSIZE 20 // 메시지 크기 정의

int main()
{
    char buf[MSGSIZE];
    int p[2], i;
    int pid;

    /* 1. 파이프 생성 */
    if (pipe(p) == -1) {
        perror("pipe call failed");
        exit(1);
    }

    /* 2. 자식 프로세스 생성 */
    pid = fork();

    if (pid == 0) {
        /* --- 자식 프로세스 --- */
        
        // 3. 자식: 읽기용 fd(p[0])를 닫는다.
        close(p[0]);
        
        // 4. 자식: p[1](쓰기용)으로 메시지를 두 번 보낸다.
        for (i = 0; i < 2; i++) {
            sprintf(buf, "Hello, world #%d", i + 1);
            write(p[1], buf, MSGSIZE);
        }
        
    } else if (pid > 0) {
        /* --- 부모 프로세스 --- */
        
        // 3. 부모: 쓰기용 fd(p[1])를 닫는다.
        close(p[1]);

        // 4. 부모: p[0](읽기용)에서 메시지를 두 번 읽어 출력한다.
        for (i = 0; i < 2; i++) {
            read(p[0], buf, MSGSIZE);
            printf("%s\n", buf);
        }
        
    } else {
        /* --- fork 실패 --- */
        perror("fork failed");
        exit(1);
    }
    
    return 0;
}
