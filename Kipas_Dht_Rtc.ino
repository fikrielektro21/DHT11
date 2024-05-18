// import library
#include <DHT.h>
#include <DHT_U.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(2, DHT11);
#define fan 3
#define Led1 10
#define Led2 11
ThreeWire myWire(4, 5, 7); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() { 
  Serial.begin(57600);
  pinMode(fan, OUTPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  digitalWrite(fan, HIGH);
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }

  Serial.println("Do you want to set the time? (y/n)");
  while (!Serial.available());
  char response = Serial.read();
  if (response == 'y' || response == 'Y') {
    setTimeFromSerial();
  }
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println(" +");

  byte kelembaban = dht.readHumidity();
  byte suhu = dht.readTemperature();
  
  //Serial.print("Suhu: ");
  //Serial.print(suhu);
 // Serial.print("A");
  //Serial.print("Kelembaban: ");
 // Serial.println(kelembaban);

  lcd.setCursor(0, 0);
  char datestring[16];
  snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            now.Month(),
            now.Day(),
            now.Year(),
            now.Hour(),
            now.Minute(),
            now.Second());
  lcd.print(datestring);

  lcd.setCursor(6, 1);
  //lcd.print("Suhu: ");
  lcd.print(suhu);
  lcd.print((char)223);
  lcd.print("C ");
  //lcd.print("AIO");
  //lcd.print("%");
  

  if (suhu >= 30) {
    digitalWrite(fan, LOW);
    digitalWrite(Led2, HIGH);
    digitalWrite(Led1, LOW);
    ngeprint();
  } else {
    digitalWrite(fan, HIGH);
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, LOW);
    baik();
  }
  delay(1000);
}

void ngeprint() {
  lcd.setCursor(0, 1);
  lcd.print("ALERT!");
  delay(600);
  lcd.setCursor(0, 1);
  lcd.print("      ");
  delay(400);
  lcd.setCursor(11, 1);
  lcd.print("Kipas");
  delay(600);
}

void baik(){
  lcd.setCursor(0, 1);
  lcd.print("Baik ");
  lcd.setCursor(11, 1);
  lcd.print("Aman");
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[26];

  snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second());
  Serial.print(datestring);
}

void setTimeFromSerial() {
  int year, month, day, hour, minute, second;

  Serial.println("Enter year (YYYY): ");
  while (!Serial.available());
  year = Serial.parseInt();
  Serial.println(year);

  Serial.println("Enter month (MM): ");
  while (!Serial.available());
  month = Serial.parseInt();
  Serial.println(month);

  Serial.println("Enter day (DD): ");
  while (!Serial.available());
  day = Serial.parseInt();
  Serial.println(day);

  Serial.println("Enter hour (HH): ");
  while (!Serial.available());
  hour = Serial.parseInt();
  Serial.println(hour);

  Serial.println("Enter minute (MM): ");
  while (!Serial.available());
  minute = Serial.parseInt();
  Serial.println(minute);

  Serial.println("Enter second (SS): ");
  while (!Serial.available());
  second = Serial.parseInt();
  Serial.println(second);

  RtcDateTime newTime(year, month, day, hour, minute, second);
  Rtc.SetDateTime(newTime);

  Serial.println("Time set successfully!");
}
