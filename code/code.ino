#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <Wire.h>       // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal I2C library

// Replace with your network credentials
const char* ssid = "Redmi12";
const char* password = "12345678";

// DHT sensor settings
#define DHTPIN D4       // DHT data pin (e.g., D2)
#define DHTTYPE DHT11  // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);

// LDR sensor settings
const int ldrPin = A0;  // LDR sensor connected to analog pin A0

// Rain sensor settings
const int rainSensorPin = D3;  // Rain sensor connected to digital pin D1

// ThingSpeak settings
const char* server = "api.thingspeak.com";
const char* writeAPIKey = "WGFI8LV88T39KIWY";

WiFiClient client;

// LCD display settings
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Change the I2C address and display size if needed

void setup() {
  Serial.begin(9600);
  delay(10);

  // Initialize the LCD display
  lcd.init();
  lcd.backlight();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  dht.begin();
  ThingSpeak.begin(client);
}

void loop() {
  // Read data from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read data from the LDR sensor
  int ldrValue = analogRead(ldrPin);

  // Read data from the rain sensor
  int rainValue = digitalRead(rainSensorPin);

  // Print data to the serial monitor
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  Serial.print("Rain State: ");
  Serial.println(rainValue == HIGH ? "No Rain" : "Rain Detected");

  // Display data on the LCD
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.setCursor(8, 1);
  lcd.print("Rain:");
  lcd.print(rainValue == HIGH ? "No" : "Yes");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, ldrValue);
  ThingSpeak.setField(4, rainValue);

  int status = ThingSpeak.writeFields(2336604, writeAPIKey);

  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.println("Failed to send data to ThingSpeak.");
  }

  delay(5000); // Delay for 5 seconds before reading the sensors again
}

