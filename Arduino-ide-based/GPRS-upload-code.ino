#include <RTClib.h>  // Download the library from https://github.com/adafruit/RTClib
#include <Wire.h>    // Library for I2C Communication with DS3231 Module
#include <DHT.h>
#include <SoftwareSerial.h>
SoftwareSerial myserial(10, 11); // RX: 10, TX:11
#include <ArduinoJson.h>
StaticJsonBuffer<200> jsonBuffer; 
 
 
#define DHTPIN A1
#define DHTTYPE DHT11
 
RTC_DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE);
 
char t[32];
char deviceID[12] = "MYTEST56";
 
 
void setup()
{
  myserial.begin(9600);        // the GPRS baud rate
  Serial.begin(9600);
  Serial.println("Initializing..........");
  dht.begin();
  Wire.begin();
  DynamicJsonBuffer jsonBuffer;
 
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2020, 02, 29, 17, 50, 40));
  delay(5000);
}
 
void loop()
{
  Serial.println("");
  Serial.println("************************************************************");
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
 
  DateTime now = rtc.now();
 
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
 
  Serial.print("Device ID: ");
  Serial.println(deviceID);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print(F("Time/Date: "));
  Serial.println(t);
  delay(1000);
 
 
 /********************GSM Communication Starts********************/
 
  if (myserial.available())
  Serial.write(myserial.read());
 
  myserial.println("AT");
  delay(3000);
 
  myserial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");//APN
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=1,1");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=2,1");
  delay(6000);
  ShowSerialData();
 
 
  myserial.println("AT+HTTPINIT");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CID\",1");
  delay(6000);
  ShowSerialData();
 
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.createObject();
  
  object.set("deviceID",deviceID);
  object.set("humidity",humidity);
  object.set("temperature",temperature);
  object.set("timedate",t);
  
  object.printTo(Serial);
  Serial.println(" ");  
  String sendtoserver;
  object.prettyPrintTo(sendtoserver);
  delay(4000);
 
  myserial.println("AT+HTTPPARA=\"URL\",\"http://192.xxxxxxxxxxxxxxxxxxxxxxxx.php\""); //Server address
  delay(4000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  delay(4000);
  ShowSerialData();
 
 
  myserial.println("AT+HTTPDATA=" + String(sendtoserver.length()) + ",100000");
  Serial.println(sendtoserver);
  delay(6000);
  ShowSerialData();
 
  myserial.println(sendtoserver);
  delay(6000);
  ShowSerialData;
 
  myserial.println("AT+HTTPACTION=1");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPREAD");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPTERM");
  delay(10000);
  ShowSerialData;
 
  /********************GSM Communication Stops********************/
 
}
 
 
void ShowSerialData()
{
  while (myserial.available() != 0)
    Serial.write(myserial.read());
  delay(1000);
 
}
