#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GyverEncoder.h"
#include <virtuabotixRTC.h>
#include <Adafruit_GrayOLED.h>
#include <Fonts/FreeSansOblique12pt7b.h>  
#include <Fonts/FreeSansOblique9pt7b.h>


#define CLK 3
#define DT 4
#define SW 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Encoder enc1(CLK, DT, SW); // для работы c кнопкой
virtuabotixRTC myRTC(6, 7, 8);

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

#define BT_BMPWIDTH 13
#define BT_BMPHIGHT 6

#define CH_BMPWIDTH 8
#define CH_BMPHIGHT 7

static const unsigned char PROGMEM logo_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

static const unsigned char PROGMEM bitmap_battary[] =
    {B01111111, B11000000,
     B11111111, B11100000,
     B11111111, B11111000,
     B11111111, B11111000,
     B11111111, B11100000,
     B01111111, B11000000};


static const unsigned char PROGMEM bitmap_charging[] =
    {B00110000,
     B01100000,
     B11000000,
     B11110000,
     B00110000,
     B01100000,
     B11000000};

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  
  myRTC.setDS1302Time(15, 6, 21, 1, 14, 10, 2021);
  display.display();
  display.setFont(&FreeSansOblique12pt7b);
  delay(1000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white

  // Show the display buffer on the screen. You MUST call display() after\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


  display.setCursor(0, 20);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.display();
  delay(1000);
}

void dayOfWeek()
{
  int dow = myRTC.dayofweek;
  if (dow == 1)
  {
    char dayOfWeekWord = "Monday";
    // display.setCursor(37, 15);
    // display.print(dayOfWeekWord);
  }
}

void mainDisplay()
{
  display.setFont();
  display.setCursor(1, 50);
  display.print(myRTC.dayofmonth);
  display.print(".");
  display.print(myRTC.month);
  display.print(".");
  display.print(myRTC.year);

  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.drawLine(0, 47, 75, 47, SSD1306_WHITE);
  display.drawBitmap(115, 1, bitmap_battary, 13, 6, WHITE);
  display.drawBitmap(109, 1, bitmap_charging, 4, 7, WHITE);

  display.setFont(&FreeSansOblique12pt7b);
  display.setTextSize(1);
  display.setCursor(1, 43);
  display.print(myRTC.hours);
  display.print(":");
  display.print(myRTC.minutes);
  
  display.setFont(&FreeSansOblique9pt7b);
  display.print(".");
  display.print(myRTC.seconds);

  display.display();
}

int r = 0;
void turn()
{
  enc1.tick();

  if (enc1.isRight())
  {
    r = r + 1;
    Serial.println(r);
  }

  if (enc1.isLeft())
  {
    r = r - 1;
    Serial.println(r);
  }
  display.clearDisplay();
}

bool ifon = LOW;
int timeLeft = 0;
void loop()
{
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0x8F);


  enc1.tick();
  myRTC.updateTime();
  mainDisplay();
  turn();
  dayOfWeek();

  timeLeft = timeLeft+1;
  delay(5);

  if (enc1.isRightH())
    Serial.println("Right holded"); // если было удержание + поворот
  if (enc1.isLeftH())
    Serial.println("Left holded");

  if (enc1.isPress())
    Serial.println("Press"); // нажатие на кнопку (+ дебаунс)

  if (enc1.isClick() && ifon == HIGH){
    display.ssd1306_command(SSD1306_DISPLAYON);
    display.fadein();
    Serial.println("Click"); // если была удержана и энк не поворачивался
    ifon = LOW;

    //display.ssd1306_command(SSD1306_SETCONTRAST);
    
    delay(30);
    timeLeft = 0;
  }
   if (timeLeft >= 100){
    display.fadeout();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    Serial.println("AUTO SLEEP");
    ifon = HIGH;
    timeLeft = 0;
   }

  if (enc1.isHolded()){
    display.fadeout();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    Serial.println("Holded"); // если была удержана и энк не поворачивался 
    ifon = HIGH; 
  }
    

  //Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system

  //Serial.print(myRTC.month);

  //Serial.print(myRTC.year);

  //Serial.print(myRTC.hours);

  //Serial.print(myRTC.minutes);

  //Serial.println(myRTC.seconds);
  delay(10);
}