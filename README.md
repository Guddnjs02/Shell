# 👨‍💻 Linux System Programming Lab 2: 프로세스, 통신, 쉘 구현

이 저장소는 [과목명]의 **Lab2** 결과물입니다. 리눅스 환경에서 시스템 호출(System Calls)을 사용하여 파일 시스템, 프로세스 제어, 프로세스 간 통신(IPC) 메커니즘을 이해하고 구현하는 것을 목표로 합니다.

## ✨ 주요 구현 기능 (Features)

본 프로젝트에서는 다음과 같은 핵심 시스템 기능을 구현하고 테스트했습니다.

1.  **Rust 언어 실습 (01_Rust_Intro)**
    * `Vec<T>`를 이용한 행렬 덧셈 프로그램 구현.
    * `HashMap<K, V>`를 이용한 전화번호부 관리 및 검색 기능 구현.
  
2.  **파일 시스템 및 알고리즘 (02_File_System)**
    * `ls -R` 재귀 탐색 기능 구현 (`ls_recursive.c`).
    * `lstat()`을 이용한 파일 유형 및 권한 확인.
    * `termios`를 이용한 타자 연습 프로그램 구현 및 CPM(분당 타자수) 측정.

3.  **프로세스 간 통신 (IPC) 구현 (03_Process_IPC)**
    * **파이프 채팅:** `pipe()`와 `select()`를 이용한 양방향 통신 구현 (`chat_pipe.c`).
    * **메시지 큐:** `msgsnd/msgrcv`를 이용한 채팅 프로그램 구현 (`chat_mq.c`).
    * **공유 메모리:** `mmap()`을 이용한 고속 파일 복사 기능 (`mmap_copy.c`).
    * **D-BUS:** `sd-bus` 라이브러리를 사용한 서버-클라이언트 메시지 통신 구현.

4.  **쉘 구현 및 제어 (04_Shell_Core)**
    * **프로세스 제어:** `fork()`, `execvp()`, `waitpid()`를 사용한 명령어 실행.
    * **백그라운드 실행:** 명령어 끝에 `&`를 붙여 비동기 실행 가능.
    * **입출력 제어:** `dup2()`를 사용한 파일 재지향 (`<`, `>`) 및 `pipe()`를 사용한 명령어 파이프 연결 (`|`).
    * **시그널 처리:** `Ctrl-C`와 `Ctrl-Z`에 대해 쉘이 종료되지 않도록 보호 (`SIGINT`, `SIGQUIT`).
    * **내장 명령어:** `cd` 구현.

## 📁 저장소 구조

| 폴더명 | 내용 |
| :--- | :--- |
| **`01_Rust_Intro`** | Rust 언어의 `Vec` 및 `HashMap` 실습 소스 코드. |
| **`02_File_System`** | 파일/디렉토리 조작 (`ls -R`, `typing_test`, `simple_ls`) 관련 C 코드. |
| **`03_Process_IPC`** | 파이프, 메시지 큐, D-BUS, `mmap` 등 고급 IPC 구현 소스 코드. |
| **`04_Shell_Core`** | 13번 과제의 핵심 쉘(`final_shell.c`) 및 팀원 구현 명령어 (`my_ls.c`, `Makefile` 등). |
| **`test_dir`** | 재귀 탐색 및 파일 I/O 테스트용 디렉토리 구조. |

## ⚙️ 빌드 및 실행 방법

1.  **환경 설정:** Ubuntu Linux 환경에서 `gcc`, `make`, `git`, `rustc`, `libsystemd-dev`가 설치되어 있어야 합니다.
2.  **C 코드 컴파일:** `04_Shell_Core` 폴더로 이동하여 `make` 명령어를 사용하거나, 개별 파일에 대해 `gcc -g [파일명].c`를 사용합니다.
3.  **쉘 실행:** `./final_shell`을 실행하여 모든 기능을 테스트합니다.
