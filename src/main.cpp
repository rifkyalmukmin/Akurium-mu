#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h> // Library untuk waktu

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Rachmat-Wifi"
#define WIFI_PASSWORD "Delphin00@@"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDqe5n_h7Aep6ThjhwTy_6GtyzB2vyPfiU"

// Insert RTDB URL
#define DATABASE_URL "https://team7-e5280-default-rtdb.firebaseio.com"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// Sensor DS18B20 setup
#define ONE_WIRE_BUS 4 // Pin untuk DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// TDS Sensor setup
#define TDS_PIN 39      // Pin ADC untuk TDS sensor
const float VREF = 3.3; // Tegangan referensi ESP32
const int RESOLUTION = 4096;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C LCD (biasanya 0x27 atau 0x3F)

// Fungsi untuk sinkronisasi waktu melalui NTP
void initTime()
{
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC+7 untuk WIB
    Serial.print("Waiting for NTP time sync...");
    while (time(nullptr) < 24 * 3600)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nTime synchronized");
}

// Fungsi untuk mendapatkan waktu dan tanggal saat ini
String getCurrentTime()
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%H:%M", timeinfo);
    return String(buffer);
}

String getCurrentDate()
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    char buffer[11]; // Format YYYY-MM-DD
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return String(buffer);
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // Initialize time
    initTime();

    // Assign the api key (required)
    config.api_key = API_KEY;

    // Assign the RTDB URL (required)
    config.database_url = DATABASE_URL;

    // Sign up
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("Firebase signup successful");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    // Assign the callback function for the long running token generation task
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // LCD setup
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("TEAM 7 IoT");

    // DS18B20 setup
    sensors.begin();
}

void loop()
{
    // Read temperature from DS18B20
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0); // Mendapatkan suhu pertama

    // Read TDS value
    int tdsValue = analogRead(TDS_PIN);
    float voltage = tdsValue * (VREF / RESOLUTION);
    float tds = (voltage / VREF) * 1000; // Menghitung TDS (dalam ppm)

    // Display data on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Suhu: ");
    lcd.print(temperature);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("TDS: ");
    lcd.print(tds);
    lcd.print(" Ppm");

    // Print data to Serial Monitor
    Serial.print("Suhu: ");
    Serial.print(temperature);
    Serial.print(" C, TDS: ");
    Serial.print(tds);
    Serial.println(" Ppm");

    // Send data to Firebase
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        String currentDate = getCurrentDate(); // Mendapatkan tanggal saat ini
        String currentTime = getCurrentTime(); // Mendapatkan waktu saat ini (HH:MM)

        // Write temperature to Firebase under the current date and time
        String temperaturePath = "/akuarium-mu/temperature/" + currentDate + "/" + currentTime;
        if (Firebase.RTDB.setFloat(&fbdo, temperaturePath.c_str(), temperature))
        {
            Serial.println("Temperature sent to Firebase");
        }
        else
        {
            Serial.println("Failed to send temperature");
        }

        // Write real-time temperature to Firebase
        if (Firebase.RTDB.setFloat(&fbdo, "/akuarium-mu/temperature/realtime_temperature", temperature))
        {
            Serial.println("Real-time temperature sent to Firebase");
        }
        else
        {
            Serial.println("Failed to send real-time temperature");
        }

        // Write turbidity to Firebase under the current date and time
        String tdsPath = "/akuarium-mu/turbidity/" + currentDate + "/" + currentTime;
        if (Firebase.RTDB.setFloat(&fbdo, tdsPath.c_str(), tds))
        {
            Serial.println("TDS sent to Firebase");
        }
        else
        {
            Serial.println("Failed to send TDS");
        }

        // Write real-time turbidity to Firebase
        if (Firebase.RTDB.setFloat(&fbdo, "/akuarium-mu/turbidity/realtime_turbidity", tds))
        {
            Serial.println("Real-time TDS sent to Firebase");
        }
        else
        {
            Serial.println("Failed to send real-time TDS");
        }

        delay(5000); // Delay for 5 seconds before sending data again
    }

    delay(1000);
}
