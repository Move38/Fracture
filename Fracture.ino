/*  Fracture (Game)

    Setup: For 3-6 players. Each player gets 5-6 tiles of a specific color.
           Game starts will all tiles connected, but completely segregated by color.

    Goal: Get your tiles to be touching only other players tiles.
          Your tiles must be touching at least two other tiles to feel happily diverse.

    Turns: Start from a player of choice and then go clockwise taking turns.
           A turn consists of fracturing the population into two sub populations,
           and then connecting them back together in any way you please.

    When a player has a tile fully diversified, it will blink in unison with other blinking Blinks of its color,
    first player to have all of their tiles blink wins.

    Game development by: Celia Pearce, Jeanie Choi, Isabella Carlson, Mike Lazer-Walker, Joshua Sloane

    by Jonathan Bobrow
    9.28.2017
*/

#define HAPPY_FLASH_DURATION    500
#define EDGE_FADE_DURAION       500
#define SPARKLE_OFFSET          80
#define SPARKLE_DURATION        800
#define SPARKLE_CYCLE_DURATION  2400

Color displayColor;

//Color teamColors[] = {RED, BLUE, YELLOW, GREEN};
byte teamHues[6] = {22, 49, 82, 99, 160, 200};

byte teamIndex = 0;

Timer happyFlashTimer;
bool happyFlashOn;

byte sparkleOffset[6] = {0, 3, 5, 1, 4, 2};

Timer edgeTimer[6];
bool  edgeAcquired;

bool hasRecentlySeenNeighbor[6];

void setup() {
}

void loop() {

  // change team if triple clicked
  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3) {
      teamIndex++;
      if (teamIndex >= COUNT_OF(teamHues)) {
        teamIndex = 0;
      }
    }
  }

  byte numNeighbors = 0;
  bool noNeighborsOfSameColor = true;

  // look at neighbors
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {

      if (hasRecentlySeenNeighbor[f] == false) {
        edgeAcquired = true;
        edgeTimer[f].set(EDGE_FADE_DURAION);
      }
      hasRecentlySeenNeighbor[f] = true;

      numNeighbors++;

      // if their color is the same as mine... not happy
      if (getLastValueReceivedOnFace(f) == teamIndex) {
        noNeighborsOfSameColor = false;
      }
    }
    else {
      if (hasRecentlySeenNeighbor[f] == true) {
        edgeAcquired = false;
        edgeTimer[f].set(EDGE_FADE_DURAION);
      }
      hasRecentlySeenNeighbor[f] = false;
    }
  }

  bool isHappy = false;

  // if I have two neighbors or more and my neighbors are not my color i'm happy
  if (numNeighbors >= 2 && noNeighborsOfSameColor) {
    isHappy = true;
  }

  // if I'm happy
  if (isHappy) {
    displayHappy();
  }
  else {
    displayNotHappy();
  }

  // display fracture animation or mend animation
  FOREACH_FACE(f) {
    if (!edgeTimer[f].isExpired()) {
      if (edgeAcquired) {
        // if we just gained a neighbor saturate from white
        byte sat = 255 - (255 * edgeTimer[f].getRemaining() ) / EDGE_FADE_DURAION;
        setColorOnFace(makeColorHSB(teamHues[teamIndex], sat, 255), f);
      }
      else {
        // if we just lost a neighbor fade up from dark
        byte bri = 255 - (255 * edgeTimer[f].getRemaining() ) / EDGE_FADE_DURAION;
        setColorOnFace(makeColorHSB(teamHues[teamIndex], 255, bri), f);
      }
    }
  }

  setValueSentOnAllFaces(teamIndex);
}

void displayHappy() {

  // have the color on the Blink raise and lower to feel more alive
  byte bri = 220 + 35 * sin_d( (millis() / 10) % 360); // oscillate between values 185 and 255
  setColor(dim(getColorForTeam(teamIndex), bri));

  // lets do a celebration on each face in an order
  word delta = millis() % SPARKLE_CYCLE_DURATION; // 2 second cycle

  if (delta > SPARKLE_DURATION) {
    delta = SPARKLE_DURATION;
  }

  FOREACH_FACE(f) {

    // if the face has started it's glow
    uint16_t sparkleStart = sparkleOffset[f] * SPARKLE_OFFSET;
    uint16_t sparkleEnd = sparkleStart + SPARKLE_DURATION - (6 * SPARKLE_OFFSET);

    if ( delta > sparkleStart ) {
      // minimum of 125, maximum of 255
      word phaseShift = 60 * f;
      byte amplitude = 55;
      byte midline = 185;
      byte rate = 4;
      byte lowBri = midline + (amplitude * sin_d( (phaseShift + millis() / rate) % 360) / 100);
      byte brightness;
      byte saturation;

      if ( delta < sparkleEnd ) {
        brightness = map(delta, sparkleStart, sparkleStart + SPARKLE_DURATION - (6 * SPARKLE_OFFSET), 255, lowBri);
        saturation = map(delta, sparkleStart, sparkleStart + SPARKLE_DURATION - (6 * SPARKLE_OFFSET), 0, 255);
      }
      else {
        //brightness = lowBri;
        saturation = 255;
      }

      Color faceColor = makeColorHSB(teamHues[teamIndex], saturation, 255);
      setColorOnFace(faceColor, f);
    }
  }

}

void displayNotHappy() {
  // have the color on the Blink raise and lower to feel more alive
  byte bri = 220 + 35 * sin_d( (millis() / 10) % 360); // oscillate between values 185 and 255
  setColor(dim(getColorForTeam(teamIndex), bri));
}


Color getColorForTeam(byte t) {
  return makeColorHSB(teamHues[t], 255, 255);
}


// Sin in degrees ( standard sin() takes radians )

float sin_d( uint16_t degrees ) {

  return sin( ( degrees / 360.0F ) * 2.0F * PI   );
}
