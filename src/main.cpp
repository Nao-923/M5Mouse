#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>
#include <ClosedCube_TCA9548A.h>
#include <SparkFun_BNO08x_Arduino_Library.h>
#include <Adafruit_SGP30.h>
#include <array>


#define TCA9548A_I2C_ADDRESS	0x70
#define SGP_CH 0
#define BNO_CH 1

ClosedCube::Wired::TCA9548A tca9548a;
BNO08x vrimu;
Adafruit_SGP30 sgp;

struct BNOData {
  float i;
  float j;
  float k;
  float real;
  float accuracy;
  bool valid;
};

struct SGPData {
  uint16_t TVOC;
  uint16_t eCO2;
  uint16_t rawH2;
  uint16_t rawEthanol;
  bool valid;
};

BNOData getBNOData() {
  BNOData quat;

  tca9548a.selectChannel(BNO_CH);

  if (vrimu.wasReset()) {
    Serial.println("sensor was reset");
  }

  if (vrimu.getSensorEvent() && vrimu.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
    quat.i = vrimu.getQuatI();
    quat.j = vrimu.getQuatJ();
    quat.k = vrimu.getQuatK();
    quat.real = vrimu.getQuatReal();
    quat.accuracy = vrimu.getQuatRadianAccuracy();
    quat.valid = true;
  }

  return quat;
}

SGPData getSGPData() {
  SGPData data;
  data.valid = false;  // 初期化

  tca9548a.selectChannel(SGP_CH);

  if (!sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return data;
  }

  if (!sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return data;
  }

  data.TVOC = sgp.TVOC;
  data.eCO2 = sgp.eCO2;
  data.rawH2 = sgp.rawH2;
  data.rawEthanol = sgp.rawEthanol;
  data.valid = true;

  return data;
}

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
  delay(10);
  BNOData bnoData = getBNOData();
  SGPData sgpData = getSGPData();
  if (bnoData.valid) {
    Serial.print("Quat I: "); Serial.print(bnoData.i, 2); Serial.print("\t");
    Serial.print("J: "); Serial.print(bnoData.j, 2); Serial.print("\t");
    Serial.print("K: "); Serial.print(bnoData.k, 2); Serial.print("\t");
    Serial.print("Real: "); Serial.print(bnoData.real, 2); Serial.print("\t");
    Serial.print("Accuracy: "); Serial.println(bnoData.accuracy, 2);
  } else {
    Serial.println("BNO08x data invalid");
  }
  if (sgpData.valid) {
    Serial.print("TVOC: "); Serial.print(sgpData.TVOC); Serial.print(" ppb\t");
    Serial.print("eCO2: "); Serial.print(sgpData.eCO2); Serial.println(" ppm");
    Serial.print("Raw H2: "); Serial.print(sgpData.rawH2); Serial.print("\t");
    Serial.print("Raw Ethanol: "); Serial.println(sgpData.rawEthanol);
  } else {
    Serial.println("SGP30 data invalid");
  }
}