#include <Adafruit_TinyUSB.h>

const int switchPin = 14;
// const int switchPin = 4;

int prevSwitchState;        // 前回のタクトスイッチの状態を保存する変数

void setup() {
  Serial.begin(115200);               // シリアル通信を開始
  pinMode(switchPin, INPUT_PULLUP);   // タクトSWのピンを入力に設定
                                      // 内蔵のプルアップ抵抗も有効に
  // 最初のタクトスイッチの状態を保存
  prevSwitchState = digitalRead(switchPin);
}

void loop() {
  // タクトスイッチの状態を読み取る
  int switchState = digitalRead(switchPin);
  if (switchState != prevSwitchState) {     // 状態が変化したとき
    if (switchState == LOW) {               // 押されたとき
      Serial.println("SW Push");            // シリアルモニタに出力
    } else {                                // 離されたとき
      Serial.println("SW Release");         // シリアルモニタに出力
    }
  }
  prevSwitchState = switchState;            // 状態を保存
  delay(10);                                // 10ミリ秒待つ
}

