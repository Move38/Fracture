/*
   Fracture Simple

   Game rules

   Blinks blink when touching at least 2 other Blinks
   and not touching any of our own team

   Coding by
   Jonathan Bobrow (& Jamie Tsukamaki)
   2.1.18
*/

#define HAPPY_FLASH_DURATION 500
#define URGE_THRESHOLD       100
#define NUM_TEAMS            4

uint16_t urge = 0;
uint32_t lastSyncTime = 0;

Color displayColor;

Color teamColors[] = {RED, BLUE, YELLOW, GREEN};

byte teamIndex = 0;

Timer happyFlashTimer;
bool happyFlashOn;

void setup() {
}

void loop() {
  
  // change team if triple clicked
  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3) {
      teamIndex++;
      if(teamIndex>= COUNT_OF(teamColors)) {
        teamIndex = 0;      
      }
    }
  }

  byte numNeighbors = 0;
  bool noNeighborsOfSameColor = true;

  // look at neighbors
  FOREACH_FACE(f) {
    if(!isValueReceivedOnFaceExpired(f)) {
      
      numNeighbors++;

      byte neighborValue = getLastValueReceivedOnFace(f);

      // reduce to team index
      while(neighborValue > NUM_TEAMS) {
        neighborValue -= NUM_TEAMS;
      }
      
      // if their color is the same as mine... not happy
      if (neighborValue == teamIndex) {
        noNeighborsOfSameColor = false;
      }
    }
  }

  bool isHappy = false;
  
  // if I have two neighbors or more and my neighbors are not my color i'm happy
  if (numNeighbors >= 2 && noNeighborsOfSameColor) {
    isHappy = true;
  }

  bool isOn = true;
  
  // if I'm happy
//  if (isHappy) {
//
//    if(happyFlashTimer.isExpired()) {
//      happyFlashOn = !happyFlashOn;
//      
//      happyFlashTimer.set(HAPPY_FLASH_DURATION);
//    }
//    
//    if(!happyFlashOn) {
//      isOn = false;
//    }
//  }
//
//  if(isOn) {
//    setColor(teamColors[teamIndex]);
//  }
//  else {
//    setColor(OFF);
//  }

// if I'm happy
  if (isHappy) {
    // blink in my team color
    int blinkTiming = (urge + (teamIndex * URGE_THRESHOLD / NUM_TEAMS)) % URGE_THRESHOLD;
    if (blinkTiming < URGE_THRESHOLD / NUM_TEAMS) {
      setColor(teamColors[teamIndex]);

      if (blinkTiming < URGE_THRESHOLD / (2 * NUM_TEAMS)) {
        // blink off before ring of happiness
        setColor(OFF);
      }
      else {
        // ring of happiness
        int face = 6 * NUM_TEAMS * (blinkTiming - URGE_THRESHOLD / (2 * NUM_TEAMS)) / (URGE_THRESHOLD / 2);
        setFaceColor(face, WHITE);
      }
    }
    else {
      setColor(teamColors[teamIndex]);
    }
  }
  else {
    // if I'm not happy
    // glow my team color solid
    setColor(teamColors[teamIndex]);
  }
  
  setValueSentOnAllFaces(teamIndex);

  updateSync();
}

void updateSync() {

  uint32_t curTime = millis();

  if (curTime - lastSyncTime > 0) {
    // The urge is building...
    urge++;

    // Until we can't take it any more!...

    if ( urge >= URGE_THRESHOLD ) {
      setValueSentOnAllFaces(teamIndex + NUM_TEAMS);  // Flash to neighbors! (actual value sent does not matter)
      //      setFaceColor(0,WHITE);
      //      setFaceColor(2,WHITE);
      //      setFaceColor(4,WHITE);
      urge = 0;
    } else {
      setValueSentOnAllFaces(teamIndex);
    }

    // See what our neighbors are up to....
    FOREACH_FACE(face) {
      if(!isValueReceivedOnFaceExpired(face)) {
        if (getLastValueReceivedOnFace(face) > NUM_TEAMS) {
          urge += (urge / 10);
        }
      }
    }
    lastSyncTime = curTime;
  }
}
