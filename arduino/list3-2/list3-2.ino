const int ledPin = 19;
const int switchPin = 14;
// const int ledPin = 1;
// const int switchPin = 4;

void setup() {
  pinMode(ledPin, OUTPUT);            // LEDのピンを出力に設定
  pinMode(switchPin, INPUT_PULLUP);   // タクトSWのピンを入力に設定
                                      // 内蔵のプルアップ抵抗も有効に
  digitalWrite(ledPin, LOW);          // LEDを消灯
}

void loop() {
  if (digitalRead(switchPin) == LOW) {    // 押されている時はLOWになる
    // タクトスイッチが押されているとき
    digitalWrite(ledPin, HIGH);           // LEDを点灯
  } else {
    // タクトスイッチが押されていないとき
    digitalWrite(ledPin, LOW);            // LEDを消灯
  }
  delay(10);                              // 10ミリ秒待つ
}

