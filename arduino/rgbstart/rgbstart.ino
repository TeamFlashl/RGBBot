// ----- arduino_rgb_bot.ino -----
#include <Arduino.h>
#include <FastLED.h>          // библиотека для работы с лентой
#include "analog.h" // Подключаем файл с функциями для работы с RGB лампочками
#include "CommandHandler.h"

#define LEDS      FastLED     // опционально, чтобы писать LEDS.…
#define LED_COUNT 90          // число светодиодов
#define LED_DT    3           // пин DIN ленты

//void setRGB(int _r, int _g, int _b); // ← добавьте эту строку в начало файла


volatile int ledMode = 2;
//volatile byte ledMode = 2;
int max_bright = 10;          // максимальная яркость (0 - 255)
//struct RGB8 { uint8_t r, g, b; };
RGB8  rgbMode;

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1) // TODO ??

volatile uint32_t btnTimer;
boolean adapt_light = 0;       // адаптивная подсветка (1 - включить, 0 - выключить)

byte fav_modes[] = {2, 11, 14, 25, 27, 30, 41};  // список "любимых" режимов
byte num_modes = sizeof(fav_modes);         // получить количество "любимых" режимов (они все по 1 байту..)
unsigned long change_time, last_change, last_bright;
int new_bright;
/*
// цвета мячиков для режима
byte ballColors[3][3] = {
  {0xff, 0, 0},
  {0xff, 0xff, 0xff},
  {0   , 0   , 0xff}
};
*/
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

//int thisindex = 0;
//int thisRED = 0;
//int thisGRN = 0;
//int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR
bool debug = true;

volatile byte modeCounter;
volatile boolean changeFlag;
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
/////////////////////////////////

void setup() {
  Serial.begin(9600);              // открыть порт для связи
  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)

  fill_solid(leds, LED_COUNT, CRGB::Yellow); // Зажечь желтый
  LEDS.show();                     // отослать команду

  randomSeed(analogRead(0));
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, btnISR, FALLING);
  setupAnalog();
  setupLEDEffect(LED_COUNT); // Инициализация LED эффектов с количеством светодиодов
}

const size_t bufSize = 50;
char buf[bufSize + 1];  // +1 для завершающего нуля

void loop() { 
  setRGB(0, 0, 0);
  //Serial.println("Waiting for commands..."); 

/*
  if (Serial.available()) {     // если что то прислали
    setRGB(0, 0, 255);
    ledMode = Serial.readString().toInt();    // парсим в тип данных int
    Serial.print("Mode: ");
    Serial.println(ledMode);
    change_mode(ledMode);           // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
    //Serial.flush(); // очищаем буфер после отправки команды
  }
*/
  if (Serial.available()) {
    setRGB(0,0,255);
    Serial.setTimeout(50);
    String command = Serial.readString();
      
    setRGB(0,255,255);
    handleSerialCommands(command, &ledMode, &rgbMode);
    //setRGB(0,255,255);
  }

  switch (ledMode) {
    case 0: setColor(0, 0, 0); break;            // off 
    case 1: setColor(255, 0, 0); break; // RED
    case 2: setColor(0, 255, 0); break; // GREEN для отладки
    case 3: setColor(0, 0, 255); break; // BLUE
    case 4: setColor(255, 255, 0); break; // YELLOW
    case 5: setColor(255, 0, 255); break; // MAGENTA
    case 6: setColor(0, 255, 255); break; // CYAN
    case 7: setColor(255, 255, 255); break; // WHITE
    case 8: setColor(255, 0, 0); break; // RED
  //  case 50: pulse_one_color_all(); break;//color(255, 0, 255); break;            // цвет    
    case 128: setColor(246,118,142); break; // pink 

    case 129: setColor(255, 165, 0); break; // orange
    //case 130: M_flash(); break; 
  //  case 131: fill_solid(leds, LED_COUNT, CRGB::Cyan); break;  // Устанавливаем первый светодиод в цвет CYAN
    case 999: setColor(rgbMode.r, rgbMode.g, rgbMode.b); break;
  }
}

void btnISR() {
  if (millis() - btnTimer > 150) {
    btnTimer = millis();  // защита от дребезга
    if (++modeCounter >= num_modes) modeCounter = 0;
    //ledMode = fav_modes[modeCounter];    // получаем новый номер следующего режима
    ledMode = 8;
    change_mode(ledMode);               // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)    
    changeFlag = true;
  }
}


void change_mode(int newmode) {
  thissat = 255;
  switch (newmode) {
    case 0: 
    case 1:
    case 3: thisdelay = 20; thisstep = 10; break;       //---RAINBOW LOOP
    case 8: thisdelay = 40; thishue = 0; break;         //---POLICE LIGHTS SOLID
    case 9: thishue = 160; thissat = 50; break;         //---STRIP FLICKER
    case 35: thisdelay = 15; break;                     // Fire
    case 44: thisdelay = 100; break;                    // Strobe
  }
  bouncedirection = 0;
  ledMode = newmode;
  //Serial.println(String(newmode) + " +"); // отправляем номер режима обратно в бот
  //Serial.flush(); // очищаем буфер после отправки команды
}


