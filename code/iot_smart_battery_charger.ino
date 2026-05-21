#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "IOT SMART CHARGER"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Wi-Fi credentials
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// Sensor and control pins
const int ACS_pin = 34;
const int voltagePin = 35;
const int relayPin = 26;
const int buzzerPin = 27;

// System calibration parameters
const float Vcc = 3.3;
const float Vref = 2.386;
const float sensitivity = 0.0104;
const float dividerRatio = 5.29;
const float fullChargeVoltage = 14.4;

// System status variables
bool batteryFull = false;
bool alarmTriggered = false;
bool systemOn = true;
bool blynkConnected = false;

// Remote ON/OFF control from Blynk dashboard
BLYNK_WRITE(V3) {
  systemOn = param.asInt();
}

// Attempt Blynk connection with timeout protection
void tryBlynkConnection(unsigned long timeoutMs) {

  unsigned long start = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");

  while (!Blynk.connected() && millis() - start < timeoutMs) {
    Blynk.run();
    delay(100);
  }

  lcd.clear();

  if (Blynk.connected()) {

    blynkConnected = true;

    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");

    delay(1500);

    lcd.clear();

    Serial.println("Blynk connected successfully");

  } else {

    blynkConnected = false;

    lcd.setCursor(0, 0);
    lcd.print("WiFi Not Found");

    lcd.setCursor(0, 1);
    lcd.print("Offline Mode");

    delay(2500);

    lcd.clear();

    Serial.println("Blynk connection failed. Running offline.");
  }
}

void setup() {

  Serial.begin(115200);

  // Configure ESP32 ADC resolution
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Configure output devices
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(relayPin, HIGH);
  digitalWrite(buzzerPin, LOW);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();

  // System splash screen
  lcd.setCursor(1, 0);
  lcd.print("IOT SMART");

  lcd.setCursor(2, 1);
  lcd.print("CHARGER");

  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("DESIGNED BY:");

  lcd.setCursor(1, 1);
  lcd.print("IBE ANGEL");

  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SUPERVISED BY:");

  lcd.setCursor(0, 1);
  lcd.print("DR. C.U EYA");

  delay(2500);
  lcd.clear();

  // Initialize Wi-Fi and Blynk
  WiFi.begin(ssid, pass);

  Blynk.config(BLYNK_AUTH_TOKEN);

  tryBlynkConnection(20000);
}

void loop() {

  // Maintain Blynk communication
  if (blynkConnected)
    Blynk.run();

  // System shutdown logic
  if (!systemOn) {

    digitalWrite(relayPin, LOW);

    lcd.noBacklight();

    lcd.clear();

    return;
  }

  lcd.backlight();

  // ----------------------------------------------------
  // Acquire and average ACS712 current sensor readings
  // to improve measurement stability and reduce ADC noise
  // ----------------------------------------------------

  long sumCurrent = 0;

  for (int i = 0; i < 100; i++) {

    sumCurrent += analogRead(ACS_pin);

    delay(1);
  }

  float currentVoltage =
      (sumCurrent / 100.0 / 4095.0) * Vcc;

  float current =
      (currentVoltage - Vref) / sensitivity;

  // Remove minor sensor fluctuations
  if (abs(current) < 0.05)
    current = 0.0;

  // ----------------------------------------------------
  // Acquire battery voltage measurements
  // ----------------------------------------------------

  long sumVoltage = 0;

  for (int i = 0; i < 100; i++) {

    sumVoltage += analogRead(voltagePin);

    delay(1);
  }

  float voltageADC =
      (sumVoltage / 100.0 / 4095.0) * Vcc;

  float batteryVoltage =
      voltageADC * dividerRatio;

  // Battery percentage estimation
  int batteryPercent =
      map(batteryVoltage * 100, 1190, 1270, 0, 100);

  batteryPercent =
      constrain(batteryPercent, 0, 100);

  // ----------------------------------------------------
  // Transmit real-time parameters to Blynk IoT platform
  // ----------------------------------------------------

  if (blynkConnected) {

    Blynk.virtualWrite(V0, batteryVoltage);

    Blynk.virtualWrite(V1, current);

    Blynk.virtualWrite(V2, batteryPercent);
  }

  // ----------------------------------------------------
  // Intelligent charging protection logic:
  // disconnect charging when battery reaches full-charge
  // threshold to prevent overcharging
  // ----------------------------------------------------

  if (batteryVoltage >= fullChargeVoltage) {

    if (!batteryFull) {

      batteryFull = true;

      alarmTriggered = false;
    }

    // Disconnect charger
    digitalWrite(relayPin, LOW);

    // Trigger buzzer alert once
    if (!alarmTriggered) {

      digitalWrite(buzzerPin, HIGH);

      delay(2000);

      digitalWrite(buzzerPin, LOW);

      alarmTriggered = true;
    }

    lcd.clear();

    lcd.setCursor(1, 0);
    lcd.print("BATTERY FULL");

    lcd.setCursor(0, 1);
    lcd.print("CHARGING CUT OFF");

    delay(3000);

    lcd.clear();

  } else {

    batteryFull = false;

    alarmTriggered = false;

    digitalWrite(relayPin, HIGH);

    digitalWrite(buzzerPin, LOW);
  }

  // ----------------------------------------------------
  // LCD real-time monitoring display
  // ----------------------------------------------------

  lcd.setCursor(0, 0);

  lcd.print("I:");
  lcd.print(current, 1);
  lcd.print("A ");

  lcd.setCursor(8, 0);

  lcd.print("V:");
  lcd.print(batteryVoltage, 1);
  lcd.print("V ");

  lcd.setCursor(0, 1);

  lcd.print("Battery: ");
  lcd.print(batteryPercent);
  lcd.print("%   ");

  // Serial debugging output
  Serial.print("Current: ");
  Serial.print(current);

  Serial.print(" A | Voltage: ");
  Serial.print(batteryVoltage);

  Serial.print(" V | Battery: ");
  Serial.print(batteryPercent);

  Serial.println("%");

  delay(1000);
}
