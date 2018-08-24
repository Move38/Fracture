byte gameState;
byte team;
byte teamHues[] = {22, 49, 82, 99};
enum fractureStates {NOMINAL, FRACTURED, RESOLVING};
bool neighborStates[6];
//animation variables
Timer animTimer;
byte saturation = 255;
byte brightness = 255;
Timer happyTimer;
bool flashOn = true;
void setup() {
  gameState = NOMINAL;
  setColor(makeColorHSB(teamHues[team], 255, 255));
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      neighborStates[f] = true;
    } else {
      neighborStates[f] = false;
    }
  }
}
void loop() {
  //first, we need to listen for double clicks to change team
  if (buttonDoubleClicked()) {
    team++;
    if (team > 3) {
      team = 0;
    }
    setColor(makeColorHSB(teamHues[team], 255, 255));
  }
  //determine which state I'm in, send to appropriate loop behavior
  switch (gameState) {
    case NOMINAL:
      nominalLoop();
      break;
    case FRACTURED:
      fracturedLoop();
      break;
    case RESOLVING:
      resolvingLoop();
      break;
  }
  //this is where you update your face list
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      neighborStates[f] = true;
    } else {
      neighborStates[f] = false;
    }
  }
  //last, we want to resolve any flashes that have been initiated
  if (animTimer.isExpired() && (saturation != 255 || brightness != 255)) {
    if (saturation != 255) {
      saturation += 17;
    }
    if (brightness != 255) {
      brightness += 17;
    }
    animTimer.set(50);
    setColor(makeColorHSB(teamHues[team], saturation, brightness));
  }
  if (happyTimer.isExpired()) {
    if (saturation == 255 && brightness == 255) { //this only happens if no other flashes are happening
      if (happinessCheck()) {
        if (flashOn) {
          setColor(OFF);
          flashOn = false;
        } else {
          setColor(makeColorHSB(teamHues[team], 255, 255));
          flashOn = true;
        }
      }
    }
    happyTimer.set(500);
  }
  setValueSentOnAllFaces((gameState * 10) + team);
}
void nominalLoop() {
  //check surroundings for MISSING NEIGHBORS or neighbors already in distress
  FOREACH_FACE(f) {
    if (isValueReceivedOnFaceExpired(f) && neighborStates[f] == true) { //missing neighbor
      gameState = FRACTURED;
    } else if (!isValueReceivedOnFaceExpired(f) && getLastValueReceivedOnFace(f) / 10 == FRACTURED) { //distressed neighbor
      gameState = FRACTURED;
    }
  }
  if (gameState == FRACTURED) {
    //begin the off flash, change the state
    setColor(OFF);
    brightness = 0;
  }
}
void fracturedLoop() {
  //first we do the flash animation
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f) && neighborStates[f] == false) { //new neighbor
      gameState = RESOLVING;
    } else if (!isValueReceivedOnFaceExpired(f) && getLastValueReceivedOnFace(f) / 10 == RESOLVING) { //resolving neighbor
      gameState = RESOLVING;
    }
  }
  if (gameState == RESOLVING) {
    setColor(WHITE);
    saturation = 0;
  }
}
void resolvingLoop() {
  gameState = NOMINAL;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f) && getLastValueReceivedOnFace(f) / 10 == FRACTURED) {
      gameState = RESOLVING;
    }
  }
  if (gameState == NOMINAL) {
    setColor(makeColorHSB(teamHues[team], 255, 255));
  }
}
bool happinessCheck() {
  bool isHappy = true;
  byte numNeighbors = 0;
  FOREACH_FACE(f) {
    byte neighbor = getLastValueReceivedOnFace(f) % 10;
    if (!isValueReceivedOnFaceExpired(f)) { //this means there is something there
      numNeighbors++;
      if (neighbor == team) { //this means this neighbor is the same color as us. Oh no!
        isHappy = false;
      }
    }
  }
  if (numNeighbors < 2) { //if numNeighbors never reached 2, it's not happy
    isHappy = false;
  }
  return isHappy;
}
