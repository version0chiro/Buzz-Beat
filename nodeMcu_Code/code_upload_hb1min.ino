#include <Wire.h>
#include <FS.h>
#include <ArduinoJson.h>   
#include <math.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>  
#include <ESP8266WiFi.h>
#include "display.h"
#include "sensor.h"
#include "MAX30100_PulseOximeter.h"
#define camTrigger D4
#define trigger D7
#define buzzer D5
#define REPORTING_PERIOD_MS     1000

#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

int i = 0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;
const String Count;
ESP8266WebServer server(80);
String sensorDataToTransmit;
bool shouldSaveConfig = false;
uint8_t stat;
long time1 = 0, time2 = 0;
double count = 0;
PulseOximeter pox;
uint32_t tsLastReport = 0;
long hrv_time1 = 0, hrv_time2 = 0;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

void onBeatDetected()
{
  count++;
  hrv_time1 = (millis() - hrv_time2);
  hrv_time2 = millis();
}

void setup() {
  Serial.begin(115200);
  pinMode(camTrigger,OUTPUT);
  pinMode(trigger,INPUT);
  pinMode(buzzer, OUTPUT);
  WiFi.disconnect();
  EEPROM.begin(512);
  delay(10);
  
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  WiFi.begin(esid.c_str(), epass.c_str());

  if (!testWifi()) {
    Serial.println("Connection Status Negative / D15 HIGH");
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }
  
  Serial.println();
  Serial.println("Waiting.");
    
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    server.handleClient();Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  
  mlx.begin(); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.clearDisplay();
  Serial.println("Initializing pulse oximeter..");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_50MA);
    display_connected();
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
    time1 = millis();
    time2 = millis();
    hrv_time1 = millis();
    hrv_time2 = millis();
//    server.on("/", handle_root);
//    server.begin();
//    Serial.println("HTTP server started");
}

void loop() {
       pox.update();
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {        
        hrv = (60000/(hb+1) - hrv_time1)/1000; 
        //hb = pox.getHeartRate();
        spo2 = pox.getSpO2();
        //oxi_temp = pox.getTemperature();
        display_home((String(hb,0)),1,(String(hrv,1)),(String(spo2,0)),(String(temperature,1)),(String(count,0)));
        tsLastReport = millis();
    };
   if(millis() - time1 > 30000){
    hb = count*2;
    count = 0;
    time1 = millis();
//    Serial.print("Heart rate:");
//    Serial.println(hb);
    temperature = mlx_read();
    Serial.println(temperature);
    
   }
   if(millis() - time2 > 3000) {
       Serial.print(hrv);
       Serial.print(",");
       Serial.print(spo2);
       Serial.print(",");
       Serial.print(temperature);
       Serial.print(",");
       Serial.println(count);
       time2 = millis();
       json_send();
   }
   // handle client connections  
   //server.handleClient();
}

void handle_root() {
  server.send(200, "text/html", sensorDataToTransmit);
}

void json_send() {
  StaticJsonBuffer<100> sensorValues;
  JsonObject& sensor = sensorValues.createObject(); 
  if (sensor.success()) {
      Serial.println("\nparsed json");
      sensor["spo2"] = spo2;
      sensor["hrv"] = hrv;
      sensor["hb"] = hb*2.5;
      //sensor["Ambient(degC)"] = mlx.readAmbientTempC();
      //sensor["oxi_temp"] = oxi_temp;
      sensor["temp"] = temperature;
      // clear any previous data
      sensorDataToTransmit = "";
      sensor.printTo(sensorDataToTransmit); 
      Serial.println("This is the sensor data that will be transmitted - ");
      Serial.println(sensorDataToTransmit);
      
      HTTPClient http;    //Declare object of class HTTPClient
       
      http.begin("http://spo2-registration.herokuapp.com/neoSenseSender");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
   
      int httpCode = http.POST(sensorDataToTransmit);   //Send the request
      String payload = http.getString();                                        //Get the response payload
   
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
   
      http.end();
      
      }
}

//-------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
 
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
 
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
 
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("advanced_pulse_oxi", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}
 
void createWebServer()
{
 {
    server.on("/", []() {
 
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><label>Password: </label><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
 
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
 
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
 
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
 
    });
  } 
}



