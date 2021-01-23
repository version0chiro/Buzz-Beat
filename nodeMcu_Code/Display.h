// *****FUNCTIONS****
//  display_splash();
//  display_move_away();
//  display_come_close();
//  display_okay('str');
//  display_sick('str');
//  display_connected(); 
//  display_config(); 
//  display_temp("36.2");
//  display_home("36.2");



#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#define camTrigger D4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define buzzer D5

 
static const unsigned char PROGMEM wifi[] =
{ 0b00000000, 0b00000000, //                 
  0b00000111, 0b11100000, //      ######     
  0b00011111, 0b11111000, //    ##########   
  0b00111111, 0b11111100, //   ############  
  0b01110000, 0b00001110, //  ###        ### 
  0b01100111, 0b11100110, //  ##  ######  ## 
  0b00001111, 0b11110000, //     ########    
  0b00011000, 0b00011000, //    ##      ##   
  0b00000011, 0b11000000, //       ####      
  0b00000111, 0b11100000, //      ######     
  0b00000100, 0b00100000, //      #    #     
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //   
  };
  static const unsigned char PROGMEM warning[] =
{ 0b00000000, 0b10000000, //         #       
  0b00000001, 0b11000000, //        ###      
  0b00000001, 0b11000000, //        ###      
  0b00000011, 0b11100000, //       #####     
  0b00000011, 0b01100000, //       ## ##     
  0b00000111, 0b01110000, //      ### ###    
  0b00000110, 0b00110000, //      ##   ##    
  0b00001110, 0b10111000, //     ### # ###   
  0b00001100, 0b10011000, //     ##  #  ##   
  0b00011100, 0b10011100, //    ###  #  ###  
  0b00011000, 0b10001100, //    ##   #   ##  
  0b00111000, 0b00001110, //   ###       ### 
  0b00110000, 0b10000110, //   ##    #    ## 
  0b01111111, 0b11111111, //  ###############
  0b01111111, 0b11111111, //  ###############
  0b00000000, 0b00000000, //    
  };

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_home(const String target, boolean stat,const String target1,const String target2,const String target3,const String target4) {
  Serial.println(target);
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSans9pt7b);       

  
  //display.print(F(" C"));
  display.setFont();
  display.setTextSize(1);     
  display.setCursor(10,5);             
  display.print("HB");
  display.setCursor(10,15);             
  display.print(target);
  display.setCursor(10,35);
  display.print("Temp");
  display.setCursor(10,45);
  display.print(target3);
  display.setCursor(55,5);
  display.print("HRV");
  display.setCursor(55,15);
  display.print(target1);
  display.setCursor(55,35);
  display.print("Spo2");
  display.setCursor(55,45);
  display.print(target2);
  display.setTextSize(2);
  display.setCursor(90,34);
  display.print(target4);
  //display.drawCircle(85, 32, 1, WHITE);
  if(stat){
  display.drawBitmap(110,3,wifi,16,16,1); 
  }
  display.display();
  }
  
void display_config() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  display.setCursor(10,28); 
  display.setFont(&FreeSans9pt7b);   
  display.print(F("Connect your")); 
  display.setCursor(38,56);
  display.print(F("Phone."));     
  display.drawBitmap(56,5,wifi,16,16,1);            
  display.display();
  }
  
void display_connected() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  display.setCursor(46,32); 
  display.setFont(&FreeSans9pt7b);   
  display.print(F("Wi-fi")); 
  display.setCursor(20,56);
  display.print(F("Connected"));     
  display.drawBitmap(56,5,wifi,16,16,1);            
  display.display();
  delay(2000);
  }
  
void display_come_close(void){
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSans12pt7b);       
  display.setCursor(30,23);             
  display.print(F("COME"));
  display.setCursor(16,47);             
  display.print(F("CLOSER"));
  display.setCursor(18,58); 
  display.setFont();
  display.setTextSize(1); 
  display.print(F("You are too far."));
  display.display();
  } 
  
void display_okay(const String target) {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSans9pt7b);       
  display.setCursor(37,18);             
  display.print(target);
  display.drawCircle(78, 7, 2, WHITE);
  display.print(F("  C"));
  display.setCursor(34,35); 
  display.print(F("You are"));
  display.setCursor(28,57);
  display.setFont(&FreeSans12pt7b); 
  display.print(F("OKAY!"));
  display.display();
  
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
  delay(2000);
  }
  
void display_sick(const String target) {
  digitalWrite(buzzer,HIGH);
  display.clearDisplay();
  for(int16_t i=0; i<=10; i+=1){
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSans9pt7b);       
  display.setCursor(37,18);             
  display.print(target);
  display.drawCircle(78, 7, 2, WHITE);
  display.print(F("  C"));
  display.setCursor(34,35); 
  display.print(F("You are"));
  display.setCursor(34,57);
  display.setFont(&FreeSans12pt7b); 
  display.print(F("SICK!"));
  display.display();
  if(i%2==0){
  display.setFont(&FreeSans9pt7b);  
  display.drawBitmap(11,26,warning,16,16,1);
  display.drawBitmap(103,26,warning,16,16,1);
  }
  display.display();
  delay(500);
  }
  digitalWrite(buzzer,LOW);
  digitalWrite(camTrigger,HIGH);
  delay(2000);
  digitalWrite(camTrigger,LOW);
  display.clearDisplay();
}
