#include "HomeSpan.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#include "SHTSensor.h"
#include "EEPROM.h"

// Remote
const uint16_t kIrLed = 25;
IRPanasonicAc ac(kIrLed);

// Senser
SHTSensor sht;

EEPROMClass ACState("acState");
EEPROMClass IRACSwitch("IRACSwitch");
EEPROMClass IRACState("IRACState");
EEPROMClass ACTemp("acTemp");

struct AC_Service: Service::Thermostat {

  SpanCharacteristic *currentState;
  SpanCharacteristic *targetState;
  SpanCharacteristic *currentTemp;
  SpanCharacteristic *currentHumidity;
  SpanCharacteristic *targetTemp;
  SpanCharacteristic *unit;

  // Extra characteristic for automation.
  SpanCharacteristic *currentTemp2;

  int previousACState = 0;

  AC_Service()
    : Service::Thermostat() {

    sht.init();
    float temperature = sht.getTemperature();
    float humidity = sht.getHumidity();

    // Read from EEPROM
    int savedState = 0;
    float savedTemp = 0;

    // EEPROM Initialization.
    if (!ACState.begin(0x4)) {
      Serial.println("Failed to initialise ACState");
      delay(1000);
      ESP.restart();
    }
    if (!IRACSwitch.begin(0x4)) {
      Serial.println("Failed to initialise IRACSwitch");
      delay(1000);
      ESP.restart();
    }
    if (!IRACState.begin(0x4)) {
      Serial.println("Failed to initialise IRACState");
      delay(1000);
      ESP.restart();
    }
    if (!ACTemp.begin(0x4)) {
      Serial.println("Failed to initialise ACState");
      delay(1000);
      ESP.restart();
    }

    ACState.get(0, savedState);
    ACTemp.get(0, savedTemp);

    if (savedState == -1) {
      savedState = 0;
    }
    if (isnan(savedTemp)) {
      savedTemp = 22;
    }

    new Characteristic::Name("Panasonic Remote");
    currentState = new Characteristic::CurrentHeatingCoolingState(0);
    targetState = new Characteristic::TargetHeatingCoolingState(savedState);
    currentTemp = new Characteristic::CurrentTemperature(temperature);
    currentHumidity = new Characteristic::CurrentRelativeHumidity(humidity);
    targetTemp = (new Characteristic::TargetTemperature(savedTemp))->setRange(16, 30, 0.5);
    unit = new Characteristic::TemperatureDisplayUnits(0);

    // Extra temperature sensor for automation.
    new Service::TemperatureSensor();
    new Characteristic::Name("Sensor for Automation");
    currentTemp2 = new Characteristic::CurrentTemperature(temperature);
  }

  void printState() {
    Serial.println("Panasonic A/C remote is in the following state:");
    Serial.printf("  %s\n", ac.toString().c_str());
    WEBLOG("AC Stat  %s\n", ac.toString().c_str());
    // Display the encoded IR sequence.
    unsigned char *ir_code = ac.getRaw();
    Serial.print("IR Code: 0x");
    for (uint8_t i = 0; i < kPanasonicAcStateLength; i++)
      Serial.printf("%02X", ir_code[i]);
    Serial.println();
  }

  void updateSensor() {
    float temperature = sht.getTemperature();
    float humidity = sht.getHumidity();
    currentTemp->setVal(temperature);
    currentHumidity->setVal(humidity);

    currentTemp2->setVal(temperature);
  }

  void toggleAC() {
    int state = targetState->getNewVal();
    if (state != 0) {
      previousACState = state;
      targetState->setVal(0);
    } else {
      targetState->setVal(previousACState);
    }
    update();
  }

  // Send IR signal based on the target state.
  boolean update() {
    int state = targetState->getNewVal();
    float temp = targetTemp->getNewVal();

    // Save to EEPROM.
    ACState.put(0, state);
    ACState.commit();
    ACTemp.put(0, temp);
    ACTemp.commit();
    
    if (state == 0) {
      IRACSwitch.put(0, 0);
      ac.off();
    } else {
      IRACSwitch.put(0, 1);
      ac.on();
      previousACState = state;
    }

    switch (state) {
      case 0:
        break;
      case 1:
        state = kPanasonicAcHeat;
        currentState->setVal(state);
        break;
      case 2:
        state = kPanasonicAcCool;
        currentState->setVal(state);
        break;
      default:
        state = kPanasonicAcAuto;
        // Setting the current state to auto cause the device to stop responding.
        currentState->setVal(0);
        break;
    }

    ac.setMode(state);
    IRACState.put(0, state);

    ac.setFan(kPanasonicAcFanAuto);
    ac.setSwingVertical(kPanasonicAcSwingVAuto);
    ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
    ac.setTemp(temp);
    ac.send();
    printState();
    return (true);
  }
};