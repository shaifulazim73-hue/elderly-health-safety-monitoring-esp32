/*
 * Elderly Health & Safety Monitoring System
 * ESP32 Final Year Project
 *
 * Hardware:
 * - ESP32
 * - DS18B20 temperature sensor
 * - MPU6050 accelerometer/gyroscope
 * - OLED I2C display
 * - Buzzer
 *
 * Cloud:
 * - Wi-Fi
 * - ThingSpeak
 */

#include <WiFi.h>
#include <HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// =====================================================
// Wi-Fi CONFIGURATION
// =====================================================

const char* ssid = "DummyWifiName";
const char* password = "DummyWifiPass";


// =====================================================
// THINGSPEAK CONFIGURATION
// =====================================================

const char* thingSpeakServer = "http://api.thingspeak.com/update";

String apiKey = "XYZ1234567890";


// =====================================================
// PIN CONFIGURATION
// =====================================================

// DS18B20
#define ONE_WIRE_BUS 4

// Buzzer
#define BUZZER_PIN 25


// =====================================================
// OLED CONFIGURATION
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// =====================================================
// SENSOR OBJECTS
// =====================================================

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);

Adafruit_MPU6050 mpu;


// =====================================================
// SYSTEM VARIABLES
// =====================================================

float temperature = 0.0;

float accelerationX = 0.0;
float accelerationY = 0.0;
float accelerationZ = 0.0;

float accelerationMagnitude = 0.0;


// Temperature safety threshold
const float HIGH_TEMP_THRESHOLD = 38.0;


// Fall detection threshold
// NOTE: This value needs to be replaced with the
// actual threshold from the original December code
const float FALL_THRESHOLD = 2.5;


// System status
String systemStatus = "NORMAL";


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ---------------------------------------------------
  // Buzzer
  // ---------------------------------------------------

  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);


  // ---------------------------------------------------
  // DS18B20
  // ---------------------------------------------------

  temperatureSensor.begin();


  // ---------------------------------------------------
  // I2C
  // ---------------------------------------------------

  Wire.begin();


  // ---------------------------------------------------
  // MPU6050
  // ---------------------------------------------------

  if (!mpu.begin())
  {
    Serial.println("MPU6050 not detected!");

    while (1)
    {
      delay(1000);
    }
  }

  Serial.println("MPU6050 connected.");


  // ---------------------------------------------------
  // OLED
  // ---------------------------------------------------

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("OLED initialization failed!");

    while (1)
    {
      delay(1000);
    }
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Elderly Monitor");

  display.println();

  display.println("Initializing...");

  display.display();

  delay(2000);


  // ---------------------------------------------------
  // Wi-Fi
  // ---------------------------------------------------

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wi-Fi connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  // ---------------------------------------------------
  // Ready
  // ---------------------------------------------------

  display.clearDisplay();

  display.setCursor(0, 0);

  display.println("System Ready");

  display.println();

  display.println("Monitoring...");

  display.display();

  delay(1500);
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ---------------------------------------------------
  // READ TEMPERATURE
  // ---------------------------------------------------

  temperatureSensor.requestTemperatures();

  temperature =
    temperatureSensor.getTempCByIndex(0);


  // ---------------------------------------------------
  // READ MPU6050
  // ---------------------------------------------------

  sensors_event_t acceleration;
  sensors_event_t gyro;
  sensors_event_t temperatureMPU;

  mpu.getEvent(
    &acceleration,
    &gyro,
    &temperatureMPU
  );


  accelerationX = acceleration.acceleration.x;

  accelerationY = acceleration.acceleration.y;

  accelerationZ = acceleration.acceleration.z;


  // ---------------------------------------------------
  // CALCULATE ACCELERATION MAGNITUDE
  // ---------------------------------------------------

  accelerationMagnitude =
    sqrt(
      accelerationX * accelerationX +
      accelerationY * accelerationY +
      accelerationZ * accelerationZ
    );


  // ---------------------------------------------------
  // SERIAL MONITOR
  // ---------------------------------------------------

  Serial.println("-----------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Acceleration X: ");
  Serial.println(accelerationX);

  Serial.print("Acceleration Y: ");
  Serial.println(accelerationY);

  Serial.print("Acceleration Z: ");
  Serial.println(accelerationZ);

  Serial.print("Acceleration Magnitude: ");
  Serial.println(accelerationMagnitude);


  // ---------------------------------------------------
  // DETECTION
  // ---------------------------------------------------

  bool abnormalTemperature = false;
  bool fallDetected = false;


  // Temperature detection

  if (temperature >= HIGH_TEMP_THRESHOLD)
  {
    abnormalTemperature = true;

    Serial.println("WARNING: High temperature!");
  }


  // Fall detection

  if (accelerationMagnitude >= FALL_THRESHOLD)
  {
    fallDetected = true;

    Serial.println("WARNING: Possible fall detected!");
  }


  // ---------------------------------------------------
  // SYSTEM STATUS
  // ---------------------------------------------------

  if (fallDetected && abnormalTemperature)
  {
    systemStatus = "FALL + HIGH TEMP";
  }

  else if (fallDetected)
  {
    systemStatus = "FALL DETECTED";
  }

  else if (abnormalTemperature)
  {
    systemStatus = "HIGH TEMP";
  }

  else
  {
    systemStatus = "NORMAL";
  }


  Serial.print("System Status: ");
  Serial.println(systemStatus);


  // ---------------------------------------------------
  // BUZZER
  // ---------------------------------------------------

  if (fallDetected || abnormalTemperature)
  {
    digitalWrite(BUZZER_PIN, HIGH);

    delay(500);

    digitalWrite(BUZZER_PIN, LOW);
  }

  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }


  // ---------------------------------------------------
  // OLED DISPLAY
  // ---------------------------------------------------

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.println("Elderly Monitor");

  display.println();

  display.print("Temp: ");

  display.print(temperature, 2);

  display.println(" C");

  display.println();

  display.print("Status:");

  display.setCursor(0, 48);

  display.println(systemStatus);

  display.display();


  // ---------------------------------------------------
  // SEND DATA TO THINGSPEAK
  // ---------------------------------------------------

  sendToThingSpeak(
    temperature,
    accelerationMagnitude,
    systemStatus
  );


  // ---------------------------------------------------
  // MONITORING INTERVAL
  // ---------------------------------------------------

  delay(15000);
}


// =====================================================
// THINGSPEAK FUNCTION
// =====================================================

void sendToThingSpeak(
  float temp,
  float acceleration,
  String status
)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi disconnected.");

    return;
  }


  HTTPClient http;


  String url =
    String(thingSpeakServer) +
    "?api_key=" +
    apiKey +
    "&field1=" +
    String(temp) +
    "&field2=" +
    String(acceleration);


  http.begin(url);


  int httpResponseCode =
    http.GET();


  if (httpResponseCode > 0)
  {
    Serial.print("ThingSpeak response: ");

    Serial.println(httpResponseCode);
  }

  else
  {
    Serial.print("ThingSpeak error: ");

    Serial.println(httpResponseCode);
  }


  http.end();
}