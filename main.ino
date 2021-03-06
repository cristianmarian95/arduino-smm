/**
 * Fainisi Marian Cristian
 * Informatica Aplicata - UVT
 * marian.fainisi95@e-uvt.ro
 * An 2 Grupa 2 Sub-Grupa 3
 * 
 * Sistem pentru intretinerea umiditatii din sol si a temperaturii ambientale pentru plante
 * 
 */

/* Library */ 
#include <SimpleDHT.h>
#include <LiquidCrystal.h>
#include <TimedAction.h>


/* Declare functions */
void checkWaterLevel();
void checkMoistureLevel();
void checkTemperatureLevel();


/* Leds */
const int ledPinRed = 2;
const int ledPinGreen = 3;

/* Analog sensors */
const int analogMoisturePin = 0;
const int analogWaterLevelPin = 2;

/* LCD Display */
const int rs = 12;
const int en = 13;
const int d7 = 8;
const int d6 = 9;
const int d5 = 10;
const int d4 = 11;


/* Components */
const int ena1 = 7;
const int ena2 = 6;

const int enb1 = 5;
const int enb2 = 4;

/* ========== */
SimpleDHT11 dht11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Variabiles */
int moistureDataRead;
byte temperatureDataRead = 0;
byte humidityDataRead = 0;
int waterLevelDataRead;
int resultMoistureData;
int resultTemperatureData;
boolean waterLevelLow = false;
boolean waterPomp = false;
boolean startFan = false;

/* Action Timer */
TimedAction checkWaterLevelAction = TimedAction(1000,checkWaterLevel);
TimedAction checkMoistureLevelAction = TimedAction(1100,checkMoistureLevel);
TimedAction checkTemperatureLevelAction = TimedAction(5000,checkTemperatureLevel);

void setup() {
  Serial.begin(9600);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ena1, OUTPUT);
  pinMode(ena2, OUTPUT);
  pinMode(enb1, OUTPUT);
  pinMode(enb2, OUTPUT);
  Serial.println("Initialization....");
  lcd.begin(16, 2);
  lcd.print("Umiditate:");
  lcd.setCursor(0,1);
  lcd.print("Temperatura:");
}

void loop() {
  checkWaterLevelAction.check();
  checkMoistureLevelAction.check();
  checkTemperatureLevelAction.check();
  lcd.setCursor(10,0);
  lcd.print(resultMoistureData);
  lcd.setCursor(12,1);
  lcd.print(temperatureDataRead);

  if(startFan){
    digitalWrite(ena1,LOW);
    digitalWrite(ena2,HIGH);
  }else{
    digitalWrite(ena1,LOW);
    digitalWrite(ena2,LOW);
  }

  if(waterPomp){
    digitalWrite(enb1,HIGH);
    digitalWrite(enb2,LOW);
  }else{
    digitalWrite(enb1,LOW);
    digitalWrite(enb2,LOW);
  }

  delay(100);
}

void checkWaterLevel() {
  waterLevelDataRead = analogRead(analogWaterLevelPin);
  if(waterLevelDataRead < 480 && waterPomp == true){
    waterPomp = false;
  }
  if(waterLevelDataRead < 480){
    digitalWrite(ledPinRed,HIGH);
    digitalWrite(ledPinGreen,LOW);
    Serial.println("Atentie! nivel de apa scazut.");
    waterLevelLow = true;
  } else if (waterLevelDataRead < 600) {
    digitalWrite(ledPinRed,LOW);
    digitalWrite(ledPinGreen,HIGH);
    Serial.println("Nivel de apa mediu.");
    waterLevelLow = false;
  } else if (waterLevelDataRead > 600 ) {
    digitalWrite(ledPinRed,LOW);
    digitalWrite(ledPinGreen,HIGH);
    Serial.println("Nivel de apa ridicat.");
    waterLevelLow = false;
  }
  
}

void checkMoistureLevel() {
  moistureDataRead = analogRead(analogMoisturePin);
  resultMoistureData = map(moistureDataRead, 1024, 10, 0, 100);
  if(resultMoistureData < 20 && waterLevelLow == false){
    waterPomp = true;
  } else {
    waterPomp = false;
  }
  Serial.print("Umiditate sol: ");
  Serial.print(resultMoistureData);
  Serial.println("%");
}

void checkTemperatureLevel() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(A1, &temperatureDataRead, &humidityDataRead, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err);
    delay(1000);
    return;
  }
  if((int)temperatureDataRead > 23){
    startFan = true;
  }else{
    startFan = false;
  }
  Serial.print("Temperatura: ");
  Serial.print((int)temperatureDataRead);
  Serial.println("*C");
  Serial.print("Umiditate atmosfera: ");
  Serial.print((int)humidityDataRead);
  Serial.println("%");
  delay(1500);
}
