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

Color displayColor;

Color teamColors[] = {RED, BLUE, YELLOW, GREEN};

byte teamIndex = 2;

Timer happyFlashTimer;
bool happyFlashOn;



// Sin in degrees ( standard sin() takes radians )

float sin_d( uint16_t degrees ) {

    return sin( ( degrees / 360.0F ) * 2.0F * PI   );
}


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
    
      // if their color is the same as mine... not happy
      if (getLastValueReceivedOnFace(f) == teamIndex) {
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
  if (isHappy) {

    if(happyFlashTimer.isExpired()) {
      happyFlashOn = !happyFlashOn;
      
      happyFlashTimer.set(HAPPY_FLASH_DURATION);
    }
    
    if(!happyFlashOn) {
      isOn = false;
    }
  }

  if(isOn) {
    setColor(dim(teamColors[teamIndex], 220 + 35 * sin_d( (millis()/10) % 360)));
  }
  else {
    setColor(OFF);
  }

  setValueSentOnAllFaces(teamIndex);
}


