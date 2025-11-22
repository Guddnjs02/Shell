#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // fork, read, STDIN_FILENO
#include <sys/ipc.h>    // msgget, msgsnd, msgrcv, msgctl, IPC_CREAT, IPC_RMID
#include <sys/msg.h>    // struct msqid_ds
#include <sys/select.h> // select, fd_set
#include <errno.h>      // errno, ENOMSG
#include <sys/wait.h>

#define MSG_SIZE 256
#define MSG_KEY  1234  // 프로세스들이 공유할 고유한 키

// 메시지 큐를 위한 메시지 버퍼 구조체
// [cite: 1065-1068]
struct msg_buffer {
    long msg_type; // 메시지 타입 (0보다 커야 함)
    char msg_text[MSG_SIZE];
};

// 채팅 루프 함수
void chat_loop(int msqid, long my_type, long other_type) {
    fd_set read_set;
    struct msg_buffer buffer;
    int nread;

    printf("채팅 시작 (종료는 Ctrl+D):\n");

    while (1) {
        // 1. select()가 감시할 대상 설정
        FD_ZERO(&read_set);
        FD_SET(STDIN_FILENO, &read_set); // 0번 (키보드) 감시

        // 2. 키보드 입력이 올 때까지 대기 (타임아웃 1초)
        // (메시지 큐는 파일 디스크립터가 아니므로 select로 감시 불가)
        struct timeval tv = {1, 0}; // 1초
        select(STDIN_FILENO + 1, &read_set, NULL, NULL, &tv);

        // 3. 키보드에 입력이 발생했는지 확인
        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            nread = read(STDIN_FILENO, buffer.msg_text, MSG_SIZE);
            if (nread <= 0) { // Ctrl+D (EOF) 또는 오류
                printf("채팅을 종료합니다.\n");
                break;
            }
            buffer.msg_text[nread - 1] = '\0'; // 개행 문자 제거
            buffer.msg_type = my_type;

            // 메시지 큐에 '나의 타입'으로 메시지 전송 [cite: 1061-1064]
            if (msgsnd(msqid, &buffer, strlen(buffer.msg_text) + 1, 0) == -1) {
                perror("msgsnd");
                break;
            }
        }

        // 4. '상대방 타입'의 메시지를 non-blocking으로 확인 [cite: 1081-1082, 1089-1090]
        if (msgrcv(msqid, &buffer, MSG_SIZE, other_type, IPC_NOWAIT) != -1) {
            // 메시지 수신 성공
            printf("[상대방] %s\n", buffer.msg_text);
        } else {
            // 수신 실패. 큐가 비어있으면(ENOMSG) 정상, 그 외는 오류.
            if (errno != ENOMSG) {
                perror("msgrcv");
                break;
            }
        }
    }
}

int main() {
    int msqid;
    pid_t pid;
    long my_type, other_type;

    // 1. 메시지 큐 생성 (없으면 생성) [cite: 1045-1048]
    msqid = msgget((key_t)MSG_KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    // 2. 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /* --- 자식 프로세스 --- */
        my_type = 2;
        other_type = 1;
        chat_loop(msqid, my_type, other_type);
        
    } else {
        /* --- 부모 프로세스 --- */
        my_type = 1;
        other_type = 2;
        chat_loop(msqid, my_type, other_type);
        
        // 3. 부모가 종료 시 메시지 큐 삭제 [cite: 1097, 1101]
        wait(NULL); // 자식이 종료될 때까지 대기
        if (msgctl(msqid, IPC_RMID, NULL) == -1) {
            perror("msgctl (IPC_RMID)");
            exit(1);
        }
    }

    return 0;
}
