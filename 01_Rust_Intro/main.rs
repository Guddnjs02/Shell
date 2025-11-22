use std::io::{self, Write};

/// 사용자로부터 양의 정수(행/열 크기)를 입력받는 헬퍼 함수
fn read_usize(prompt: &str) -> usize {
    loop {
        print!("{}", prompt);
        // 프롬프트를 먼저 출력하기 위해 stdout을 flush합니다.
        io::stdout().flush().expect("출력 버퍼를 비우는 데 실패했습니다.");

        let mut input = String::new();
        io::stdin().read_line(&mut input)
            .expect("입력을 읽는 데 실패했습니다.");

        // 입력받은 문자열을 공백 제거 후 숫자로 파싱
        match input.trim().parse() {
            Ok(num) => {
                if num > 0 {
                    return num; // 0보다 큰 값이면 반환
                } else {
                    println!("0보다 큰 값을 입력해야 합니다.");
                }
            },
            Err(_) => println!("유효한 양의 정수를 입력하세요."),
        }
    }
}

/// 사용자로부터 정수(행렬 원소)를 입력받는 헬퍼 함수
fn read_i32(prompt: &str) -> i32 {
    loop {
        print!("{}", prompt);
        io::stdout().flush().expect("출력 버퍼를 비우는 데 실패했습니다.");

        let mut input = String::new();
        io::stdin().read_line(&mut input)
            .expect("입력을 읽는 데 실패했습니다.");

        match input.trim().parse() {
            Ok(num) => return num, // 정수이면 반환
            Err(_) => println!("유효한 정수를 입력하세요."),
        }
    }
}

/// 지정된 크기(rows, cols)의 행렬을 사용자로부터 입력받아
/// Vec<Vec<i32>> 형태로 반환합니다.
fn read_matrix(name: &str, rows: usize, cols: usize) -> Vec<Vec<i32>> {
    println!("\n--- 행렬 {}의 원소를 입력합니다. ---", name);
    
    // Vec<T>를 사용하여 동적 할당 (rows 크기만큼 미리 용량 확보)
    let mut matrix: Vec<Vec<i32>> = Vec::with_capacity(rows);

    for r in 0..rows {
        // 각 행(row)에 대해 Vec<T>를 사용하여 동적 할당 (cols 크기만큼 용량 확보)
        let mut row: Vec<i32> = Vec::with_capacity(cols);
        for c in 0..cols {
            let prompt = format!("[{}] {}행 {}열 원소: ", name, r + 1, c + 1);
            // 헬퍼 함수를 호출하여 원소 입력
            row.push(read_i32(&prompt));
        }
        // 완성된 행을 전체 행렬 Vec에 추가
        matrix.push(row);
    }
    matrix
}

/// 두 행렬(a, b)을 더하여 새로운 행렬(c)을 반환합니다.
fn add_matrices(a: &Vec<Vec<i32>>, b: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    let rows = a.len();
    let cols = a[0].len(); // 0이 아닌 크기를 가정

    // 결과 행렬 c를 Vec<T>로 동적 할당
    let mut c: Vec<Vec<i32>> = Vec::with_capacity(rows);

    for r in 0..rows {
        // 결과 행 c_row를 Vec<T>로 동적 할당
        let mut c_row: Vec<i32> = Vec::with_capacity(cols);
        for c in 0..cols {
            // A[r][c] + B[r][c]
            c_row.push(a[r][c] + b[r][c]);
        }
        c.push(c_row);
    }
    c
}

/// 행렬을 콘솔에 출력합니다.
fn print_matrix(matrix: &Vec<Vec<i32>>, title: &str) {
    println!("\n--- {} ---", title);
    for row in matrix {
        for val in row {
            print!("{}\t", val); // 탭(\t)으로 원소 구분
        }
        println!(); // 각 행이 끝나면 줄바꿈
    }
}

/// 메인 함수
fn main() {
    // 1. 두 행렬의 크기를 입력받음
    let rows = read_usize("행렬의 행(rows) 크기를 입력하세요: ");
    let cols = read_usize("행렬의 열(cols) 크기를 입력하세요: ");

    // 2. 행렬 A를 입력받아 동적으로 할당
    let matrix_a = read_matrix("A", rows, cols);

    // 3. 행렬 B를 입력받아 동적으로 할당
    let matrix_b = read_matrix("B", rows, cols);

    // 4. 두 행렬을 더함
    let matrix_c = add_matrices(&matrix_a, &matrix_b);

    // 5. 결과 출력
    print_matrix(&matrix_a, "행렬 A");
    print_matrix(&matrix_b, "행렬 B");
    print_matrix(&matrix_c, "결과 행렬 (A + B)");
}
