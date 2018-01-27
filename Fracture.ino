/*
   Fracture Simple

   Game rules

   Blinks blink when touching at least 2 other Blinks
   and not touching any of our own team

   Coding by
   Jonathan Bobrow (& Jamie Tsukamaki)
   12.30.17
*/

#include "blinklib.h"
#include "blinkstate.h"

Color displayColor;

Color teamColors[] = {RED, BLUE, YELLOW, GREEN};

byte teamIndex = 0;

void setup() {
  blinkStateBegin();
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
    if(!isNeighborExpired(f)) {
      
      numNeighbors++;
    
      // if their color is the same as mine... not happy
      if (getNeighborState(f) == teamIndex) {
        noNeighborsOfSameColor = false;
      }
    }
  }

  bool isHappy = false;
  
  // if I have two neighbors or more and my neighbors are not my color i'm happy
  if (numNeighbors >= 2 && noNeighborsOfSameColor) {
    isHappy = true;
  }

  // if I'm happy
  if (isHappy) {
    // blink in my team color
    if (millis() % 1000 > 500) {
      setColor(teamColors[teamIndex]);
    }
    else {
      setColor(OFF);
    }
  }
  else {
    // if I'm not happy
    // glow my team color solid
    setColor(teamColors[teamIndex]);
  }

  setState(teamIndex);
}
