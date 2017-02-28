//Include the H file Code
#include "flame_kaavya.h"

// include the library code:
#include <Wire.h>
// #include <Adafruit_RGBLCDShield.h>
// #include <utility/Adafruit_MCP23017.h>
#include <Servo.h>    // standard servo motor routines


// The LCD shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.

// Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// Define Constants
const int SPKR = 5;
//const int QTISensorPin = A6;    // Analog pin A6 == Digital D60
//const int PurpleButton = 28;    // This is the left  purple user button
//const int YellowButton = 29;    // This is the right yellow user button
const int FlameSensorPin = A7;  // Analog pin A7  == Digital D61

const int TiltServoPin = tilt_p;
const int PanServoPin = pan_p;
const int CO2ServoPin = 43;
const int CO2LEDPin = 12;     // Double check this if it exists ~ This exists dw :)

const int LeftMotorPin  = 52;
const int RightMotorPin = 53;

//char LINE[32];

// QTISensor qtiSensor(QTISensorPin);
FlameSensor flameSensor(FlameSensorPin);

//Servo RightServo;               // makes Servo functions available to RightServo
//Servo LeftServo;                // makes Servo functions available to LeftServo

#define PxLED Serial  // PxLED needs to be connected to Serial2 i.e. pin18
int front = 0;
int left = 0;
int right = 0;

int panMax = 0;
int tiltMax = 0;

char posChar = 128;
char LINE[48]; // 16 chars per line, add some for control chars if needed
const char CLEAR = '\f';
//const char CR = '\r'; //Does this always work?
const char CR = 13; // Try implicit conversion
const char BACKLIGHT_ON = 17;
const char BACKLIGHT_OFF = 18;

void ftksetup() {
  // Initiate serial communication @ 9600 baud
//  Serial.begin(9600);
  // set up the LCD's number of columns (16) and rows (2):
//  lcd.begin(16, 2);

  // LCD column and row layout:

  //  col = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15  <== row 0
  //  col = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 15 15  <== row 1


//  lcd.print("Flame Detector");
//  lcd.setBacklight(WHITE);

  // Sensor setup
//  qtiSensor.setup();
  flameSensor.setup(PanServoPin, TiltServoPin, CO2ServoPin, CO2LEDPin);
//  Serial.println("PanServoPin =");
//  Serial.println(PanServoPin);
//  Serial.println("Sensor setup complete");
 // delay(1000);
  // panMax = flameSensor.panFindMax(30, 150);
 // Serial.println("ENTIRE SETUP DONE");
 // delay(1000);
}


void ftkloop()
{
//  Serial.println("Going to start the VOID LOOP");
  flameSensor.resetPosition();
  delay(50);

  /*  lcd.setBacklight(WHITE);
    //lcd.clear();
    lcd.setCursor(0, 0);  // go to 1st col of top row
    lcd.print("Select function");
    //  lcd.setCursor(0,1);   // go to 1st col of 2nd row
    //  lcd.print("to be displayed");
    Serial.println("Backlight stuff complete");

    uint8_t buttons = lcd.readButtons();  // Get button status from RGB LCD function
    Serial.println("button status complete");

    // this function returns the status of all buttons at once, in the bit pattern
    // of the byte "buttons"

    // First, check to see if ANY of the buttons are pressed:
    Serial.println(buttons);

    if (buttons) {
      Serial.println("Hiiiiiiiiiiiiii");
      lcd.clear();
      lcd.setCursor(0, 0);

      // Now, use the bitwise AND operator ("&")
      // to see which button has been (is being?) pressed.
      Serial.println("Passed the 'button has been/is being pressed' stage");
  */
  //   if (buttons & BUTTON_RIGHT) {
  //    Serial.println("Right Button Was Pressed");
 // Serial.println("next SHOULD be the panMax function");
  panMax = flameSensor.panFindMax(30, 150);
 // Serial.print("panMax = ");
 // Serial.println(panMax);
 // Serial.println("continuing on...");
 // Serial.println("Flame was searched for");
  tiltMax = 90;
  flameSensor.setPos(panMax, tiltMax);
  delay(100);
  unsigned int flameVal = flameSensor.readValue();
//  Serial.print("Flame val = ");
//  Serial.println(flameVal);
  if (flameVal > 70) {
    delay(100);
  //  Serial.println("Flame has been detected");
    tiltMax = flameSensor.tiltFindMax(80, 120);
    delay(10);
   // Serial.println("Tilt was adjusted to better aim at the flame");
  }
  //    lcd.print3Values(1, panMax, tiltMax, flameSensor.readValue());
  sprintf(LINE, " panMax=%d, tiltMax=%d, flameSensor.readValue()=%d\n", panMax, tiltMax, flameSensor.readValue());
 // Serial.print(LINE);

  //}
  //}
  delay(5000);
}

