#define PIR 2       // PIR 신호선이 아두이노 2번핀에 연결
#define button_1 3  // 1번 버튼이 아두이노 3번핀에 연결
#define button_2 4  // 2번 버튼이 아두이노 4번핀에 연결
#define button_3 5  // 3번 버튼이 아두이노 5번핀에 연결
#define button_4 6  // 4번 버튼이 아두이노 6번핀에 연결
#define FAN 9       // FAN과 연결된 신호선이 아두이노 9번핀에 연결

// 추후 사용될 True(1 / HIGH)와 False(0 / LOW)로 이루어진 boolean 형식의 변수
bool on_off = LOW;
bool lock = LOW;
bool flag_1 = LOW;
bool flag_3 = LOW;
bool flag_4 = LOW;

int second = 0;
int minute = 0;

void setup() {
  Serial.begin(9600);   // 시리얼 모니터를 사용하기 위한 시작문
  pinMode(PIR, INPUT);  // PIR 센서를 입력(INPUT) 모드로 설정
  pinMode(button_1, INPUT_PULLUP); // 1번 버튼을 입력모드로 설정 ( 풀업회로 연결 )
  pinMode(button_2, INPUT_PULLUP); // 2번 버튼을 입력모드로 설정 ( 풀업회로 연결 )
  pinMode(button_3, INPUT_PULLUP); // 3번 버튼을 입력모드로 설정 ( 풀업회로 연결 )
  pinMode(button_4, INPUT_PULLUP); // 4번 버튼을 입력모드로 설정 ( 풀업회로 연결 )
  pinMode(FAN, OUTPUT); // FAN을 출력(OUTPUT) 모드로 설정  
}

void loop() {
  // vr(variable resitor)이란 변수에 가변저항의 아날로그 값을 저장
  int vr = analogRead(A0); 
  // vr 값을 analogread(vr)의 범위인 0~1023에서 analogwrite의 범위인 0~255로 비례변환
  vr = map(vr,0,1023,0,255);
  bool detect = digitalRead(PIR);      // PIR 센서에서 입력받은 값을 detect 변수에 저장
  bool power = digitalRead(button_1);  // 1번 버튼에서 입력받은 값을 power 변수에 저장
  bool enable = digitalRead(button_2); // 2번 버튼에서 입력받은 값을 enable 변수에 저장
  bool plus = digitalRead(button_3);  // 1번 버튼에서 입력받은 값을 plus 변수에 저장
  bool minus = digitalRead(button_4); // 2번 버튼에서 입력받은 값을 minus 변수에 저장
  
  if(detect == HIGH && lock == LOW && on_off == LOW){  // PIR 센서 감지 + 감지가 활성화 상태 일때
    Serial.println("PIR detect");
    digitalWrite(FAN,HIGH);           // FAN 작동
    lock = HIGH;                      // 감지 비활성화
    on_off = HIGH;                    // FAN이 동작중인 상태임을 나타냄
  }

  // 한 번 버튼을 눌렀을 때 여러번 인식하는 현상이 있어 flag_n이라는 변수를 통해 억제
  if(power == LOW && flag_1 == LOW){    // 1번 버튼이 눌림
    Serial.println("button_1_push");  // 시리얼 모니터에 문구 출력 - 동작 여부 확인 가능
    on_off = !on_off;                 // FAN이 동작중인 상태 반전
    digitalWrite(FAN,on_off);         // 반전된 FAN 상태를 FAN에 적용
    if(on_off == LOW){
      second = 0;
      minute = 0;
    }
    flag_1 = HIGH;
  }else if(power == HIGH){
    flag_1 = LOW;
    delay(10);
  }

  if(enable == LOW){                  // 2번 버튼이 눌림
    Serial.println("button_2_push");
    lock = 0;                         // 감지 활성화
    delay(1000);
  }

  if(plus == LOW && flag_3 == LOW && on_off == LOW){  // 3번 버튼이 눌림
    Serial.print("button_3_push | ");
    minute += 2;                                      // 타이머 2분 증가
    Serial.println(minute);
    flag_3 = HIGH;                
  }else if(plus == HIGH){
    flag_3 = LOW;
    delay(10);
  }

  if(minus == LOW && flag_4 == LOW && on_off == LOW){ // 4번 버튼이 눌림
    Serial.print("button_4_push | ");
    minute -= 2;                                  // 타이머 2분 감소
    if(minute < 0) minute = 0;                    // 타이머가 0보다 감소되면 0으로 초기화
    Serial.println(minute);
    flag_4 = HIGH;
  }else if(minus == HIGH){
    flag_4 = LOW;
    delay(10);
  }

  if(minute > 0 && on_off == HIGH){ // 타이머 값이 설정된 상태 + FAN 구동
    delay(1000);                    // 1초 지연
    second++;                       // 1초 카운트
    if(second >= 60){               // 1분 경과
      minute -= 1;                  // 1분 카운트
      second = 0;                   // 초 단위 초기화
      if(minute <= 0){              // 타이머가 종료되었을 때
        on_off = LOW;               // FAN 정지
        digitalWrite(FAN, on_off);
      }
    }
    Serial.print("second : ");
    Serial.print(second);
    Serial.print("s / minute : ");
    Serial.println(minute);
  }
  
  if(on_off == HIGH) {      // FAN이 동작 중일 때
    if(vr <= 25 ) vr = 25;  // FAN의 속도값이 25이하 일때, 25로 고정
    analogWrite(FAN, vr);   // vr 값에 따라 FAN의 속도 제어
  }else if(on_off == LOW){  // FAN이 동작 중이 아닐 때
    analogWrite(FAN, 0);    // FAN의 속도를 0으로 고정 ( 선풍기 off )
  }

  
}
