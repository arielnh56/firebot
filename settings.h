#ifndef settings_h
#define settings_h


// #define DEBUG // master debug flag. Wrap other debug defines in an #ifdef DEBUG
#ifdef DEBUG
//#define DEBUGMOTOR
// #define DEBUGFLAME
// #define DEBUGLINE
// #define DEBUGPING
// #define DEBUGFIRE  // fires co2 on setup() - caution!
#define DEBUGNAV
#endif

// digital pins
#define leftTrack_p  52
#define rightTrack_p 53
#define leftButton   28
#define rightButton  29
#define frontPing_p  A10
#define leftPing_p   A8
#define rightPing_p  A9
#define pan_p        41
#define tilt_p       42
#define co2_p        43
#define line_p       18

// analog pins
#define flame_p 7

#define FLAME_TIMER 500    // check flame every half second
#define SEE_FIRE 20  // above this - fire!
#define SEE_LINE 500 // below this - line!
#define STOP 90  // motors stop
#define CENTER 90  // pan/tilt center
#define CO2_PARK 100  // co2 parking pos
#define CO2_FIRE 70   // co2 firing pos
#define LINE_TIMER 50    // check line every 20ms

#define PING_TIMER 100   // pings every this millis
#define PING_WIDTH 28  // side to side measure our bot
#define HALL_WIDTH 46  // hallway width
#define PING_OFFCENTER -1 // mounted them off to the left, so we did
#define HALL_EXTRA_BIT 10  // allow a bit extra width in case we are squint
#define STANDARD_SPEED 30  // normal roaming speed
#define INSIDE_TURN_SPEED 0 // speed for inside track of turn
#define DEGREES 34 //milliseconds per degree of turn
#define DEGREES_ROTATE 13 //milliseconds per degree of turn
#define PID_TIMER 100   // PID sample timer
#define PID_KP 0.5      // PID Proportional
#define PID_KI 0.1     // PID Integral
#define PID_KD 0.4      // PID Derivative
#define PID_MAX 20      // PID Limit

// drive modes
#define DRIVE_STOP 0
#define DRIVE_FWD  1
#define TURN_LEFT  2
#define TURN_RIGHT 3
#define DRIVE_REVERSE 4
#define ROTATE_LEFT 5
#define ROTATE_RIGHT 6
#define REVERSE_LEFT 7
#define REVERSE_RIGHT 8

// ping stuff
#define FRONTPING 0
#define LEFTPING 1
#define RIGHTPING 2
#define SONAR_NUM     3 // Number or sensors.
#define MAX_SIDE_DISTANCE 50  // ignore pings at the side beyond this cm
#define MAX_FRONT_DISTANCE 100 // ignore pings at the front beyond this cm
#define PING_INTERVAL 33 // Milliseconds between pings.

// navigation
#define NUM_STAGES 10
char stageNames[NUM_STAGES][17] = {
  "hall 1 out      ", // HALL_1_O down the first hall and turn right
  "hall 2 out      ",  // HALL_2_O follow the left wall for 3 seconds and stop
  "room 1 in       ",  // ROOM_1_I drive forward until see room, turn right, drive forward, search for flame
  "room 1 out      ",  // ROOM_1_O back up until hit wall, turn left
  "room 2 in       ",  // ROOM_2_I drive forward, ignore hallway, drive until see room, turn right, forward till QTI
  "hall 3 out      ",  // HALL_3_O turn left, drive forward
  "room 4 in       ",  // ROOM_4_I leave room, go forward until wall, turn left, drive forward until ping senses room, turn right, QTI, look for flame
  "room 4 out      ",  // ROOM_4_O leave room, until hit wall, turn right, drive forward until cross junction, turn right, continue until wall, turn left, continue until wall, turn left, continue to finish
  "room 1 search   ",   // ROOM_1_S Dummy room search, drive in, turn, drive back out 
  "FINSIHED" 
};
#define HALL_1_O 0
#define HALL_2_O 1
#define ROOM_1_I 2
#define ROOM_1_O 3
#define ROOM_2_I 4
#define HALL_3_O 5
#define ROOM_4_I 6
#define ROOM_4_O 7
#define ROOM_1_S 8
#define FINISHED 9
#endif

