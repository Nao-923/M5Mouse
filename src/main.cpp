#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>
#include <ClosedCube_TCA9548A.h>
#include <SparkFun_BNO08x_Arduino_Library.h>
#include <Adafruit_SGP30.h>


#define TCA9548A_I2C_ADDRESS	0x70
#define SGP_CH 0
#define BNO_CH 1

ClosedCube::Wired::TCA9548A tca9548a;
BNO08x vrimu;
Adafruit_SGP30 sgp;

void setup() {
  M5.begin();
  Wire.begin();

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting PaHub test...");

  tca9548a.address(TCA9548A_I2C_ADDRESS);

  // --- SGP30 初期化 ---
  tca9548a.selectChannel(SGP_CH);
  if (!sgp.begin()) {
    Serial.println("SGP30 not found");
  } else {
    Serial.println("SGP30 initialized");
    sgp.IAQinit();
    delay(1000);
  }

  // --- BNO08x 初期化 ---
  tca9548a.selectChannel(BNO_CH);
  if (!vrimu.begin()) {
    Serial.println("BNO08x not found");
  } else {
    Serial.println("BNO08x initialized");
    vrimu.enableRotationVector(50); // 50ms = 20Hz
  }
}

void loop() {
}