const int ledPin = 19;
// const int ledPin = 1;

void setup() {
  pinMode(ledPin, OUTPUT);    // LEDのピンを出力に設定
  digitalWrite(ledPin, LOW);  // LEDを消灯
}

void loop() {
  digitalWrite(ledPin, HIGH); // LEDを点灯
  delay(1000);                // 1秒待つ
  digitalWrite(ledPin, LOW);  // LEDを消灯
  delay(1000);                // 1秒待つ
}

