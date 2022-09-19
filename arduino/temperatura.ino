#include <TimeLib.h>
#include <DHT.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define DHTPIN 9
#define DHTTYPE DHT11
#define START A1
#define LEFT A0
#define RIGHT A2
#define SCREEN_PIN 8
#define BACKLIGHT_TIME 10 * 1000
#define MENU_COUNT 3
#define TEMPERATURE_INTERVAL 1000

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(2,3,4,5,6,7);
float humidity, temperature;
int MENU = 1;
bool EDIT_MODE = false;
bool BRIGHTNESS = true;
char buff[16];
unsigned long last_clicked = millis();
unsigned long last_temperature_update = millis();

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

void initializeDate() { 
    int h, m, s, month, day, year;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", buff, &day, &year);
    month = (strstr(month_names, buff)-month_names)/3+1;
    sscanf(__TIME__, "%d:%d:%d", &h, &m, &s);
    setTime(h,m,s,day,month,year);
}

void setup() {
  SoftwareSerial mySensor2 (14, 2); // RX, TX

  lcd.begin(16, 2);
  lcd.createChar(1, proggressChar);
  lcd.createChar(2, temperatureChar);
  lcd.createChar(3, humidityChar);

  Serial.begin(9600);
  initializeDate(); 
  dht.begin();
  pinMode(START, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SCREEN_PIN, OUTPUT);
}

void updateTemperature(){
  humidity = int(dht.readHumidity() * 8 / 10);
  temperature = dht.readTemperature() - 1;
  // float hic = dht.computeHeatIndex(temperature, humidity, false);   // Compute heat index in Celsius (isFahreheit = false)
  if (isnan(humidity)) humidity = 0;
  if (isnan(temperature)) temperature = 0;
}


void mainScreen(){
  sprintf(buff, "%02d/%02d/%04d %02d:%02d", day(), month(), year(), hour(), minute());
  lcd.setCursor(0, 0);
  lcd.print(buff);
  
  lcd.setCursor(0, 1);
  lcd.write(3);
  sprintf(buff, ":%02d%% ", int(humidity));
  lcd.print(buff);

  lcd.write(2);
  sprintf(buff, ":%02dC", int(temperature));
  lcd.print(buff);
}

bool buttonClicked(int name){
  if(analogRead(name) > 900){
    lcd.clear();
    last_clicked = millis();
    BRIGHTNESS = true;
    return true;
  }
  return false;
}

int navigateMenu(int valueToAdd, int menu){
  menu += valueToAdd;
  if(menu < 1) menu = 1;
  else if (menu > MENU_COUNT) menu = MENU_COUNT;
  return menu;
}

void setTimeScreen(){
  sprintf(buff, "%d: Set time", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  sprintf(buff, "%02d/%02d/%04d %02d:%02d", day(), month(), year(), hour(), minute());
  lcd.setCursor(0, 1);
  lcd.print(buff);
}

void setAlarmScreen(){
  sprintf(buff, "%d: Alarm 1", MENU);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  
  
  sprintf(buff, "%02d:  %02d:%02d", weekday(), 0,0);
  lcd.setCursor(0, 1);
  lcd.print(buff);
}

// void brightnessScreen(){
//   int brightness16 = (BRIGHTNESS + 1) / 16;

//   if(buttonClicked(START)) EDIT_MODE = !EDIT_MODE;
//   if(EDIT_MODE && buttonClicked(LEFT)) brightness16 -= 1;
//   if(EDIT_MODE && buttonClicked(RIGHT)) brightness16 += 1;

//   if(brightness16 > 16) brightness16 = 16;
//   else if(brightness16 < 0) brightness16 = 0;

//   BRIGHTNESS = (brightness16 * 16);
//   if (BRIGHTNESS > 0) BRIGHTNESS-=1;

//   if(!EDIT_MODE)  sprintf(buff, "%d: Jasnosc - %d", MENU, brightness16);
//   else sprintf(buff, "%d: XDD - %d", MENU, brightness16);
//   lcd.setCursor(0, 0);
//   lcd.print(buff);

//   lcd.setCursor(0, 1);
//   for(int i = 0; i < brightness16; i++) lcd.write(1);
// }

void loop() {
  buttonClicked(START);
  if (!EDIT_MODE && buttonClicked(RIGHT)) MENU = navigateMenu(1, MENU);
  if (!EDIT_MODE && buttonClicked(LEFT)) MENU = navigateMenu(-1, MENU);
  
  if((last_temperature_update + TEMPERATURE_INTERVAL) < millis()){
     updateTemperature();
     last_temperature_update = millis();
  }

  if (MENU == 1) mainScreen();
  else if (MENU == 2) setTimeScreen();
  else if (MENU == 3) setAlarmScreen();
  // else if (MENU == 4) brightnessScreen();

  if((last_clicked + BACKLIGHT_TIME) < millis()) BRIGHTNESS = false;
  digitalWrite(SCREEN_PIN, BRIGHTNESS);  
  delay(250);
}
