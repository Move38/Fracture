/*
 * Fracture
 * 
 * Game rules
 * 
 * Game Featues
 * - Game board flashes in the color of the player whose turn it is
 * - Game board synchronizes color flashing showing player's progress
 * - Game board celebrates winner with entire board changing to winner color and performing a light show
 * - Game board goes to sleep after 5 minutes of no active play. Game board wakes with a single press.
 * 
 * Wanted features
 * - Game board determines the correct amount of each color on reset
 *   - i.e. 
 *   - 12 Blinks results in 4 Red, 4 Blue, 4 Yellow 
 *   - 15 Blinks results in 5 Red, 5 Blue, 5 Yellow
 *   - 16 Blinks results in 4 Red, 4 Blue, 4 Yellow, 4 Green 
 * - Game board signals ready for start of game when in the correct conformation
 *   - conformation is dependent on the number of Blinks
 * 
 */

#include "blinklib.h"
#include "blinkstate.h"

byte team = 1;
byte numTeams = 4;
bool teamDidChange = false;
bool isSatisfied = false;
int blinkDuration = 200;
int period = 1000;
int timeLastTicked = 0;

Color teamColors[] = {RED, BLUE, YELLOW, GREEN};

void setup() {

  setState(1);          // show presence
  setMetronome(period); // synchonized actions on board
}

void loop() {

  if(buttonDoubleClicked) {
    // change team
    team = team % numTeams + 1; // returns value between 1 and numTeams+1
    teamDidChange = true;
  }

  /*
   *  Update our color based on the team we are on 
   */
  if(teamDidChange) {
    setColor(teamColors[team-1];  // update display color
    teamDidChange = false;
  }

  /*
   * If neighbor changed, evaluate our state
   */
  if(neighborChanged) {

    int numNeighbors = 0;
    bool neighborOnSameTeam = false;
    for(int i=0; i<6; i++) {
      if(getNeighbor(i) != 0) {
        numNeighbors++;
      }
      if(getNeighbor(i) == team) {
        neighborOnSameTeam = true;
      }
    }

    // if number of neighbors is greater than 2 and no neighbor is on our team, we are satisfied
    if(numNeighbors >= 2 && !neighborOnSameTeam) {
      isSatisfied = true;
    }
    else {
      isSatisfied = false;
    }
  }

  /*
   *  Flash in unison with team
   */
  if(isSatisfied) { 

    if(tickDidHappen){
    
      timeLastTicked = millis();
    
    }

    // blink in a time window with our team
    int timeSinceTicked = millis() - timeLastTicked;
    int blinkStartTime = (team - 1) * period / numTeams;
    
    if(timeSinceTicked > blinkStartTime && 
       timeSinceTicked < blinkStartTime + blinkDuration) {
      
      setColor(teamColors[team-1]);  // flash team color
    } 
    else {
      setColor(OFF); // dark portion of blinking
    }
  } // end of satisfied behavior
  else {
    // show team color
    setColor(teamColors[team-1]);
  }
}
