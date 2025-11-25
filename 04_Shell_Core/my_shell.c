#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

/* --- 매크로 상수 정의 --- */
#define MAX_CMD_LEN  1024   // 최대 명령어 길이
#define MAX_ARG      64     // 최대 인자 개수
#define DELIMITERS   " \t\n" // 토큰 구분자 (공백, 탭, 개행)

/* --- 텍스트 색상 정의 (ANSI Escape Codes) --- */
#define COLOR_RESET  "\x1b[0m"
#define COLOR_CYAN   "\x1b[36m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"

/* --- 전역 변수 --- */
// (없음 - 가능한 지역 변수 사용 지향)

/*
 * 함수 프로토타입 선언
 * (코드의 가독성을 높이기 위해 함수들을 미리 선언합니다.)
 */
void setup_signal_handlers();
void handle_sigint(int signo);
void print_prompt();
void print_welcome_msg();
void print_help();
int tokenize_command(char *cmd_line, char *argv[]);
int check_background(char *argv[], int argc);
void handle_redirection(char *argv[]);
void execute_builtin_cd(char *argv[]);
void execute_single_command(char *argv[], int is_bg);
void execute_piped_command(char *cmd1[], char *cmd2[], int is_bg);
void process_command_line(char *cmd_line);

/*
 * ======================================================================================
 * main 함수: 쉘의 진입점
 * ======================================================================================
 */
int main() {
    char cmd_line[MAX_CMD_LEN];

    // 1. 초기화: 시그널 핸들러 설정 및 환영 메시지 출력
    setup_signal_handlers();
    print_welcome_msg();

    // 2. 메인 루프 (REPL: Read-Eval-Print Loop)
    while (1) {
        // 프롬프트 출력
        print_prompt();

        // 사용자 입력 대기 (fgets 사용으로 버퍼 오버플로우 방지)
        if (fgets(cmd_line, MAX_CMD_LEN, stdin) == NULL) {
            // EOF(Ctrl-D) 입력 시 정상 종료
            printf("\n%s로그아웃 (EOF detected).%s\n", COLOR_YELLOW, COLOR_RESET);
            break;
        }

        // 입력된 명령어 처리 위임
        process_command_line(cmd_line);
    }

    return 0;
}

/*
 * ======================================================================================
 * 시그널 핸들러 설정 함수
 * 기능: SIGINT(Ctrl-C), SIGQUIT(Ctrl-Z)에 대한 동작을 정의합니다.
 * ======================================================================================
 */
void setup_signal_handlers() {
    // SIGINT (Ctrl-C): 쉘이 종료되지 않고 프롬프트를 다시 띄우도록 설정
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal(SIGINT) error");
        exit(EXIT_FAILURE);
    }

    // SIGQUIT (Ctrl-Z): 쉘에서는 무시하도록 설정
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGQUIT) error");
        exit(EXIT_FAILURE);
    }
}

/*
 * SIGINT 핸들러
 * 설명: Ctrl-C 입력 시 줄을 바꾸고 프롬프트를 다시 출력합니다.
 */
void handle_sigint(int signo) {
    printf("\n");
    print_prompt();
    fflush(stdout);
}

/*
 * 프롬프트 출력 함수
 * 기능: 현재 작업 디렉토리(CWD)를 포함하여 프롬프트를 출력합니다.
 */
void print_prompt() {
    char cwd[1024];
    
    // 현재 디렉토리 경로 가져오기
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s%s%s$ ", COLOR_CYAN, cwd, COLOR_RESET);
    } else {
        // 경로를 가져오지 못한 경우 기본 프롬프트
        printf("%smy_shell%s> ", COLOR_CYAN, COLOR_RESET);
    }
    fflush(stdout); // 버퍼 강제 비움
}

/*
 * 환영 메시지 출력 함수
 */
void print_welcome_msg() {
    printf("========================================================\n");
    printf("       Welcome to %sMy Custom Linux Shell%s v1.0\n", COLOR_GREEN, COLOR_RESET);
    printf("       Type 'help' to see supported commands.\n");
    printf("========================================================\n");
}

/*
 * 도움말 출력 함수
 */
void print_help() {
    printf("\n--- Shell Supported Features ---\n");
    printf("1. Internal Commands:\n");
    printf("   - cd [dir]: Change directory\n");
    printf("   - exit: Exit the shell\n");
    printf("   - help: Show this help message\n");
    printf("2. External Commands: Supports standard Linux commands (ls, cp, vi...)\n");
    printf("3. Features:\n");
    printf("   - Pipe (|): cmd1 | cmd2\n");
    printf("   - Redirection (<, >): cmd > file, cmd < file\n");
    printf("   - Background (&): cmd &\n");
    printf("--------------------------------\n");
}

/*
 * 명령어 토큰화 함수
 * 설명: 입력 문자열을 공백 단위로 잘라 argv 배열에 저장합니다.
 * 반환값: 토큰의 개수 (argc)
 */
int tokenize_command(char *cmd_line, char *argv[]) {
    int count = 0;
    char *token;

    // 첫 번째 토큰 추출
    token = strtok(cmd_line, DELIMITERS);
    
    while (token != NULL && count < MAX_ARG - 1) {
        argv[count++] = token;
        // 다음 토큰 추출
        token = strtok(NULL, DELIMITERS);
    }
    
    argv[count] = NULL; // 인자 리스트의 끝은 반드시 NULL이어야 함
    return count;
}

/*
 * 백그라운드 실행 확인 함수
 * 설명: 명령어 마지막에 '&'가 있는지 확인하고, 있으면 제거합니다.
 * 반환값: 1(백그라운드), 0(포그라운드)
 */
int check_background(char *argv[], int argc) {
    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
        argv[argc - 1] = NULL; // '&' 기호 제거
        return 1;
    }
    return 0;
}

/*
 * 입출력 재지향 처리 함수
 * 설명: 명령어 인자 중 '<' 또는 '>'를 찾아 파일 디스크립터를 연결합니다.
 */
void handle_redirection(char *argv[]) {
    int i;
    int fd;

    for (i = 0; argv[i] != NULL; i++) {
        // 1. 입력 재지향 (<)
        if (strcmp(argv[i], "<") == 0) {
            argv[i] = NULL; // 기호 제거
            if (argv[i+1] == NULL) {
                fprintf(stderr, "Error: No input file specified.\n");
                exit(EXIT_FAILURE);
            }
            
            // 파일 읽기 전용으로 열기
            fd = open(argv[i+1], O_RDONLY);
            if (fd < 0) {
                perror("open input file failed");
                exit(EXIT_FAILURE);
            }
            
            // 표준 입력(0)을 파일로 교체
            dup2(fd, STDIN_FILENO);
            close(fd);
            
            // 파일명 인자 처리 완료 (루프 계속 진행하여 '>'도 찾음)
        } 
        // 2. 출력 재지향 (>)
        else if (strcmp(argv[i], ">") == 0) {
            argv[i] = NULL; // 기호 제거
            if (argv[i+1] == NULL) {
                fprintf(stderr, "Error: No output file specified.\n");
                exit(EXIT_FAILURE);
            }
            
            // 파일 쓰기 전용으로 열기 (없으면 생성, 있으면 내용 삭제)
            fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open output file failed");
                exit(EXIT_FAILURE);
            }
            
            // 표준 출력(1)을 파일로 교체
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
    }
}

/*
 * 내장 명령어 'cd' 실행 함수
 */
void execute_builtin_cd(char *argv[]) {
    char *path = argv[1];

    // 인자가 없으면 홈 디렉토리로 이동
    if (path == NULL) {
        path = getenv("HOME");
    }

    if (chdir(path) < 0) {
        fprintf(stderr, "%scdn: No such file or directory: %s%s\n", 
                COLOR_RED, path, COLOR_RESET);
    }
}

/*
 * 단일 명령어 실행 함수 (fork-exec 구조)
 */
void execute_single_command(char *argv[], int is_bg) {
    pid_t pid;
    int status;

    pid = fork(); // 자식 프로세스 생성

    if (pid < 0) {
        // fork 실패 시
        perror("fork failed");
        return;
    } 
    else if (pid == 0) {
        // [자식 프로세스]
        
        // 시그널 핸들링: 자식은 기본 동작(종료)을 따름
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        // 재지향 처리 (파일 입출력 연결)
        handle_redirection(argv);

        // 명령어 실행
        if (execvp(argv[0], argv) < 0) {
            fprintf(stderr, "%s%s: command not found%s\n", 
                    COLOR_RED, argv[0], COLOR_RESET);
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // [부모 프로세스]
        if (is_bg) {
            // 백그라운드 실행: 기다리지 않음
            printf("[%d] %s started in background\n", pid, argv[0]);
        } else {
            // 포그라운드 실행: 자식이 끝날 때까지 대기
            // waitpid를 사용하여 특정 자식만 기다림
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid failed");
            }
        }
    }
}

/*
 * 파이프라인 명령어 실행 함수 (cmd1 | cmd2)
 */
void execute_piped_command(char *arg1[], char *arg2[], int is_bg) {
    int pfd[2]; // 파이프 파일 디스크립터
    pid_t pid1, pid2;

    // 파이프 생성
    if (pipe(pfd) < 0) {
        perror("pipe failed");
        return;
    }

    // 첫 번째 자식 (Writer)
    if ((pid1 = fork()) == 0) {
        // 파이프 읽기 포트 닫기 (쓰기만 함)
        close(pfd[0]);
        
        // 표준 출력을 파이프 쓰기 포트로 연결
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[1]);

        // 재지향 처리 및 실행
        handle_redirection(arg1);
        
        if (execvp(arg1[0], arg1) < 0) {
            fprintf(stderr, "%s: command not found\n", arg1[0]);
            exit(1);
        }
    }

    // 두 번째 자식 (Reader)
    if ((pid2 = fork()) == 0) {
        // 파이프 쓰기 포트 닫기 (읽기만 함)
        close(pfd[1]);
        
        // 표준 입력을 파이프 읽기 포트로 연결
        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]);

        // 재지향 처리 및 실행
        handle_redirection(arg2);
        
        if (execvp(arg2[0], arg2) < 0) {
            fprintf(stderr, "%s: command not found\n", arg2[0]);
            exit(1);
        }
    }

    // 부모 프로세스: 파이프 포트 모두 닫기 (중요: 안 닫으면 자식이 안 끝남)
    close(pfd[0]);
    close(pfd[1]);

    // 백그라운드가 아니면 두 자식이 끝날 때까지 대기
    if (!is_bg) {
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    } else {
        printf("[%d] Pipe command started in background\n", pid1);
    }
}

/*
 * 명령어 라인 처리 함수 (Main Logic)
 * 설명: 입력된 문자열을 분석하여 파이프 여부 확인 후 적절한 실행 함수 호출
 */
void process_command_line(char *cmd_line) {
    char *argv[MAX_ARG];
    char *arg1[MAX_ARG], *arg2[MAX_ARG];
    char *pipe_pos;
    int argc;
    int is_bg = 0;

    // 개행 문자 제거
    if (cmd_line[strlen(cmd_line) - 1] == '\n')
        cmd_line[strlen(cmd_line) - 1] = '\0';

    // 빈 명령어 체크
    if (strlen(cmd_line) == 0) return;

    // 1. 파이프(|) 처리 확인
    pipe_pos = strchr(cmd_line, '|');
    
    if (pipe_pos != NULL) {
        // 파이프가 있는 경우
        *pipe_pos = '\0'; // 파이프 문자를 기준으로 문자열 분리
        char *cmd2_str = pipe_pos + 1;

        // 앞뒤 명령어 각각 파싱
        tokenize_command(cmd_line, arg1);
        int count2 = tokenize_command(cmd2_str, arg2);

        // 백그라운드 체크 (두 번째 명령어 기준)
        is_bg = check_background(arg2, count2);

        // 파이프 실행
        execute_piped_command(arg1, arg2, is_bg);
        return;
    }

    // 2. 일반 명령어 처리 (파이프 없음)
    argc = tokenize_command(cmd_line, argv);

    if (argc == 0) return; // 공백만 입력된 경우

    // 3. 백그라운드(&) 확인
    is_bg = check_background(argv, argc);

    // 4. 내장 명령어 처리
    if (strcmp(argv[0], "exit") == 0) {
        printf("Goodbye!\n");
        exit(0);
    } 
    else if (strcmp(argv[0], "help") == 0) {
        print_help();
    }
    else if (strcmp(argv[0], "cd") == 0) {
        execute_builtin_cd(argv);
    }
    // 5. 외부 명령어 실행
    else {
        execute_single_command(argv, is_bg);
    }
}
