#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

// 클라이언트가 "Hello" 메소드를 호출했을 때 실행될 콜백 함수
static int method_hello(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    const char *client_msg;
    int ret = 0;
    char reply[256];

    // 1. 클라이언트가 보낸 메시지(문자열 's')를 읽음
    ret = sd_bus_message_read(m, "s", &client_msg);
    if (ret < 0) {
        fprintf(stderr, "메시지 읽기 실패: %s\n", strerror(-ret));
        return ret;
    }

    // 2. 응답 메시지 생성
    sprintf(reply, "서버: '%s' 메시지 잘 받았습니다!", client_msg);

    // 3. 클라이언트에게 응답(문자열 's')을 보냄
    return sd_bus_reply_method_return(m, "s", reply);
}

// 이 서버가 D-Bus에 제공할 기능 목록 (vtable)
// 교재의 vtable [cite: 2261-2276]을 단순화하여 'Hello' 메소드만 남김
static const sd_bus_vtable dbus_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("Hello", "s", "s", method_hello, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

int main() {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret = 0;

    // 1. D-Bus의 '세션(사용자) 버스'에 연결
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "세션 버스 연결 실패: %s\n", strerror(-ret));
        exit(1);
    }

    // 2. D-Bus에 'org.example.MyService'라는 이름으로 서비스 등록 요청
    ret = sd_bus_request_name(bus, "org.example.MyService", 0);
    if (ret < 0) {
        fprintf(stderr, "서비스 이름 요청 실패: %s\n", strerror(-ret));
        exit(1);
    }

    // 3. '/org/example/MyService' 경로에 위에서 정의한 기능(vtable)들을 등록
    ret = sd_bus_add_object_vtable(bus,
                                   &slot,
                                   "/org/example/MyService", // 객체 경로
                                   "org.example.MyService", // 인터페이스 이름
                                   dbus_vtable,
                                   NULL);
    if (ret < 0) {
        fprintf(stderr, "vtable 등록 실패: %s\n", strerror(-ret));
        exit(1);
    }

    printf("D-BUS 서버 실행 중... (클라이언트의 'Hello' 메소드 호출 대기)\n");

    // 4. 클라이언트의 요청을 기다리는 메인 루프
    while (1) {
        // 버스에서 이벤트가 있는지 처리
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "프로세스 처리 실패: %s\n", strerror(-ret));
            break;
        }
        
        // 이벤트가 없으면 잠시 대기
        if (ret == 0) {
            sd_bus_wait(bus, (uint64_t)-1);
        }
    }

    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);
    return 0;
}
