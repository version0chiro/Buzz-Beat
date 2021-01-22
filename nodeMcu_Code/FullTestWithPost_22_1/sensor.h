//****Functions***
//  SR04_read();
//  mlx_read();
//  buzz_1sec();
//  buzz_5sec();
//

#include <Adafruit_MLX90614.h>

#define buzzer D5
#define camTrigger D4

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

double ambientTemp,objectTemp,mlx_avg=0, temperature = 0;
float spo2=0, hrv=0, hb=29, oxi_temp=0, hb_time = 0;

void buzz_1sec(void) {
 digitalWrite(buzzer,HIGH);
 delay(150);
 digitalWrite(buzzer,LOW);
}

void buzz_5sec(void) {
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(5000);        // ...for 1 sec
  noTone(buzzer);
}

double read_avg(double value1) {
  double avg = 0;
  for(int i=5;i>=0;i--){
    avg = (value1 + avg);
  }
  return avg/5;
}

double mlx_read(void) {
    ambientTemp = mlx.readAmbientTempC();
    objectTemp = read_avg(mlx.readObjectTempC());
    objectTemp=objectTemp-5.5;
  if(objectTemp<37.7){
   display_okay(String(objectTemp,1)); 
  }
  else if(objectTemp>37.7 && objectTemp<120)
  {
   //display_sick(String(objectTemp,1));
  }
  return objectTemp;
}

void heroku_post (void) {
    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

    Serial.println(host);

    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  
  String getData, Link;
  
  //POST Data
  Link = "/post";

  Serial.print("requesting URL: ");
  Serial.println(host);
  /*
   POST /post HTTP/1.1
   Host: postman-echo.com
   Content-Type: application/x-www-form-urlencoded
   Content-Length: 13
  
   say=Hi&to=Mom
    
   */

  httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded"+ "\r\n" +
               "Content-Length: 13" + "\r\n\r\n" +
               "say=Hi&to=Mom" + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
    
  delay(2000);  //POST Data at every 2 seconds
}
