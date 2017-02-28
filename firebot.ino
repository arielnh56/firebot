
#include <NewPing.h>
#include <Servo.h>
#include <PID_v1.h>
#include "Analog32ma.h"
#include "settings.h"

Analog32ma flameMA(flame_p, 0.0, 1.0);
Analog32ma lineMA(line_p, 0.0, 1.0);

Servo leftTrack, rightTrack;
Servo pan, tilt, co2;
unsigned long int lastFlame = 0;
boolean gettingWarmer = false;
boolean gettingColder = false;
boolean firefirefire = false;
int flame = 0;
int oldFlame = 0;
boolean line_seen = false;
unsigned long int lastLine = 0;

int8_t leftSpeed, rightSpeed;
int8_t newLeftSpeed, newRightSpeed;

long debounceDelay = 50;
long leftLastDebounce = 0;
long rightLastDebounce = 0;
int leftLastState = HIGH;
int rightLastState = HIGH;
int leftState = HIGH;
int rightState = HIGH;
boolean stopped, started, stopping;
int range[SONAR_NUM];
uint8_t driving_mode;
uint16_t stage;
uint16_t substage;
unsigned long int substage_timer;
uint16_t last_stage = 1001;
uint16_t last_substage = 1001;
unsigned long int QTIvalue = 0;

#ifdef DEBUG
unsigned long int lastDebug = 0;
#endif

// set up pid for hallway centering
double pid_setpoint, pid_input, pid_output;
PID myPID(&pid_input, &pid_output, &pid_setpoint, PID_KP, PID_KI, PID_KD, DIRECT);

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("in setup()");
#endif
  // display
  Serial2.begin(9600);
  Serial2.write(0x0E); // Display Driver On
  Serial2.write(0x0C); // clear screen
  Serial2.write(0x03); // set font back to normal
  Serial2.write(0x01); // Home
  Serial2.write("Push to start ->");
  // attach servos
  // HB-25 range is 1000us to 2000us
  leftTrack.attach(leftTrack_p, 1000, 2000);
  rightTrack.attach(rightTrack_p, 1000, 2000);
  // pan.attach(pan_p);
  // tilt.attach(tilt_p);
  // co2.attach(co2_p);
  // co2.write(CO2_PARK);
  // set controllers to middle/stop
  // 0 = full speed ahead
  // 180 = full speed astern
  // 90 = stop
  leftSpeed = 0;
  rightSpeed = 0;
  leftTrack.write(STOP);
  rightTrack.write(STOP);
  // pan.write(CENTER);
  // tilt.write(CENTER);
  // buttons
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  // start by being stopped
  stopped = true;
#ifdef DEBUG_FIRE
  delay(1000);
  douse_fire();
#endif
  pid_setpoint = 0;
  pid_input = 0;
  pid_output = 0;
  myPID.SetOutputLimits(-90, 90);
  myPID.SetSampleTime(PID_TIMER);
  driving_mode = DRIVE_STOP;
  stage = HALL_1_O;
  substage = 0;
  substage_timer = 0;
  ftksetup();
}

void loop() {
  // reset our speed adjustments
  newLeftSpeed = leftSpeed;
  newRightSpeed = rightSpeed;

  // check our buttons
  checkButtons();

  // if we are still stopped, that's all we do. Just to be sure, we keep telling the motors that.
  // if these controllers get one bad blip they will start moving until told otherwise
  if (stopped) {
    rightSpeed = 0;
    leftSpeed = 0;
    leftTrack.write(STOP);
    rightTrack.write(STOP);
    pan.write(CENTER);
    tilt.write(CENTER);
    stage = HALL_1_O;
    substage = 0;
    substage_timer = 0;
    return;
  }

  // we were stopped, but now we are not

  // check our pings
  checkPings();

  // update flame sensor moving average
  flameMA.update();

  // check our line sensor
  if (millis() - lastLine > LINE_TIMER) {
    lastLine = millis();
    //   line_seen = (RCtime(line_p) < SEE_LINE);
    line_seen = QTIvalue < 2000;
#ifdef DEBUGLINE
    Serial.print("line = ");
    //    Serial.print(RCtime(line_p));
    Serial.print(QTIvalue);
    Serial.println("");
#endif
    setQTI(line_p);
  }

  // check flame sensor
  if (millis() - lastFlame > FLAME_TIMER) {
    oldFlame = flame;
    flame = flameMA.getVal();
    lastFlame = millis();
    gettingWarmer = (flame > oldFlame);
    gettingColder = (flame < oldFlame);
    firefirefire  = (flame >= SEE_FIRE);
#ifdef DEBUGFLAME
    Serial.print("flame = ");
    Serial.print(flame);
    if (gettingWarmer) Serial.print(" getting warmer ");
    if (gettingColder) Serial.print(" getting colder ");
    if (firefirefire)  Serial.print(" FIRE! ");
    Serial.println("");
#endif
  }

  // update PID
  pid_input = drift();
  myPID.Compute();
  // sensors all checked. Now decide what to do

  // test the pan tilt
  // setPanTilt(100, 100);

#ifdef DEBUGNAV
  Serial.print("stage="); Serial.print(stage);
  Serial.print(" substage="); Serial.print(substage);
  Serial.print(" driving_mode="); Serial.println(driving_mode);
  Serial.print("RIGHTPING="); Serial.println(range[RIGHTPING]);
#endif
  if (last_stage != stage) {
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x50); // Home
    Serial2.write(stageNames[stage]);
    last_stage = stage;
  }
  if (last_substage != substage) {
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x5F); // Home
    Serial2.print(substage);
    last_substage = substage;
  }
  switch (stage) {
    case HALL_1_O: // down the first hall and turn right
      switch (substage) {
        case 0: // down the hall to the end
          if ((range[FRONTPING] > 0 && range[FRONTPING] < 60) &&
              (range[RIGHTPING] == 0 || range[RIGHTPING] > 30)) {
            substage = 1;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            driving_mode = DRIVE_FWD;
          }
          break;
        case 1: // shuffle forwards a bit
          if (substage_timer == 0) substage_timer = millis() + 200;
          if (substage_timer < millis()) {
            substage = 2;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            driving_mode = DRIVE_FWD;
          }
          break;
        case 2: // turn right
          if (substage_timer == 0) substage_timer = millis() + 90 * DEGREES;
          if (substage_timer > millis()) {
            driving_mode = TURN_RIGHT;
          } else {
            substage = 0;
            substage_timer = 0;
            stage = HALL_2_O;
            driving_mode = DRIVE_STOP;
          }
          break;
      }
      break;
    case HALL_2_O:
      switch (substage) {
        case 0: // down the hall to the end
          if ((range[FRONTPING] > 0 && range[FRONTPING] < 60) &&
              (range[RIGHTPING] == 0 || range[RIGHTPING] > 30)) {
            substage = 1;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            driving_mode = DRIVE_FWD;
          }
          break;
        case 1: // turn right
          if (substage_timer == 0) substage_timer = millis() + 90 * DEGREES;
          if (substage_timer > millis()) {
            driving_mode = TURN_RIGHT;
          } else {
            substage = 0;
            substage_timer = 0;
            stage = ROOM_1_I;
            driving_mode = DRIVE_STOP; //changed left to STOP
          }
          break;
      }
      break;

    case ROOM_1_I:
      switch (substage) {
        case 0: // forwards until we are sure that we are in a hallway
          if (range[RIGHTPING] > 0 && range[LEFTPING] > 0) {

            substage = 1;
            substage_timer = 0;
            // driving_mode = DRIVE_STOP;
          } else {
            driving_mode = DRIVE_FWD;
          }
          break;
        case 1: // looking for something on the left and nothing on the right
          if (( range[LEFTPING] > 0) &&
              (range[RIGHTPING] == 0 || range[RIGHTPING] > 30)) {
            substage = 2;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            driving_mode = DRIVE_FWD;
          }
          break;
        case 2: // turn right
          if (substage_timer == 0) substage_timer = millis() + 90 * DEGREES;
          if (substage_timer > millis()) {
            driving_mode = TURN_RIGHT;
          } else {
            substage = 0;
            substage_timer = 0;
            stage = ROOM_1_S;
            driving_mode = DRIVE_STOP;
          }
          break;
      }
      break;
    // ROOM_1_S Dummy room search, drive in, turn, drive back out
    case ROOM_1_S:
      switch (substage) {
        case 0: //Forward until we see the line or a wall and not crash
          if (line_seen || (range[FRONTPING] != 0 && range[FRONTPING] < 25)) {

            substage = 1;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            range[RIGHTPING] = 0;  // don't look at the far wall
            range[LEFTPING] = 0;  // don't look at the left wall
            driving_mode = DRIVE_FWD;
          }
          break;
        case 1: // search
          ftkloop();
          substage = 2;
          break;
        case 2: //reverse until we see the line or 5 second
          if (substage_timer == 0) substage_timer = millis() + 5000;
          if (line_seen || substage_timer < millis()) {
            substage = 3;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            range[RIGHTPING] = 0;  // don't look at the far wall
            range[LEFTPING] = 0;  // don't look at the left wall
            pid_output = 0;
            driving_mode = DRIVE_REVERSE;
          }
          break;
        case 3: //reverse half a sec
          if (substage_timer == 0) substage_timer = millis() + 500;
          if (substage_timer < millis()) {
            substage = 4;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            range[RIGHTPING] = 0;  // don't look at the far wall
            range[LEFTPING] = 0;  // don't look at the left wall
            pid_output = 0;
            driving_mode = DRIVE_REVERSE;
          }
          break;
        case 4: //Rotate left 90 degrees
          if (substage_timer == 0) substage_timer = millis() + 1000;
          if (substage_timer < millis()) {
            substage = 0;
            stage = FINISHED;
            substage_timer = 0;
            driving_mode = DRIVE_STOP;
          } else {
            driving_mode = ROTATE_LEFT;
          }
          break;
      }
      break;
    case FINISHED:
      driving_mode = DRIVE_STOP;
      break;

  }
  /*
    =========================Defining Driving Modes==================================
    set speed according to movement type*/
  switch (driving_mode) {
    case DRIVE_STOP:
      myPID.SetMode(MANUAL);
      newLeftSpeed = 0;
      newRightSpeed = 0;
      myPID.SetMode(MANUAL);
      break;
    case DRIVE_FWD:
      myPID.SetMode(AUTOMATIC);
      newLeftSpeed = STANDARD_SPEED + constrain(pid_output, 0 - PID_MAX, PID_MAX);
      newRightSpeed = STANDARD_SPEED - constrain(pid_output, 0 - PID_MAX, PID_MAX);
      break;
    case TURN_LEFT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = INSIDE_TURN_SPEED;
      newRightSpeed = STANDARD_SPEED;
      break;
    case TURN_RIGHT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = STANDARD_SPEED;
      newRightSpeed = INSIDE_TURN_SPEED;
      break;
    case DRIVE_REVERSE:
      myPID.SetMode(AUTOMATIC);
      newLeftSpeed = 0 - (STANDARD_SPEED + constrain(pid_output, 0 - PID_MAX, PID_MAX));
      newRightSpeed = 0 - (STANDARD_SPEED - constrain(pid_output, 0 - PID_MAX, PID_MAX));
      break;
    case ROTATE_LEFT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = 0 - STANDARD_SPEED;
      newRightSpeed = STANDARD_SPEED;
      break;
    case ROTATE_RIGHT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = STANDARD_SPEED;
      newRightSpeed = 0 - STANDARD_SPEED;
      break;
    case REVERSE_LEFT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = 0 - INSIDE_TURN_SPEED;
      newRightSpeed = 0 - STANDARD_SPEED;
      break;
    case REVERSE_RIGHT:
      myPID.SetMode(MANUAL);
      newLeftSpeed = 0 - STANDARD_SPEED;
      newRightSpeed = 0 - INSIDE_TURN_SPEED;
      break;
  }


  // by the the time we have got here we have decided our new speeds
  setMotors(newLeftSpeed, newRightSpeed);


}

