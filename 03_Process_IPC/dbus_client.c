#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = NULL;
    sd_bus *bus = NULL;
    const char *server_reply;
    int ret = 0;

    // 1. D-Bus의 '세션(사용자) 버스'에 연결
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "세션 버스 연결 실패: %s\n", strerror(-ret));
        exit(1);
    }

    // 2. D-Bus 메소드 호출
    printf("서버의 'Hello' 메소드를 호출합니다...\n");
    ret = sd_bus_call_method(bus,
                             "org.example.MyService",     // 서비스 이름 (서버)
                             "/org/example/MyService",  // 객체 경로
                             "org.example.MyService",  // 인터페이스 이름
                             "Hello",                  // 메소드 이름
                             &error,                   // 오류 반환용
                             &msg,                     // 응답 메시지용
                             "s",                      // 보낼 인자 타입 (string)
                             "클라이언트입니다");     // 보낼 메시지

    if (ret < 0) {
        fprintf(stderr, "메소드 호출 실패: %s\n", error.message);
        goto finish;
    }

    // 3. 서버로부터 받은 응답 메시지(문자열 's')를 읽음
    ret = sd_bus_message_read(msg, "s", &server_reply);
    if (ret < 0) {
        fprintf(stderr, "응답 메시지 읽기 실패: %s\n", strerror(-ret));
        goto finish;
    }
    printf("서버 응답: %s\n", server_reply);
finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(msg);
    sd_bus_unref(bus);
    return 0;
}
