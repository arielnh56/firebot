#include <PID_v1.h>
#include "settings.h"

double lastDrift = 0;
double drift() {
  // while moving in a straight line, how far off are we from optimal path
  if ( range[LEFTPING] == 0 && range[RIGHTPING] == 0 ) { // no reference - assume we are on track
    return 0;  
  } else if ( range[LEFTPING] > 0 && range[RIGHTPING] == 0 ) { // wall to my left
    // center of bot = ping + half a bot
    // center of hall = half a hall
    return range[LEFTPING] - (HALL_WIDTH - PING_WIDTH)/2.0;  
  } else if ( range[RIGHTPING] > 0 && range[LEFTPING] == 0 ) { // wall to my left
    return range[RIGHTPING] - (HALL_WIDTH - PING_WIDTH)/2.0;  
  } else { // walls on both sides
    return range[LEFTPING] - range[RIGHTPING];
  }
}



