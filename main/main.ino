#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <ArduinoJson.h>
#include "yeelight.h"
#include <ESP8266HTTPClient.h>
#include <Time.h>
#include "TimeAlarms.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>
#include "myWifi.hpp"
#include "myCustomChars.hpp"

#define BUTTONS A0
#define BUZZER_PIN D1
#define SDA_PIN D4
#define SDL_PIN D3
#define MOVEMENT_PIN D2
#define LEFT 1
#define MIDDLE 2
#define RIGHT 3
#define SCREEN_PIN 8
#define BACKLIGHT_TIME 30 * 1000
#define MENU_COUNT 4
#define TEMPERATURE_INTERVAL 10 * 1000
#define TEMPERATURE_INTERVAL_SEND 5 * 60 * 1000
#define DATETIME_UPDATE_INTERVAL 60 * 60 * 1000
#define POWER_UPDATE_INTERVAL 1500
#define ALARM_UPDATE_INTERVAL 1 * 60 * 1000
#define BUTTON_INTERVAL 250
#define SCREEN_REFRESH_INTERVAL 2000
#define MOVEMENT_UPDATE_INTERVAL 500
#define BULB_DELAY_REQUEST 2000

Adafruit_AM2320 am2320 = Adafruit_AM2320();
LiquidCrystal_I2C lcd(0x27, 16, 2);
float HUMIDITY, TEMPERATURE;
int MENU = 1;
bool EDIT_MODE = false;
bool BRIGHTNESS = true;
bool IS_MOVEMENT = false;
bool POWER = true;
char buff[16];
unsigned long LAST_CLICKED = millis();
unsigned long LAST_TEMPERATURE_UPDATE = 0;
unsigned long LAST_TEMPERATURE_SEND = 0;
unsigned long LAST_DATETIME_UPDATE = millis();
unsigned long LAST_ALARMS_UPDATE = millis();
unsigned long LAST_SETTINGS_SEND = millis();
unsigned long LAST_POWER_UPDATE = millis();
unsigned long LAST_SCREEN_REFRESH = 0;
unsigned long LAST_MOVEMENT_UPDATE = 0;
unsigned int COLOR_WAKE_UP = 16775041;
unsigned int COLOR = 16777215;
bool MOVEMENT = false;
String last_alarm_datetime = "";
AlarmId alarmIds[7];
struct weather {
  float temp = 0;
  float feels_like = 0;
  int pressure = 0;
  int humidity = 0;
  int uvi = 0;
  float wind = 0;
  int clouds = 0;
  String sunrise = "";
  String sunset = "";
  String desc1 = "";
  String desc2 = "";
};

weather Weather;

StaticJsonDocument<300> jsonBuffer;
Yeelight* yeelight;


void setup() {
  Wire.begin(D6, D5);  //TEMPERATURA
  Wire.begin(SDA_PIN, SDL_PIN);
  am2320.begin();
  Serial.begin(115200);
  Serial.println("Starting...");
  connectToWiFi("Marta_NET", "mIsIo412pysIo");
  updateWeather();

  yeelight = new Yeelight();
  yeelight->lookup();
  // setBulbPower(false);

  lcd.init();
  lcd.createChar(1, proggressChar);
  lcd.createChar(2, temperatureChar);
  lcd.createChar(3, humidityChar);
  lcd.createChar(4, movementTrueChar);
  lcd.createChar(5, movementFalseChar);
  lcd.createChar(6, pressureChar);
  lcd.createChar(7, windChar);
  lcd.createChar(8, cloudChar);

  pinMode(BUTTONS, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  updateDatetime();
  updateAlarms();
  sendSettings();
}

void updateWeather() {
  String dataStr = sendGET("http://clock.panjacob.online/get_weather.php");
  deserializeJson(jsonBuffer, dataStr);
  JsonObject dataJson = jsonBuffer.as<JsonObject>();
  Weather.temp = dataJson["temp"];
  Weather.feels_like = dataJson["feels_like"];
  Weather.pressure = dataJson["pressure"];
  Weather.humidity = dataJson["humidity"];
  Weather.uvi = dataJson["uvi"];
  Weather.wind = dataJson["wind"];
  Weather.clouds = dataJson["clouds"];
  Weather.sunrise = String(dataJson["sunrise"]);
  Weather.sunset = String(dataJson["sunset"]);
  Weather.sunset = String(dataJson["sunset"]);
  Weather.desc1 = String(dataJson["desc1"]);
  Weather.desc2 = String(dataJson["desc2"]);
}


void loop() {
  // // for (int i = 100; i < 2000; i += 100) {
  //   // tone(BUZZER_PIN, i);
  //   // analogWriteFreq(300);
  // for (int j = 100; j < 2000; j += 20) {
  //   tone(BUZZER_PIN, j);
  //   // analogWrite(BUZZER_PIN, j);
  //   Serial.println(j);
  //   delay(1000);
  // }
  // // }

  if (MENU == 10) {
    if (buttonClicked(RIGHT) || buttonClicked(LEFT) || buttonClicked(MIDDLE)) {
      Serial.println("Disabling alarm.");
      setBulbColor(COLOR);
      MENU = 1;
    }
  }

  if (!EDIT_MODE && buttonClicked(RIGHT)) MENU = navigateMenu(1, MENU);
  if (!EDIT_MODE && buttonClicked(LEFT)) MENU = navigateMenu(-1, MENU);
  buttonClicked(MIDDLE);

  if ((LAST_SCREEN_REFRESH + SCREEN_REFRESH_INTERVAL) < millis()) {
    LAST_SCREEN_REFRESH = millis();
    MENU = navigateMenu(1, MENU);
    if (MENU == 1) mainScreen();
    if (MENU == 2) mainScreen2();
    if (MENU == 3) mainScreen3();
    if (MENU == 4) mainScreen4();


    // else if (MENU == 2) setTimeScreen();
    // else if (MENU == 3) setAlarmScreen();
  }

  if (BRIGHTNESS) lcd.backlight();
  else lcd.noBacklight();
  if ((LAST_CLICKED + BACKLIGHT_TIME) < millis()) BRIGHTNESS = false;
  if ((LAST_TEMPERATURE_UPDATE + TEMPERATURE_INTERVAL) < millis()) updateTemperature();
  if ((LAST_TEMPERATURE_SEND + TEMPERATURE_INTERVAL_SEND) < millis()) sendTemperature();
  if ((LAST_DATETIME_UPDATE + DATETIME_UPDATE_INTERVAL) < millis()) updateDatetime();
  // if ((LAST_ALARMS_UPDATE + ALARM_UPDATE_INTERVAL) < millis()) updateAlarms();
  if ((LAST_POWER_UPDATE + POWER_UPDATE_INTERVAL) < millis()) updatePower();
  if ((LAST_MOVEMENT_UPDATE + MOVEMENT_UPDATE_INTERVAL) < millis()) updateMovement();

  Alarm.delay(100);
}

void updateMovement() {
  MOVEMENT = digitalRead(MOVEMENT_PIN);
  LAST_MOVEMENT_UPDATE = millis();
}

void connectToWiFi(const char* ssid, const char* pwd) {
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.printf("IP address: %d", WiFi.localIP());
}

void updateTemperature() {
  Wire.begin(D6, D5);
  HUMIDITY = int(am2320.readHumidity());
  delay(100);
  TEMPERATURE = int(am2320.readTemperature());
  if (isnan(HUMIDITY)) HUMIDITY = 0;
  if (isnan(TEMPERATURE)) TEMPERATURE = 0;
  LAST_TEMPERATURE_UPDATE = millis();
  Wire.begin(SDA_PIN, SDL_PIN);
}

void sendTemperature() {
  String serverPath = "http://clock.panjacob.online/set_temp.php?temp=" + String(TEMPERATURE) + "&hum=" + String(HUMIDITY);
  sendGET(serverPath);
  LAST_TEMPERATURE_SEND = millis();
}

void sendSettings() {
  String serverPath = "http://clock.panjacob.online/set_settings.php?power=" + String(POWER) + "&color=" + String(COLOR);
  sendGET(serverPath);
  // serverPath = "http://clock.panjacob.online/set_settings_client.php?power=" + String(POWER) + "&color=" + String(COLOR);
  // sendGET(serverPath);
  LAST_SETTINGS_SEND = millis();
}



void mainScreen() {
  sprintf(buff, "%02d-%02d %02d:%02d   ", day(), month(), hour(), minute());
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(buff);
  lcd.write(MOVEMENT ? 4 : 5);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.write(3);
  sprintf(buff, "%02d%% ", int(HUMIDITY));
  lcd.print(buff);

  lcd.write(2);
  sprintf(buff, "%02dC", int(TEMPERATURE));
  lcd.print(buff);

  lcd.setCursor(11, 1);
  lcd.write(6);
  lcd.print(String(Weather.pressure));
}

void mainScreen2() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.write(3);
  lcd.print(String(Weather.humidity) + "% ");

  lcd.write(2);
  lcd.print(String(Weather.temp, 1) + " ");

  lcd.setCursor(11,0);
  lcd.write(4);
  lcd.print(String(Weather.feels_like, 1) + " ");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.write(7);
  lcd.print(String(Weather.wind, 1) + " ");

  lcd.write(8);
  sprintf(buff, "%02d%%", Weather.clouds);
  lcd.print(buff);
  
  lcd.setCursor(13, 1);
  lcd.print("*" + String(Weather.uvi) + " ");
}

void mainScreen3() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("*" + Weather.sunrise + "    ");
  lcd.print("_" + Weather.sunset + " ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void mainScreen4() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(Weather.desc1);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(Weather.desc2);
}

bool buttonClicked(int name) {
  if (!((LAST_CLICKED + BUTTON_INTERVAL) < millis())) return false;
  auto buttonValue = analogRead(BUTTONS);
  // Serial.println(buttonValue);

  if (buttonValue > 750 && name == LEFT) {
    lcd.clear();
    LAST_CLICKED = millis();
    BRIGHTNESS = true;
    return true;
  }
  if (buttonValue > 590 && buttonValue < 750 && name == MIDDLE) {
    lcd.clear();
    LAST_CLICKED = millis();
    BRIGHTNESS = true;
    return true;
  }
  if (buttonValue > 100 && buttonValue < 590 && name == RIGHT) {
    lcd.clear();
    LAST_CLICKED = millis();
    BRIGHTNESS = true;
    return true;
  }

  return false;
}

int navigateMenu(int valueToAdd, int menu) {
  menu += valueToAdd;
  if (menu < 1) menu = MENU_COUNT;
  else if (menu > MENU_COUNT) menu = 1;
  return menu;
}

void setTimeScreen() {
  sprintf(buff, "%d: Set time", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  sprintf(buff, "%02d/%02d/%04d %02d:%02d", day(), month(), year(), hour(), minute());
  lcd.setCursor(0, 1);
  lcd.print(buff);
}

void setAlarmScreen() {
  sprintf(buff, "%d: Alarm 1", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);


  sprintf(buff, "%02d:  %02d:%02d", weekday(), 0, 0);
  lcd.setCursor(0, 1);
  lcd.print(buff);
}

void updateDatetime() {
  Serial.println("Checking time: ");
  String dateStr = sendGET("http://clock.panjacob.online/time.php");
  int h, m, s, month, day, year;
  char arr[20];
  dateStr.toCharArray(arr, 20),
    sscanf(arr, "%2d-%2d-%4d %2d:%2d:%2d", &day, &month, &year, &h, &m, &s);
  setTime(h, m, s, day, month, year);
  LAST_DATETIME_UPDATE = millis();
}

void updatePower() {
  String response = sendGET("http://clock.panjacob.online/get_settings_client.php");
  bool power = response.substring(0, 1).toInt();
  int color = response.substring(2).toInt();
  if (power != POWER) setBulbPower(power);
  if ((color != COLOR) && POWER) setBulbColor(color);
  LAST_POWER_UPDATE = millis();
}

void updateAlarms() {
  String request = sendGET("http://clock.panjacob.online/get_alarms.php");
  Serial.println("Aktualizacja alarmów.");
  LAST_ALARMS_UPDATE = millis();
  int i = 19;
  String current_alarm_datetime = request.substring(0, i);
  if (current_alarm_datetime == last_alarm_datetime) return;
  else last_alarm_datetime = current_alarm_datetime;

  i += 1;
  int alarmIterator = -1;
  int dayIterator = 1;
  for (int j = 0; j <= request.length(); j++) Alarm.free(alarmIds[j]);


  for (i; i <= request.length(); i += 6) {
    String timeStr = request.substring(i, i + 5);
    alarmIterator += 1;
    dayIterator += 1;
    if (dayIterator == 7 + 1) dayIterator = 1;
    if (timeStr == "NU:LL") continue;
    int hour = timeStr.substring(0, 2).toInt();
    int minute = timeStr.substring(3, 5).toInt();
    alarmIds[alarmIterator] = Alarm.alarmRepeat(static_cast<timeDayOfWeek_t>(dayIterator), hour, minute, 00, alarmScreen);  // 8:30:30 every Saturday
    Serial.printf("Budzik ustawiony na dzien: %d o %d:%d -> AlarmID: %d\n", dayIterator, hour, minute, alarmIds[alarmIterator]);
  }
}

void alarmScreen() {
  MENU = 10;
  lcd.clear();
  sprintf(buff, "%d: Pobudka!!", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  setBulbColor(COLOR_WAKE_UP);
  setBulbPower(true);
  Serial.println("TEST TEST TEST ALARM!!!");
}

// void setBulbPower(bool isPower) {
//   String power = isPower ? "on" : "off";
//   Serial.println("setBulbPower(): Turn " + power + "bulb.");
//   for (int i = 0; i < 10; i++) {
//     sendCommandX("set_power", "[\"" + power + "\", \"smooth\", 500]");
//     deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"power\"]"));
//     JsonObject root = jsonBuffer.as<JsonObject>();
//     String state = root["result"][0];
//     Serial.printf("State is: %s\n", state);

//     if (state == "null") {
//       Serial.println("Power is null.");
//       // POWER = 0;
//       // sendSettings();

//     } else if (state == power) {
//       // Serial.println("State = power, OK.");
//       POWER = isPower;
//       sendSettings();
//       break;
//     }
//     Serial.println("Trying again in loop");
//     Alarm.delay(BULB_DELAY_REQUEST);
//   }
// }

// void setBulbColor(int color) {
//   String colorStr = String(color);
//   Serial.print("Changing color to: ");
//   Serial.println(colorStr);

//   for (int i = 0; i < 10; i++) {
//     sendCommandX("set_rgb", "[" + colorStr + ", \"smooth\", 500]");
//     deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"rgb\"]"));
//     JsonObject root = jsonBuffer.as<JsonObject>();
//     String rgb = root["result"][0];
//     Serial.print("Color is: ");
//     Serial.println(rgb);

//     if (rgb == "null") {
//       Serial.println("Color is null exiting loop setBulbColor. Setting color to requested");
//       COLOR = color;
//       sendSettings();
//       break;
//     } else if (rgb == colorStr) {
//       Serial.println("Colors does match, exiting");
//       COLOR = color;
//       sendSettings();
//       break;
//     }
//     Serial.println("Trying again in loop");
//     Alarm.delay(BULB_DELAY_REQUEST);
//   }
// }

void setBulbColor(int color) {
  String colorStr = String(color);
  int i = 0;
  for (i = 0; i <= 500; i++) {
    if (yeelight->feedback()) {
      String message = "{}";
      message = yeelight->sendCommand("set_rgb", "[" + colorStr + ", \"smooth\", 500]");
      deserializeJson(jsonBuffer, message);
      JsonObject root = jsonBuffer.as<JsonObject>();
      String errorCode = root["error"]["code"];
      String errorStr = root["error"];
      Serial.print("errorCode: ");
      Serial.println(errorStr);
      if (errorCode == "null") {
        Serial.print("Wszystko ok, sprawdzam czy kolor się zgadza: ");
        Serial.println(colorStr);
      } else if (errorCode == "-1") {
        Serial.println("Za dużo zapytań czekam 5 sekund i wychodzę");
        Alarm.delay(5000);
        break;
      }

      deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"rgb\"]"));
      root = jsonBuffer.as<JsonObject>();
      String rgb = root["result"][0];
      Serial.print("- color is: ");
      Serial.println(rgb);
      Serial.println(rgb == colorStr);
      if (rgb == NULL) {
        Serial.println("Stan jest null, zostaje w petli");
      }
      if (rgb == colorStr) {
        Serial.println("Zmiana koloru pomyślnie!");
        COLOR = color;
        sendSettings();
        break;
      }
    }

    if (i % 30 == 0) {
      delete yeelight;
      yeelight = new Yeelight();
      yeelight->lookup();
    }
  }
  Serial.printf("Wychodzę z pętli po: %d\n", i);
}

// void sendCommandX(String command, String values) {
//   String response = yeelight->sendCommand(command, values);
//   Serial.print("SENDCOMMAND response: ");
//   Serial.println(response);
//   if (response == "") {
//     Serial.println("SendCommandX - response is empty, looking up");
//     Serial.println(yeelight->feedback());
//     yeelight->lookup();
//   }
// }

void setBulbPower(bool power) {
  String powerStr = power ? "on" : "off";
  Serial.print("Setting bulbpower: ");
  Serial.println(powerStr);
  int i = 0;
  for (i = 0; i <= 200; i++) {
    if (yeelight->feedback()) {
      String message = "{}";
      if (yeelight->isPowered() != power) message = yeelight->sendCommand("set_power", "[\"" + powerStr + "\", \"smooth\", 500]");

      deserializeJson(jsonBuffer, message);
      JsonObject root = jsonBuffer.as<JsonObject>();
      String errorCode = root["error"]["code"];
      String errorStr = root["error"];
      Serial.print("errorCode: ");
      Serial.println(errorStr);
      if (errorCode == "null") {
        Serial.print("Wszystko ok, sprawdzam czy stan się zgadza: ");
        Serial.println(powerStr);
      } else if (errorCode == "-1") {
        Serial.println("Za dużo zapytań o power czekam 5 sekund i wychodzę");
        Alarm.delay(5000);
        break;
      }
      Alarm.delay(5);
      deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"power\"]"));
      root = jsonBuffer.as<JsonObject>();
      String state = root["result"][0];
      Serial.print("- power is: ");
      Serial.println(state);
      Serial.println(state == powerStr);
      if (state == NULL) {
        Serial.println("Stan jest null, zostaje w petli");
      }
      if (state == powerStr) {
        Serial.println("Zmiana stanu pomyślnie!");
        POWER = power;
        sendSettings();
        break;
      }
    }

    if (i % 10 == 0) {
      delete yeelight;
      yeelight = new Yeelight();
      yeelight->lookup();
    }
  }
  // Serial.printf("Wychodzę z pętli po: %d\n", i);
}