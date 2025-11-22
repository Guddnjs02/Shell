#include <stdio.h>
#include <stdlib.h>     // exit
#include <termios.h>    // tcgetattr, tcsetattr
#include <unistd.h>     // read, write, STDIN_FILENO, ttyname, fileno
#include <string.h>     // strcmp
#include <time.h>       // time, difftime

int main(void) {
    int fd;
    int nread, cnt = 0, errcnt = 0;
    char ch;
    // 연습할 문장
    const char *text = "The magic thing is that you can change it.";
    int text_len = strlen(text);
    
    struct termios init_attr, new_attr;
    time_t start_time, end_time;
    double elapsed_seconds;
    double cpm; // Characters Per Minute (분당 타자수)

    // 1. 현재 터미널(/dev/tty)의 파일 디스크립터를 엃음
    fd = STDIN_FILENO; // 표준 입력을 사용
    
    // 2. 현재 터미널 속성을 가져와서 init_attr에 저장
    if (tcgetattr(fd, &init_attr) != 0) {
        fprintf(stderr, "터미널 속성을 가져올 수 없음.\n");
        return 1;
    }

    // 3. 비정규 모드(Non-Canonical Mode)로 속성 변경
    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON; // 정규 모드 끔
    new_attr.c_lflag &= ~ECHO;   // 입력 문자 화면 표시 끔 (수동으로 처리하기 위해)
    new_attr.c_cc[VMIN] = 1;     // 최소 1글자 입력 시 read 반환
    new_attr.c_cc[VTIME] = 0;    // 시간 제한 없음

    // 4. 변경된 속성 적용
    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
        return 1;
    }

    printf("--- 타자 연습 프로그램 ---\n");
    printf("다음 문장을 그대로 입력하세요. (Enter 키로 종료)\n");
    printf("%s\n", text);

    // 5. 첫 입력 시작 시간 측정
    start_time = time(NULL);

    // 6. 메인 루프: 사용자가 문장 길이만큼 또는 Enter를 칠 때까지
    while ((nread = read(fd, &ch, 1)) > 0 && cnt < text_len) {
        // Enter 키(개행 문자 \n) 입력 시 종료
        if (ch == '\n') {
            break;
        }

        if (ch == text[cnt]) {
            // 올바르게 입력한 경우, 해당 문자를 화면에 출력 (ECHO)
            write(fd, &ch, 1);
        } else {
            // 틀리게 입력한 경우, "*"를 화면에 출력
            write(fd, "*", 1);
            errcnt++;
        }
        cnt++; // 다음 문자로 이동
    }
    
    // 7. 입력 종료 시간 측정
    end_time = time(NULL);

    // 8. 원래 터미널 속성으로 복원
    tcsetattr(fd, TCSANOW, &init_attr);

    // 9. 결과 계산 및 출력
    printf("\n\n--- 타자 연습 결과 ---\n");
    printf("오타 수: %d\n", errcnt);

    elapsed_seconds = difftime(end_time, start_time);
    if (elapsed_seconds <= 0) {
        elapsed_seconds = 1.0; // 0으로 나누기 방지 (매우 빨리 끝낸 경우)
    }

    // 분당 타자수(CPM) 계산: (총 친 글자 수 / 총 걸린 시간(초)) * 60
    cpm = (cnt / elapsed_seconds) * 60.0;
    
    printf("총 걸린 시간: %.2f 초\n", elapsed_seconds);
    printf("평균 분당 타자수(CPM): %.0f\n", cpm);

    close(fd);
    return 0;
}
