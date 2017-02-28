#include <Servo.h>    // standard servo motor routines

// These are only available in this file - so static
static const int Threshold = 200;
static const int PanInitPosition = 90; //90
static const int TiltInitPosition = 100; //100
static const int C02InitPosition = 150;
static const int C02FirePosition = 50;
static const int CO2FireDuration = 250;

class FlameSensor {
    enum ServoType {
      PANSERVO,
      TILTSERVO,
      CO2SERVO,
      NONE
    };
    // define fixed positions for servos
    Servo _panServo;
    Servo _tiltServo;
    Servo _co2Servo;

    int _sensorPin;
    int _maxPanPos;
    int _maxTiltPos;

    int _findMax(ServoType whichServo, int startPos, int endPos, int incr = 10)
    {
      // initialize so we don't use old readings
      int curValue = 0;
      int maxValue = 0;
      int maxServoPos = 0;
      int curPos = startPos;
      if (incr <= 0) return 0; // this is really an error condition.
      if (startPos < endPos) return 0; // Let's just keep it simple
      if (whichServo != PANSERVO || whichServo != TILTSERVO) return 0;
 //     Serial.println("I have printed the initial values");
      maxValue = curValue = readValue();
      curPos += incr;
      for (; curPos < endPos; curPos += incr) {
        if (whichServo == PANSERVO)
          _panServo.write(curPos);
        else
          _tiltServo.write(curPos);
        delay(1000);
        curValue = readValue();
        if (curValue > maxValue) {
          maxValue = curValue;  // replace oldValue with previous curValue reading
          maxServoPos = curPos;
        }
      }
      return maxServoPos;
    }

  public:
    FlameSensor(int sensorPin) : _sensorPin(sensorPin), _maxPanPos(0), _maxTiltPos(0) {}

    unsigned int readValue ()         // function to establish a value for flame test reading
    {
      unsigned int curValue = 0;
      for (int i = 0; i < 10; ++i)
      {
        delay(10);
        curValue += analogRead(_sensorPin);  // get flame reading at this location
      }
      return curValue / 10;
    }

    void setup(int panServoPin, int tiltServoPin, int co2ServoPin, int co2LedPin) {
      _panServo.attach(panServoPin);
      _tiltServo.attach(tiltServoPin);
      _co2Servo.attach(co2ServoPin);
      pinMode(_sensorPin, INPUT);
      pinMode(co2LedPin, OUTPUT);
      resetPosition();
    }

    void resetPosition()
    {
      // set all servos to their initial positions
      _panServo.write(PanInitPosition);
      _tiltServo.write(TiltInitPosition);
      _co2Servo.write(C02InitPosition);
    }
    int panFindMax(int startPos, int endPos, int incr = 10)
    {
 //     Serial.println("pan start");
      //_maxPanPos = _findMax(PANSERVO, startPos, endPos, incr);
      // initialize so we don't use old readings
      int curValue = 0;
      int maxValue = 0;
      _maxPanPos = PanInitPosition;
      int curPos = startPos;
      if (incr <= 0) {
        return 0; // this is really an error condition.
      }
      if (startPos > endPos) {
        return 0; // Let's just keep it simple
      }
      maxValue = curValue = readValue();
      curPos += incr;
      for (; curPos < endPos; curPos += incr) {
        _panServo.write(curPos);
        delay(1000);
        curValue = readValue();
//        Serial.println(curValue);
        if (curValue > maxValue) {
          maxValue = curValue;  // replace oldValue with previous curValue reading
          _maxPanPos = curPos;
        }
      }

      _panServo.write(_maxPanPos);
      delay(1000);
//      Serial.println("Pan Done");
      return _maxPanPos;
    }

    int tiltFindMax(int startPos, int endPos, int incr = 5)
    {
      //_maxTiltPos = _findMax(TILTSERVO, startPos, endPos, incr);
  //    Serial.println("tilt");
      //_maxPanPos = _findMax(PANSERVO, startPos, endPos, incr);
      // initialize so we don't use old readings
      int curValue = 0;
      int maxValue = 0;
      _maxTiltPos = TiltInitPosition;
      int curPos = startPos;
      if (incr <= 0) {
        return 0; // this is really an error condition.
      }
      if (startPos > endPos) {
        return 0; // Let's just keep it simple
      }
      maxValue = curValue = readValue();
      curPos += incr;
      for (; curPos < endPos; curPos += incr) {
        _tiltServo.write(curPos);
        delay(1000);

        curValue = readValue();
  //      Serial.println(curValue);
        if (curValue > maxValue) {
          maxValue = curValue;  // replace oldValue with previous curValue reading
          _maxTiltPos = curPos;
        }
      }

      _tiltServo.write(_maxTiltPos);
      delay(1000);
 //     Serial.println("Tilt Done");
      return _maxTiltPos;
    }

    bool foundFlame() {
      return readValue() > Threshold;
    }

    void setPos(int pan, int tilt) {
      _panServo.write(pan);
      _tiltServo.write(tilt);
      delay(50);
    }

    void gotoMaxPos() {
      _panServo.write(_maxPanPos);
      _tiltServo.write(_maxTiltPos);
      delay(50);
    }

    void fireCO2() {
      _co2Servo.write(C02FirePosition);
      delay(CO2FireDuration);
      _co2Servo.write(C02InitPosition);
    }
};

