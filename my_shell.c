#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CMD_LEN 1024
#define MAX_ARG 64

// 시그널 핸들러: 쉘이 죽지 않고 프롬프트만 다시 출력
void sig_handler(int signo) {
    printf("\nfinal_shell> ");
    fflush(stdout);
}

// 공백을 기준으로 문자열을 토큰으로 분리하는 함수
int tokenize(char *buf, char *argv[]) {
    int n = 0;
    char *token = strtok(buf, " \t");
    while (token != NULL && n < MAX_ARG - 1) {
        argv[n++] = token;
        token = strtok(NULL, " \t");
    }
    argv[n] = NULL;
    return n;
}

int main() {
    char cmd_line[MAX_CMD_LEN];
    char *argv[MAX_ARG];
    char *arg1[MAX_ARG], *arg2[MAX_ARG]; // 파이프용 명령어 분리
    int status;
    pid_t pid;

    // 3. 시그널 처리 (Ctrl-C, Ctrl-Z 무시/핸들링)
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, SIG_IGN); // Ctrl-Z 무시

    while (1) {
        printf("final_shell> ");
        if (fgets(cmd_line, MAX_CMD_LEN, stdin) == NULL) break;
        
        cmd_line[strlen(cmd_line) - 1] = '\0'; // 개행 문자 제거
        if (strlen(cmd_line) == 0) continue;

        // 1. "exit" 명령 처리
        if (strcmp(cmd_line, "exit") == 0) {
            printf("Shell terminating...\n");
            break;
        }

        // 2. 백그라운드 실행 확인 ('&')
        int is_bg = 0;
        if (cmd_line[strlen(cmd_line) - 1] == '&') {
            is_bg = 1;
            cmd_line[strlen(cmd_line) - 1] = '\0'; 
        }

        // 4-2. 파이프(|) 처리 확인
        char *pipe_pos = strchr(cmd_line, '|');
        int has_pipe = 0;
        
        if (pipe_pos != NULL) {
            has_pipe = 1;
            *pipe_pos = '\0'; // 파이프 문자를 기준으로 문자열 분리
            char *cmd2_str = pipe_pos + 1; // 파이프 뒤쪽 명령어

            tokenize(cmd_line, arg1);   // 앞쪽 명령어 파싱
            tokenize(cmd2_str, arg2);   // 뒤쪽 명령어 파싱
        } else {
            // 파이프가 없으면 일반 파싱
            tokenize(cmd_line, argv);
        }

        if (!has_pipe && argv[0] == NULL) continue;

        // 5. 내장 명령어 "cd" (파이프 없을 때만 동작)
        if (!has_pipe && strcmp(argv[0], "cd") == 0) {
            if (argv[1] == NULL) chdir(getenv("HOME"));
            else if (chdir(argv[1]) == -1) perror("cd");
            continue;
        }

        // --- 명령어 실행 ---
        if (has_pipe) {
            // 파이프가 있는 경우 (자식 2개 생성)
            int pfd[2];
            pipe(pfd);

            if (fork() == 0) { // 첫 번째 명령어 (Writer)
                close(pfd[0]);
                dup2(pfd[1], STDOUT_FILENO); // 표준 출력을 파이프로
                close(pfd[1]);
                execvp(arg1[0], arg1);
                perror("pipe cmd1");
                exit(1);
            }

            if (fork() == 0) { // 두 번째 명령어 (Reader)
                close(pfd[1]);
                dup2(pfd[0], STDIN_FILENO); // 표준 입력을 파이프로
                close(pfd[0]);
                execvp(arg2[0], arg2);
                perror("pipe cmd2");
                exit(1);
            }

            close(pfd[0]);
            close(pfd[1]);
            if (!is_bg) {
                wait(NULL);
                wait(NULL);
            }
        } 
        else {
            // 일반 명령어 (파이프 없음)
            pid = fork();
            if (pid == 0) {
                // 자식 프로세스: 시그널 동작을 기본값으로 복구
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);

                // 4-1. 재지향 (Redirection) 처리 (<, >)
                // (명령어 인자를 순회하며 <, > 찾기)
                for (int i = 0; argv[i] != NULL; i++) {
                    if (strcmp(argv[i], ">") == 0) {
                        // 출력 재지향
                        int fd = open(argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                        argv[i] = NULL; // '>' 뒤쪽 인자는 execvp에 전달 안 함
                    } else if (strcmp(argv[i], "<") == 0) {
                        // 입력 재지향
                        int fd = open(argv[i+1], O_RDONLY);
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                        argv[i] = NULL;
                    }
                }

                execvp(argv[0], argv);
                perror("command execution failed");
                exit(1);
            } 
            else if (pid > 0) {
                if (!is_bg) waitpid(pid, &status, 0);
                else printf("[Background PID: %d]\n", pid);
            }
        }
    }
    return 0;
}
