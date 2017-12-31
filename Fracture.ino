/*
   Fracture Simple - Sync

   Game rules

   Blinks blink when touching at least 2 other Blinks
   and not touching any of our own team

   Flash the team colors in sync with themselves,
   out of sync with other team colors

   Coding by
   Jonathan Bobrow (& Jamie Tsukamaki)
   12.30.17
*/

#include "blinklib.h"
#include "blinkstate.h"
#include "blinkani.h"

unsigned urge = 0;
unsigned urgeThreshold = 100;
uint32_t lastSyncTime = 0;

int numTeams = 4;
int myTeam;

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
    if (neighbor > numTeams) { // using values above num teams to pass sync info
      neighbor -= numTeams;
    }
    // count them
    if (neighbor != 0) {
      numNeighbors = numNeighbors + 1;
    }

    // if their color is the same as mine... not happy
    byte myState = getState();
    if (myState > numTeams) {
      myState -= numTeams;
    }
    if (neighbor == myState) {
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
    int blinkTiming = (urge + (myTeam * urgeThreshold / numTeams)) % urgeThreshold;
    if (blinkTiming < urgeThreshold / numTeams) {
      setColor(teamColors[myTeam]);

      if (blinkTiming < urgeThreshold / (2 * numTeams)) {
        // blink off before ring of happiness
        setColor(OFF);
      }
      else {
        // ring of happiness
        int face = 6 * numTeams * (blinkTiming - urgeThreshold / (2 * numTeams)) / (urgeThreshold / 2);
        setFaceColor(face, WHITE);
      }
    }
    else {
      setColor(teamColors[myTeam]);
    }
  }
  else {
    // if I'm not happy
    // glow my team color solid
    setColor(teamColors[myTeam]);
  }

  updateSync();
}

void setTeam(int t) {
  myTeam = t;
  setState(myTeam);
  setColor(teamColors[myTeam]);
}

void updateSync() {

  uint32_t curTime = millis();

  if (curTime - lastSyncTime > 0) {
    // The urge is building...
    urge++;

    // Until we can't take it any more!...

    if ( urge >= urgeThreshold ) {
      setState(myTeam + numTeams);  // Flash to neighbors! (actual value sent does not matter)
      //      setFaceColor(0,WHITE);
      //      setFaceColor(2,WHITE);
      //      setFaceColor(4,WHITE);
      urge = 0;
    } else {
      setState(myTeam);
    }

    // See what our neighbors are up to....
    FOREACH_FACE(face) {
      if (getNeighborState(face) > numTeams) {
        urge += (urge / 10);
      }
    }
    lastSyncTime = curTime;
  }
}
