/* the way things are wired
    0   = full speed ahead
    90  = stop
    180 = full speed astern
   yes, we wired them backwards
   to make life easier for humans, we use
    +90 = full speed ahead
    0   = stop
    -90 = full speed backwards

   this function does this mapping and sets both motors
   also constrains the numbers to usable range
*/

void setMotors(int8_t newLeftSpeed, int8_t newRightSpeed) {
  newLeftSpeed = constrain(newLeftSpeed, -90, 90);
  if (newLeftSpeed != leftSpeed) {
    leftSpeed = newLeftSpeed;
    leftTrack.write(map(leftSpeed, -90, 90, 180, 0));
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x70); // Home
    Serial2.print("     ");
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x70); // Home
    Serial2.print(leftSpeed);
#ifdef DEBUGMOTOR
    Serial.print("setting left motor to = ");
    Serial.println(leftSpeed);
#endif
  }
  newRightSpeed = constrain(newRightSpeed, -90, 90);
  if (newRightSpeed != rightSpeed) {
    rightSpeed = newRightSpeed;
    rightTrack.write(map(rightSpeed, -90, 90, 180, 0));
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x7D); // Home
    Serial2.print("     ");
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x7D); // Home
    Serial2.print(rightSpeed);
#ifdef DEBUGMOTOR
    Serial.print("setting right motor to = ");
    Serial.println(rightSpeed);
#endif
  }
}

