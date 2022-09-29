#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <ArduinoJson.h>
#include "yeelight.h"
#include <ESP8266HTTPClient.h>
// #include <TimeLib.h>
#include <Time.h>
#include <TimeAlarms.h>
#include "DHTesp.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define DHTTYPE DHT11
#define LEFT 1
#define MIDDLE 2
#define RIGHT 3
#define SCREEN_PIN 8
#define BACKLIGHT_TIME 10 * 1000
#define MENU_COUNT 3
#define TEMPERATURE_INTERVAL 10 * 1000
#define TEMPERATURE_INTERVAL_SEND 5 * 60 * 1000
#define DATETIME_UPDATE_INTERVAL 60 * 60 * 1000
#define POWER_UPDATE_INTERVAL 2000
#define ALARM_UPDATE_INTERVAL 1 * 60 * 1000
#define BUTTON_INTERVAL 250
#define BUTTONS A0
#define DHT_PIN D6
#define BULB_DELAY_REQUEST 200

DHTesp dht;
const int rs = D0, en = D1, d4 = D5, d5 = D4, d6 = D3, d7 = D2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
float HUMIDITY, TEMPERATURE;
int MENU = 1;
bool EDIT_MODE = false;
bool BRIGHTNESS = true;
char buff[16];
unsigned long LAST_CLICKED = millis();
unsigned long LAST_TEMPERATURE_UPDATE = 0;
unsigned long LAST_TEMPERATURE_SEND = 0;
unsigned long LAST_DATETIME_UPDATE = millis();
unsigned long LAST_ALARMS_UPDATE = millis();
unsigned long LAST_SETTINGS_SEND = millis();
unsigned long LAST_POWER_UPDATE = millis();
String last_alarm_datetime = "";
AlarmId alarmIds[7];
bool POWER = false;
unsigned int COLOR_WAKE_UP = 16775041;
unsigned int COLOR = 16777215;


StaticJsonDocument<200> jsonBuffer;
Yeelight* yeelight;

byte proggressChar[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte temperatureChar[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B01110
};

byte humidityChar[] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B01010,
  B00100
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  connectToWiFi("Marta_NET", "mIsIo412pysIo");

  yeelight = new Yeelight();
  yeelight->lookup();

  lcd.begin(16, 2);
  lcd.createChar(1, proggressChar);
  lcd.createChar(2, temperatureChar);
  lcd.createChar(3, humidityChar);

  updateDatetime();
  dht.setup(DHT_PIN, DHTesp::DHT11);
  pinMode(BUTTONS, INPUT);
  // pinMode(SCREEN_PIN, OUTPUT);
  updateAlarms();
  sendSettings();
}



void loop() {
  if (MENU == 4) {
    if (buttonClicked(RIGHT) || buttonClicked(LEFT) || buttonClicked(MIDDLE)) {
      Serial.println("Disabling alarm.");
      setBulbColor(COLOR);
      MENU = 1;
    }
  }

  // buttonClicked(MIDDLE);
  if (!EDIT_MODE && buttonClicked(RIGHT)) MENU = navigateMenu(1, MENU);
  if (!EDIT_MODE && buttonClicked(LEFT)) MENU = navigateMenu(-1, MENU);

  if (MENU == 1) {
    mainScreen();
    // Serial.println(buttonClicked(MIDDLE) == true);
    if (buttonClicked(MIDDLE)) {
      Serial.print("Turning POWER to: ");
      Serial.println(!POWER);
      setBulbPower(!POWER);
      // POWER = !POWER;
    }
  } else if (MENU == 2) setTimeScreen();
  else if (MENU == 3) setAlarmScreen();

  if ((LAST_CLICKED + BACKLIGHT_TIME) < millis()) BRIGHTNESS = false;
  if ((LAST_TEMPERATURE_UPDATE + TEMPERATURE_INTERVAL) < millis()) updateTemperature();
  if ((LAST_TEMPERATURE_SEND + TEMPERATURE_INTERVAL_SEND) < millis()) sendTemperature();
  if ((LAST_DATETIME_UPDATE + DATETIME_UPDATE_INTERVAL) < millis()) updateDatetime();
  if ((LAST_ALARMS_UPDATE + ALARM_UPDATE_INTERVAL) < millis()) updateAlarms();
  if ((LAST_POWER_UPDATE + POWER_UPDATE_INTERVAL) < millis()) updatePower();

  Alarm.delay(50);
}

void connectToWiFi(const char* ssid, const char* pwd) {
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void updateTemperature() {
  HUMIDITY = int(dht.getHumidity() * 8 / 10);
  TEMPERATURE = dht.getTemperature() - 1;
  // float hic = dht.computeHeatIndex(temperature, humidity, false);   // Compute heat index in Celsius (isFahreheit = false)
  if (isnan(HUMIDITY)) HUMIDITY = 0;
  if (isnan(TEMPERATURE)) TEMPERATURE = 0;
  LAST_TEMPERATURE_UPDATE = millis();
}

void sendTemperature() {
  String serverPath = "http://clock.panjacob.online/set_temp.php?temp=" + String(TEMPERATURE) + "&hum=" + String(HUMIDITY);
  sendGET(serverPath);
  LAST_TEMPERATURE_SEND = millis();
}

void sendSettings() {
  String serverPath = "http://clock.panjacob.online/set_settings.php?power=" + String(POWER) + "&color=" + String(COLOR);
  sendGET(serverPath);
  LAST_SETTINGS_SEND = millis();
}



String sendGET(String serverPath) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverPath.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();
      // Serial.println(payload);
      return payload;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  return "error";
}


void mainScreen() {
  sprintf(buff, "%02d-%02d-%04d %02d:%02d", day(), month(), year(), hour(), minute());
  lcd.setCursor(0, 0);
  lcd.print(buff);

  lcd.setCursor(0, 1);
  lcd.write(3);
  sprintf(buff, ":%02d%% ", int(HUMIDITY));
  lcd.print(buff);

  lcd.write(2);
  sprintf(buff, ":%02dC", int(TEMPERATURE));
  lcd.print(buff);
}

bool buttonClicked(int name) {
  if (!((LAST_CLICKED + BUTTON_INTERVAL) < millis())) return false;

  if (analogRead(BUTTONS) > 750 && name == LEFT) {
    lcd.clear();
    LAST_CLICKED = millis();
    BRIGHTNESS = true;
    return true;
  }
  if (analogRead(BUTTONS) > 590 && analogRead(BUTTONS) < 750 && name == MIDDLE) {
    lcd.clear();
    LAST_CLICKED = millis();
    BRIGHTNESS = true;
    return true;
  }
  if (analogRead(BUTTONS) > 100 && analogRead(BUTTONS) < 590 && name == RIGHT) {
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

// void initializeDate2() {
//     int h, m, s, month, day, year;
//     static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
//     sscanf(__DATE__, "%s %d %d", buff, &day, &year);
//     month = (strstr(month_names, buff)-month_names)/3+1;
//     sscanf(__TIME__, "%d:%d:%d", &h, &m, &s);
//     setTime(h,m,s,day,month,year);
// }

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
  Serial.println("Checking time: ");
  String dateStr = sendGET("http://clock.panjacob.online/get_settings_client.php");
  bool power = dateStr.substring(0,1).toInt(); 
  int color = dateStr.substring(2).toInt();
  Serial.print("Color: ");
  Serial.println(color);
  if (power != POWER) setBulbPower(power);
  if (color != COLOR) setBulbColor(color);
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
  MENU = 4;
  lcd.clear();
  sprintf(buff, "%d: Pobudka!!", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  setBulbColor(COLOR_WAKE_UP);
  setBulbPower(true);
  Serial.println("TEST TEST TEST ALARM!!!");
}

void setBulbPower(bool isPower) {
  String power = isPower ? "on" : "off";
  Serial.println("Turn " + power + "bulb.");
  for (int i = 0; i < 100; i++) {
    sendCommandX("set_power", "[\"" + power + "\", \"smooth\", 500]");
    deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"power\"]"));
    JsonObject root = jsonBuffer.as<JsonObject>();
    String state = root["result"][0];
    Serial.printf("Power is: %s\n", state);

    if (state == power) {
      Serial.println("Light is on, exiting");
      break;
    }
    Serial.println("Trying again in loop");
    Alarm.delay(BULB_DELAY_REQUEST);
  }
  POWER = isPower;
  sendSettings();
}

void setBulbColor(int color) {
  String colorStr = String(color);
  Serial.println("Turn on bulb.");
  for (int i = 0; i < 100; i++) {
    sendCommandX("set_rgb", "[" + colorStr + ", \"smooth\", 500]");
    deserializeJson(jsonBuffer, yeelight->sendCommand("get_prop", "[\"rgb\"]"));
    JsonObject root = jsonBuffer.as<JsonObject>();
    String rgb = root["result"][0];
    Serial.print("Color is: ");
    Serial.println(colorStr);

    if (rgb == colorStr) {
      Serial.println("Colors does match, exiting");
      break;
    }
    Serial.println("Trying again in loop");
    Alarm.delay(BULB_DELAY_REQUEST);
  }
  COLOR = color;
  sendSettings();
}

void sendCommandX(String command, String values) {
  do {
    Serial.println("Sending command to bulb.");
    Serial.println(yeelight->sendCommand(command, values));
  } while (yeelight->feedback());
}