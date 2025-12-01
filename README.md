## 🐚 오픈소스프로젝트 (Shell Programming)

이 저장소는 **오픈소스프로젝트** 과목의 팀 프로젝트 결과물입니다.  
리눅스 커널의 시스템 호출(System Call)을 직접 사용하여, 상용 쉘(Bash)과 유사한 기능을 수행하는 **나만의 쉘(Shell) 프로그램**을 구현하였습니다.

### 📌 프로젝트 개요 (Project Overview)

* **목표:** 프로세스 생성(`fork`), 실행(`exec`), 동기화(`wait`), 그리고 IPC(파이프) 및 파일 입출력 제어를 통해 쉘의 동작 원리를 이해하고 구현함.
* **개발 언어:** C Language
* **개발 환경:** Linux (Ubuntu), GCC Compiler, Make

### ✨ 주요 기능 (Key Features)

#### 1. 쉘 코어 (Shell Core)
* **내장 명령어 (Built-in Commands):**
    * **`exit`**: 쉘 프로그램을 정상적으로 종료합니다 (프로세스 리소스 정리 후 종료).
    * **`cd`**: 현재 작업 디렉토리를 변경합니다.
* **명령어 해석 및 실행:** 사용자 입력을 파싱하여 내장/외부 명령어를 구분하여 실행.
* **프로세스 제어:** `&` 기호를 통한 **백그라운드(Background) 실행** 지원.
* **시그널 처리:** `Ctrl-C` (SIGINT), `Ctrl-Z` (SIGQUIT) 입력 시 쉘이 종료되지 않도록 보호.
* **I/O 재지향:**
    * `>` : 출력 재지향 (명령어 결과를 파일로 저장)
    * `<` : 입력 재지향 (파일 내용을 명령어로 전달)
* **파이프라인:** `|` 기호를 사용하여 두 명령어의 입출력을 연결 (예: `ls | grep .c`).

#### 2. 구현된 명령어 (Custom Commands)
팀원들과 분담하여 리눅스의 핵심 명령어 11가지를 직접 구현하고 쉘에 통합하였습니다.

| 명령어 | 파일명 | 기능 설명 |
| :--- | :--- | :--- |
| **ls** | `my_ls.c` | 디렉토리 내 파일 목록 출력 (숨김 파일 처리 포함) |
| **pwd** | `my_pwd.c` | 현재 작업 디렉토리 경로 출력 |
| **cd** | (Built-in) | 작업 디렉토리 변경 (쉘 내장 기능으로 구현) |
| **mkdir** | `my_mkdir.c` | 새로운 디렉토리 생성 |
| **rmdir** | `my_rmdir.c` | 비어있는 디렉토리 삭제 |
| **cp** | `my_cp.c` | 파일 복사 (`open`, `read`, `write` 활용) |
| **mv** | `my_mv.c` | 파일 이동 및 이름 변경 (`rename` 활용) |
| **rm** | `my_rm.c` | 파일 삭제 (`unlink` 활용) |
| **ln** | `my_ln.c` | 하드 링크 생성 (`link` 활용) |
| **cat** | `my_cat.c` | 파일 내용 출력 (옵션 처리 포함) |
| **grep** | `my_grep.c` | 파일 내 문자열 검색 (다양한 옵션 지원) |

### ⚙️ 설치 및 실행 방법 (Installation & Usage)

소스 코드를 다운로드하고 `make`를 통해 빌드하여 실행합니다.

```bash
# 1. 저장소 클론 (Clone)
git clone [https://github.com/Guddnjs02/Shell.git](https://github.com/Guddnjs02/Shell.git)
cd Shell-main/Shell_Programming

# 2. 컴파일 (Build)
# Makefile이 포함되어 있어 make 명령어로 모든 파일을 한 번에 빌드합니다.
make

# 3. 쉘 실행 (Run)
cd Shell-main
cd Shell_Programming

gcc -o my_shell my_shell.c
gcc -o my_ls my_ls.c
gcc -o my_pwd my_pwd.c
gcc -o my_mkdir my_mkdir.c
gcc -o my_rmdir my_rmdir.c
gcc -o my_cp my_cp.c
gcc -o my_ln my_ln.c
gcc -o my_mv my_mv.c
gcc -o my_rm my_rm.c
gcc -o my_cat my_cat.c
gcc -o my_grep my_grep.c
./my_shell
