use std::io::{self, Write};
use std::collections::HashMap; // HashMap을 사용하기 위해 import

/// 사용자로부터 한 줄을 입력받아 String으로 반환하는 헬퍼 함수
fn read_line(prompt: &str) -> String {
    print!("{}", prompt);
    // 프롬프트를 먼저 출력하기 위해 stdout을 flush합니다.
    io::stdout().flush().expect("출력 버퍼를 비우는 데 실패했습니다.");

    let mut input = String::new();
    io::stdin().read_line(&mut input)
        .expect("입력을 읽는 데 실패했습니다.");
    
    // 양 끝의 공백을 제거한 String을 반환합니다.
    input.trim().to_string()
}

/// 메인 함수
fn main() {
    // 1. 새로운 HashMap 생성
    // Key: String (이름), Value: String (전화번호)
    let mut phone_book: HashMap<String, String> = HashMap::new();

    println!("--- Rust 전화번호부 프로그램 ---");
    println!("(사용 가능 명령: '추가', '검색', '전체보기', '종료')");

    // 프로그램 메인 루프
    loop {
        let command = read_line("\n명령을 입력하세요: ");

        // 입력된 명령에 따라 분기
        match command.as_str() {
            "추가" => {
                let name = read_line("  이름: ");
                let phone = read_line("  전화번호: ");

                // HashMap에 데이터 삽입(추가)
                // insert() 함수는 기존에 name 키가 있었다면 덮어씁니다.
                phone_book.insert(name.clone(), phone);
                println!("=> '{}'님을 저장했습니다.", name);
            }
            "검색" => {
                let name = read_line("  검색할 이름: ");

                // HashMap에서 데이터 검색
                // get() 함수는 해당 키의 값에 대한 참조(Option<&String>)를 반환합니다.
                match phone_book.get(&name) {
                    Some(phone) => {
                        println!("=> '{}'님의 전화번호: {}", name, phone);
                    }
                    None => {
                        println!("=> '{}'님을 찾을 수 없습니다.", name);
                    }
                }
            }
            "전체보기" => {
                if phone_book.is_empty() {
                    println!("=> 전화번호부가 비어있습니다.");
                } else {
                    println!("\n--- 전체 전화번호부 ---");
                    // HashMap의 모든 (key, value) 쌍을 순회
                    for (name, phone) in &phone_book {
                        println!("- {}: {}", name, phone);
                    }
                }
            }
            "종료" => {
                println!("프로그램을 종료합니다.");
                break; // loop를 빠져나가 프로그램 종료
            }
            _ => {
                println!("! 알 수 없는 명령입니다.");
                println!("  ('추가', '검색', '전체보기', '종료' 중 하나를 입력하세요.)");
            }
        }
    }
}
