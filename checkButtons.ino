void checkButtons() {
  // check our buttons - check the stop button last
  // code and comment pulled from tutorial
  int reading = digitalRead(rightButton);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != rightLastState) {
    // reset the debouncing timer
    rightLastDebounce = millis();
  }

  if ((millis() - rightLastDebounce) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != rightState) {
      rightState = reading;

      // only do the thing if the button is LOW
      if (rightState == LOW) {
#ifdef DEBUG
        Serial.println("right button pushed GO");
#endif
        stopped = false;
        Serial2.write(0x0C); // clear screen 
        Serial2.write(0x01); // Home
        Serial2.print("<- Push to stop");
      }
    }
  }
  rightLastState=reading;
  reading = digitalRead(leftButton);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != leftLastState) {
    // reset the debouncing timer
    leftLastDebounce = millis();
  }

  if ((millis() - leftLastDebounce) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != leftState) {
      leftState = reading;

      // only do the thing if the button is LOW
      if (leftState == LOW) {
#ifdef DEBUG
        Serial.println("left button pushed STOP");
#endif
        stopped = true;
        Serial2.write(0x0C); // clear screen 
        Serial2.write(0x01); // Home
        Serial2.print("Push to start ->");
      }
    }
  }
  leftLastState=reading;
}
