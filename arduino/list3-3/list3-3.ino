#include <Adafruit_TinyUSB.h>

const int switchPin = 14;
// const int switchPin = 4;

void setup() {
  Serial.begin(115200);               // シリアル通信を開始
  pinMode(switchPin, INPUT_PULLUP);   // タクトSWのピンを入力に設定
                                      // 内蔵のプルアップ抵抗も有効に
}

void loop() {
  if (digitalRead(switchPin) == LOW) {    // 押されている時はLOWになる
    // タクトスイッチが押されているとき
    Serial.println("SW ON");              // シリアルモニタに出力
  } else {
    // タクトスイッチが押されていないとき
    Serial.println("SW OFF");             // シリアルモニタに出力
  }
  delay(10);                              // 10ミリ秒待つ
}

