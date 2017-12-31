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

Color teamColors[] = {WHITE, RED, BLUE, YELLOW, GREEN};

enum Team {
  TEAM_RED = 1,
  TEAM_BLUE = 2,
  TEAM_YELLOW = 3,
  TEAM_GREEN = 4
};

void setup() {
  setTeam(TEAM_RED);
}

void loop() {
  // change team if triple clicked
  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3) {
      int newTeam = getState() % 4 + 1;
      setTeam(newTeam);
    }
  }

  int numNeighbors = 0;
  bool noNeighborsOfSameColor = true;
  bool isHappy = false;

  // look at neighbors
  FOREACH_FACE(f) {
    byte neighbor = getNeighborState(f);
    // count them
    if (neighbor != 0) {
      numNeighbors = numNeighbors + 1;
    }

    // if their color is the same as mine... not happy
    if (neighbor == getState()) {
      noNeighborsOfSameColor = false;
    }
  }

  // if I have two neighbors or more and my neighbors are not my color i'm happy
  if (numNeighbors >= 2 && noNeighborsOfSameColor) {
    isHappy = true;
  }


  // if I'm happy
  if (isHappy) {
    // blink in my team color
    if (millis() % 1000 > 500) {
      setColor(teamColors[getState()]);
    }
    else {
      setColor(OFF);
    }
  }
  else {
    // if I'm not happy
    // glow my team color solid
    setColor(teamColors[getState()]);
  }

}

void setTeam(int t) {
  setState(t);
  setColor(teamColors[t]);
}

