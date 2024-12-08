// #include <Arduino.h>
// #if defined(ESP32)
//   #include <WiFi.h>
// #elif defined(ESP8266)
//   #include <ESP8266WiFi.h>
// #endif
// #include <Firebase_ESP_Client.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <time.h> // Library time.h untuk NTP

// // Provide the token generation process info.
// #include "addons/TokenHelper.h"
// // Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// // Insert your network credentials
// #define WIFI_SSID "Rachmat-Wifi"
// #define WIFI_PASSWORD "Delphin00@@"

// // Insert Firebase project API Key
// #define API_KEY "AIzaSyDqe5n_h7Aep6ThjhwTy_6GtyzB2vyPfiU"

// // Insert RTDB URL
// #define DATABASE_URL "https://team7-e5280-default-rtdb.firebaseio.com" 

// // Define Firebase Data object
// FirebaseData fbdo;

// FirebaseAuth auth;
// FirebaseConfig config;

// unsigned long sendDataPrevMillis = 0;
// bool signupOK = false;

// // Sensor DS18B20 setup
// #define ONE_WIRE_BUS 4  // Pin untuk DS18B20
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensors(&oneWire);

// // LCD setup
// LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C LCD (biasanya 0x27 atau 0x3F)

// // NTP server settings
// const char* ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 7 * 3600; // GMT+7 untuk WIB
// const int daylightOffset_sec = 0;

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println();
//   Serial.print("Connected with IP: ");
//   Serial.println(WiFi.localIP());

//   // Configure time using NTP
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//   Serial.println("Time synchronized");

//   /* Assign the api key (required) */
//   config.api_key = API_KEY;

//   /* Assign the RTDB URL (required) */
//   config.database_url = DATABASE_URL;

//   /* Sign up */
//   if (Firebase.signUp(&config, &auth, "", "")) {
//     Serial.println("ok");
//     signupOK = true;
//   } else {
//     Serial.printf("%s\n", config.signer.signupError.message.c_str());
//   }

//   /* Assign the callback function for the long running token generation task */
//   config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
//   Firebase.begin(&config, &auth);
//   Firebase.reconnectWiFi(true);

//   // LCD setup
//   lcd.init();
//   lcd.backlight();
//   lcd.setCursor(0, 0);
//   lcd.print("Initializing...");
//   lcd.clear();
//   lcd.setCursor(0, 1);
//   lcd.print("TEAM 7");
//   lcd.print("IoT monitoring water Quality");
  
//   // DS18B20 setup
//   sensors.begin();
// }

// void loop() {
//   // Read temperature from DS18B20
//   sensors.requestTemperatures();
//   float temperature = sensors.getTempCByIndex(0); // Get the first sensor's temperature

//   // Get current time
//   time_t now = time(nullptr);
//   struct tm* timeinfo = localtime(&now);

//   // Format date and time
//   String currentHour = String(timeinfo->tm_hour) + ":" + String(timeinfo->tm_min);
//   String currentDate = String(1900 + timeinfo->tm_year) + "-" + String(1 + timeinfo->tm_mon) + "-" + String(timeinfo->tm_mday);

//   //--- Display temperature on LCD
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Suhu: ");
//   lcd.print(temperature);
//   lcd.print(" C   ");
  
//   // Display on Serial Monitor
//   Serial.print("Temperature: ");
//   Serial.print(temperature);
//   Serial.println(" °C");

//   // Firebase structure: Set temperature and turbidity with date and hour
//   if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
//     sendDataPrevMillis = millis();

//     // Write temperature to Firebase under the current date and time
//     String temperaturePath = "/akuarium-mu/temperature/" + currentDate + "/" + currentHour;
//     if (Firebase.RTDB.setFloat(&fbdo, temperaturePath.c_str(), temperature)) {
//       Serial.println("Temperature sent to Firebase");
//     } else {
//       Serial.println("Failed to send temperature");
//     }

//     // Write real-time temperature to Firebase
//     if (Firebase.RTDB.setFloat(&fbdo, "/akuarium-mu/temperature/realtime_temperature", temperature)) {
//       Serial.println("Real-time temperature sent to Firebase");
//     } else {
//       Serial.println("Failed to send real-time temperature");
//     }

//     delay(5000); // Delay for 5 seconds before sending data again
//   }
// }
