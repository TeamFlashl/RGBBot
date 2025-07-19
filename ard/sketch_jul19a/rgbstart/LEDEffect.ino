// Summary: bogdan pidor

#include <FastLED.h> // если используете FastLED

uint8_t ledCount = 1; // Количество светодиодов, будет установлено в LEDEffectSetup

void setupLEDEffect( int _ledCount)
{
  ledCount = _ledCount;
}

void pulse_one_color_all() {              //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < ledCount; idex++ ) {
    leds[idex] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();
  if (safeDelay(thisdelay)) return;
}

void M_color(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < ledCount; i++ ) {
    setRGB(0, i, 255);
    delay(10);
    leds[i].setRGB( cred, cgrn, cblu);
  }
  LEDS.show();
  if (safeDelay(thisdelay)) return;
}

boolean safeDelay(int delTime) {
  uint32_t thisTime = millis();
  while (millis() - thisTime <= delTime) {
    if (changeFlag) {
      changeFlag = false;
      return true;
    }
  }
  return false;
}

void M_flash() {
  static bool redState = true;       // текущее состояние: true = красный, false = синий
  static uint32_t lastChange = 0;         // время последнего переключения

  uint32_t now = millis();

  if (redState) {
    // Сейчас горим красным, ждём RED_ON_TIME мс
    if (now - lastChange >= 100) { // RED_ON_TIME
      // переключаемся на синий
      //M_Color(0, 0, 255); // или fill_solid(leds, LED_COUNT, CRGB::Blue);
      // если используете FastLED, то:
      fill_solid(leds, ledCount, CRGB::Blue);
      FastLED.show();

      redState    = false;
      lastChange  = now;
    }
  } else {
    // Сейчас горим синим, ждём BLUE_ON_TIME мс
    if (now - lastChange >= 100) { // BLUE_ON_TIME
      // переключаемся на красный
      fill_solid(leds, ledCount, CRGB::Red);
      //M_Color(255, 0, 0); // или fill_solid(leds, LED_COUNT, CRGB::Red);
      // если используете FastLED, то:
      FastLED.show();

      redState    = true;
      lastChange  = now;
    }
  }
}

/*
void rainbow_loop() {                        //-m3-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + thisstep;
  if (idex >= LED_COUNT) {
    idex = 0;
  }
  if (ihue > 255) {
    ihue = 0;
  }
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  if (safeDelay(thisdelay)) return;
}
*/
