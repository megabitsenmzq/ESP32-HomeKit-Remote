#include "Remote.h"
#include "Wire.h"

PANASONIC_REMOTE *remote;

// Timer
unsigned long previousSensorMillis = 0;
const long sensorInterval = 10000;

void setup() {
  setCpuFrequencyMhz(120);
  Serial.begin(115200);

  // Sensor
  Wire.begin(26, 32);

  // Remote
  ac.begin();
  ac.setModel(kPanasonicRkr);

  // HomeSpan
  homeSpan.begin(Category::AirConditioners, "Panasonic Remote");
  homeSpan.enableWebLog(20,"pool.ntp.org","UTC-9"); // Timezone: JST
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  remote = new PANASONIC_REMOTE();
}

void loop() {
  // Timer
  unsigned long currentMillis = millis();

  // Update the sensor value every 10 seconds.
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;
    remote->updateSensor();
  }

  // Keep this at the bottom.
  homeSpan.poll(); 
}