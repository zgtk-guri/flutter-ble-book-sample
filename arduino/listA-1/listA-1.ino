#include <Adafruit_TinyUSB.h>
#include <bluefruit.h>

const int ledPin = 19;
const int switchPin = 14;
// const int ledPin = 1;
// const int switchPin = 4;

int prevSwitchState;  // 前回のタクトスイッチの状態を保存する変数
int prevPrevSwitchState;    // 前々回のタクトスイッチの状態を保存する変数

// サービスのUUID 3096c932-14c0-4ac7-8372-36dce85ac7a4
// 配列の並び順は1オクテットごとに逆順になることに注意
const uint8_t MY_SERVICE_UUID[] =
{
  0xa4, 0xc7, 0x5a, 0xe8, 0xdc, 0x36, 0x72, 0x83,
  0xc7, 0x4a, 0xc0, 0x14, 0x32, 0xc9, 0x96, 0x30
};
BLEService myService(MY_SERVICE_UUID);

// LED制御キャラクタリスティック
// UUID 3096c932-14c0-4ac7-8372-36dce85ac7a5
const uint8_t LED_CHARACTERISTIC_UUID[] =
{
  0xa5, 0xc7, 0x5a, 0xe8, 0xdc, 0x36, 0x72, 0x83,
  0xc7, 0x4a, 0xc0, 0x14, 0x32, 0xc9, 0x96, 0x30
};
BLECharacteristic ledCharacteristic(LED_CHARACTERISTIC_UUID);

// タクトスイッチ状態通知キャラクタリスティック
// UUID 3096c932-14c0-4ac7-8372-36dce85ac7a6
const uint8_t SW_CHARACTERISTIC_UUID[] =
{
  0xa6, 0xc7, 0x5a, 0xe8, 0xdc, 0x36, 0x72, 0x83,
  0xc7, 0x4a, 0xc0, 0x14, 0x32, 0xc9, 0x96, 0x30
};
BLECharacteristic swCharacteristic(SW_CHARACTERISTIC_UUID);


void setup()
{
  Serial.begin(115200);

  // GPIOの初期化
  pinMode(ledPin, OUTPUT);            // LEDのピンを出力に設定
  pinMode(switchPin, INPUT_PULLUP);   // タクトSWのピンを入力に設定
                                      // 内蔵のプルアップ抵抗も有効に
  digitalWrite(ledPin, LOW);          // LEDを消灯

  // 最初のタクトスイッチの状態を保存
  prevSwitchState = digitalRead(switchPin);
  prevPrevSwitchState = prevSwitchState;     // 前々回の状態も保存


  // BLEライブラリの初期化
  Bluefruit.begin();

  // デバイス名の設定
  Bluefruit.setName("LED Button");

  // サービスの初期化
  myService.begin();

  // キャラクタリスティックの初期化
  // LED制御キャラクタリスティック
  // プロパティ: Read, Write
  // パーミッション: Open to read, Open to write
  // データの長さ: 1バイト（固定）
  ledCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  ledCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  ledCharacteristic.setFixedLen(1);
  ledCharacteristic.begin();

  // タクトスイッチ状態通知キャラクタリスティック
  // プロパティ: Read, Notify
  // パーミッション: Open to read, cannot write
  // データの長さ: 1バイト（固定）
  swCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  swCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  swCharacteristic.setFixedLen(1);
  swCharacteristic.begin();

  // キャラクタリスティックの初期値を設定
  ledCharacteristic.write8(0x00);
  swCharacteristic.write8(digitalRead(switchPin) == LOW ? 0x01 : 0x00);

  // LED制御キャラクタリスティックの書き込みコールバック関数を設定
  ledCharacteristic.setWriteCallback(led_write_callback);

  // アドバタイジングの準備
  // BLEのみのデバイスとしてアドバタイジングを開始
  Bluefruit.Advertising.addFlags(
    BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE
  );
  // アドバタイジングパケットにTxPower（送信電力）を追加
  Bluefruit.Advertising.addTxPower();

  // サービスのUUIDをアドバタイジングパケットに追加
  Bluefruit.Advertising.addService(myService);

  // スキャンの応答に、デバイス名を追加
  Bluefruit.ScanResponse.addName();

  // アドバタイジングの設定
  // 切断時に自動でアドバタイジングを再開する
  Bluefruit.Advertising.restartOnDisconnect(true);
  // アドバタイジングの間隔を設定
  // 高速モード: 32 * 0.625ms = 20ms
  // 低速モード: 244 * 0.625ms = 152.5ms
  Bluefruit.Advertising.setInterval(32, 244);
  // アドバタイジングのタイムアウトを設定
  // 30秒間、高速でアドバタイジングを行う
  Bluefruit.Advertising.setFastTimeout(30);
  // アドバタイジングを開始（タイムアウトなし）
  Bluefruit.Advertising.start(0);
}


void led_write_callback(
  uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len
)
{
  Serial.print("LED Write Callback: ");
  Serial.println(data[0], HEX);
  if (data[0] == 0x00) {
    // 0x00のときLEDを消灯
    digitalWrite(ledPin, LOW);
  } else {
    // 0x01のときLEDを点灯
    digitalWrite(ledPin, HIGH);
  }
}

void loop() {
  int switchState = digitalRead(switchPin);
  if (
    switchState == prevSwitchState && 
    switchState != prevPrevSwitchState
  ) {
    uint8_t newValue;
    if (switchState == LOW) {
      // 押されたとき
      Serial.println("SW Push");
      newValue = 0x01;
    } else {                              
      // 離されたとき
      Serial.println("SW Release");
      newValue = 0x00;
    }
    // キャラクタリスティックの値を更新
    swCharacteristic.write8(newValue);
    // 通知（Notify）を送信
    if (Bluefruit.connected() && swCharacteristic.notifyEnabled()) {
      swCharacteristic.notify8(newValue);
    }
  }
  prevPrevSwitchState = prevSwitchState;    // 前々回の状態を更新
  prevSwitchState = switchState;            // 前回の状態を保存
  delay(10);                                // 10ミリ秒待つ
}

