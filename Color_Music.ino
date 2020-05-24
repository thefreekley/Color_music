// UZI_SQUAD COLOR MUSIC
#define NUM_LEDS 60 // кількість світлодіодів
#define BRIGHTNESS 200 // яскравість (0 - 255)

// Піни
#define SOUND_R A2 // аналоговий пін вхід аудіо, правий канал
#define SOUND_L A1 // аналоговий пін вхід аудіо, лівий канал
#define SOUND_R_FREQ A3 // аналоговий пін вхід аудіо для режиму з частотами (через кондер)
#define BTN_PIN 3 // кнопка перемикання режимів (PIN --- КНОПКА --- GND)
#define LED_PIN 12 // пін DI світлодіодної стрічки
#define POT_GND A0 // пін земля для потенціометра

// настройки веселки
#define RAINBOW_SPEED 6 // швидкість руху веселки (чим менше число, тим швидше веселка)
#define RAINBOW_STEP 6 // крок зміни кольору веселки

// отрисовка
#define MODE 0 // режим при запуску
#define MAIN_LOOP 5 // період основного циклу відтворення (за замовчуванням 5)
#define SMOOTH 0.5 // коефіцієнт плавності анімації VU (за замовчуванням 0.5)
#define SMOOTH_FREQ 0.8 // коефіцієнт плавності анімації частот (за замовчуванням 0.8)
#define MAX_COEF 1.8 // коефіцієнт гучності (максимальне одно срднему * цей коеф) (за замовчуванням 1.8)
#define MAX_COEF_FREQ 1.2 // коефіцієнт порогу для "спалаху" світломузики (за замовчуванням 1.5)

// сигнал
#define MONO 1 // 1 - тільки один канал (ПРАВИЙ !!!!! SOUND_R !!!!!), 0 - два канали
#define EXP 1.4 // ступінь посилення сигналу 
#define POTENT 1 // 1 - використовуємо потенціометр, 0 - використовується внутрішній джерело опорного напруги 1.1 В

// нижній поріг шумів
int LOW_PASS = 100; // нижній поріг шумів режим VU, ручна настройка
int SPEKTR_LOW_PASS = 40; // нижній поріг шумів режим спектра, ручна настройка
#define AUTO_LOW_PASS 0 // дозволити настройку нижнього порога шумів при запуску (Стандарт. 0)
#define EEPROM_LOW_PASS 1 // поріг шумів зберігається в незалежній пам'яті (Стандарт. 1)
#define LOW_PASS_ADD 13 // "додаткова" величина до нижнього порогу, для надійності (режим VU)
#define LOW_PASS_FREQ_ADD 3 // "додаткова" величина до нижнього порогу, для надійності (режим частот)

// режим світломузики
#define SMOOTH_STEP 20 // крок зменшення яскравості в режимі світломузики (чим більше, тим швидше гасне)
#define LOW_COLOR HUE_RED // колір низьких частот
#define MID_COLOR HUE_GREEN // колір середніх
#define HIGH_COLOR HUE_YELLOW // колір високих

// режим стробоскопа
#define STROBE_PERIOD 100 // період спалахів, мілісекунди
#define STROBE_DUTY 30 // шпаруватість спалахів (1 - 99) - відношення часу спалаху до часу темряви
#define STROBE_COLOR HUE_YELLOW // колір стробоскопа
#define STROBE_SAT 0 // насиченість. Якщо 0 - колір буде БІЛИЙ при будь-якому кольорі (0 - 255)
#define STROBE_SMOOTH 100 // швидкість наростання / згасання спалаху (0 - 255)

/ *
  Кольори для HSV
  HUE_RED
  HUE_ORANGE
  HUE_YELLOW
  HUE_GREEN
  HUE_AQUA
  HUE_BLUE
  HUE_PURPLE
  HUE_PINK
* /
// --------------------------- НАЛАШТУВАННЯ -------------------- -------

// --------------------- ДЛЯ РОЗРОБНИКІВ ---------------------
#define MODE_AMOUNT 6 // кількість режимів

// кольору (застарілі)
#define BLUE 0x0000FF
#define RED 0xFF0000
#define GREEN 0x00ff00
#define CYAN 0x00FFFF
#define MAGENTA 0xFF00FF
#define YELLOW 0xFFFF00
#define WHITE 0xFFFFFF
#define BLACK 0x000000

#define STRIPE NUM_LEDS / 5

#define FHT_N 64 // ширина спектра х2
#define LOG_OUT 1
#include <FHT.h> // перетворення Хартлі
#include <EEPROMex.h>

#include "FastLED.h"
CRGB leds [NUM_LEDS];

#include "GyverButton.h"
GButton butt1 (BTN_PIN);

// градієнт-палітра від зеленого до червоного
DEFINE_GRADIENT_PALETTE (soundlevel_gp) {
  0, 0, 255, 0, // green
  100, 255, 255, 0, // yellow
  150, 255, 100, 0, // orange
  200, 255, 50, 0, // red
  255, 255, 0, 0 // red
};
CRGBPalette32 myPal = soundlevel_gp;

byte Rlenght, Llenght;
float RsoundLevel, RsoundLevel_f;
float LsoundLevel, LsoundLevel_f;

float averageLevel = 50;
int maxLevel = 100;
byte MAX_CH = NUM_LEDS / 2;
int hue;
unsigned long main_timer, hue_timer, strobe_timer;
float averK = 0.006, k = SMOOTH, k_freq = SMOOTH_FREQ;
byte count;
float index = (float) 255 / MAX_CH; // коефіцієнт перекладу для палітри
boolean lowFlag;
byte low_pass;
int RcurrentLevel, LcurrentLevel;
int colorMusic [3];
float colorMusic_f [3], colorMusic_aver [3];
boolean colorMusicFlash [3], strobeUp_flag, strobeDwn_flag;
byte this_mode = MODE;
int thisBright [3], strobe_bright = 0;
unsigned int light_time = STROBE_PERIOD * STROBE_DUTY / 100;

#define cbi (sfr, bit) (_SFR_BYTE (sfr) & = ~ _BV (bit))
#define sbi (sfr, bit) (_SFR_BYTE (sfr) | = _BV (bit))
// --------------------- ДЛЯ РОЗРОБНИКІВ ---------------------

void setup () {
  Serial.begin (9600);
  FastLED.addLeds <WS2811, LED_PIN, GRB> (leds, NUM_LEDS) .setCorrection (TypicalLEDStrip);
  FastLED.setBrightness (BRIGHTNESS);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

 pinMode (POT_GND, OUTPUT);
  digitalWrite (POT_GND, LOW);
  butt1.setTimeout (900);

  // для збільшення точності зменшуємо опорна напруга,
  // виставивши EXTERNAL і підключивши Aref до виходу 3.3V на платі через дільник
  // GND --- [10-20 кОм] --- REF --- [10 кОм] --- 3V3
  // в даній схемі GND береться з А0 для зручності підключення
  if (POTENT) analogReference (EXTERNAL);
  else analogReference (INTERNAL);

  // моторошна магія, міняємо частоту оцифровки до 18 кГц
  // команди на ебучій асемблері, навіть не питайте, як це працює
  sbi (ADCSRA, ADPS2);
  cbi (ADCSRA, ADPS1);
  sbi (ADCSRA, ADPS0);

  if (AUTO_LOW_PASS &&! EEPROM_LOW_PASS) {// якщо дозволена автонастройка нижнього порога шумів
    autoLowPass ();
  }
  if (EEPROM_LOW_PASS) {// відновити значення шумів з пам'яті
    LOW_PASS = EEPROM.readInt (0);
    SPEKTR_LOW_PASS = EEPROM.readInt (2);
  }
}

void loop () {
  butt1.tick (); // обов'язкова функція відпрацювання. Повинна постійно опрашиваться
  if (butt1.isSingle ()) // якщо одиничне натискання
    if (++ this_mode> = MODE_AMOUNT) this_mode = 0; // змінити режим

  if (butt1.isHolded ()) {// кнопка утримана
    digitalWrite (13, HIGH); // включити світлодіод 13 пін
    FastLED.setBrightness (0); // погасити стрічку
    FastLED.clear (); // очистити масив пікселів
    FastLED.show (); // відправити значення на стрічку
    delay (500); // почекати чуйна
    autoLowPass (); // виміряти шуми
    delay (500); // почекати
    FastLED.setBrightness (BRIGHTNESS); // повернути яскравість
    digitalWrite (13, LOW); // вимкнути світлодіод
  }

  // кільцеве зміна положення веселки за таймером
  if (millis () - hue_timer> RAINBOW_SPEED) {
    if (++ hue> = 255) hue = 0;
    hue_timer = millis ();
  }

  // головний цикл відтворення
  if (millis () - main_timer> MAIN_LOOP) {
    // скидаємо значення
    RsoundLevel = 0;
    LsoundLevel = 0;

    // первай два режими - гучність (VU meter)
    if (this_mode == 0 || this_mode == 1) {
      for (byte i = 0; i <100; i ++) {// робимо 100 вимірювань
        RcurrentLevel = analogRead (SOUND_R); // з правого
        if (! MONO) LcurrentLevel = analogRead (SOUND_L); // і лівого каналів

        if (RsoundLevel <RcurrentLevel) RsoundLevel = RcurrentLevel; // шукаємо максимальну
        if (! MONO) if (LsoundLevel <LcurrentLevel) LsoundLevel = LcurrentLevel; // шукаємо максимальну
      }

      // фільтруємо по нижньому порогу шумів
      RsoundLevel = map (RsoundLevel, LOW_PASS, 1023, 0, 500);
      if (! MONO) LsoundLevel = map (LsoundLevel, LOW_PASS, 1023, 0, 500);

      // обмежуємо діапазон
      RsoundLevel = constrain (RsoundLevel, 0, 500);
      if (! MONO) LsoundLevel = constrain (LsoundLevel, 0, 500);

      // зводимо в ступінь (для більшої чіткості роботи)
      RsoundLevel = pow (RsoundLevel, EXP);
      if (! MONO) LsoundLevel = pow (LsoundLevel, EXP);

      // фільтр
      RsoundLevel_f = RsoundLevel * k + RsoundLevel_f * (1 - k);
      if (! MONO) LsoundLevel_f = LsoundLevel * k + LsoundLevel_f * (1 - k);

      if (MONO) LsoundLevel_f = RsoundLevel_f; // якщо моно, то лівий = правому

      // якщо значення вище порога - починаємо найцікавіше
      if (RsoundLevel_f> 15 && LsoundLevel_f> 15) {

        // розрахунок загальної середньої гучності з обох каналів, фільтрація.
        // Фільтр дуже повільний, зроблено спеціально для Автогучність
        averageLevel = (float) (RsoundLevel_f + LsoundLevel_f) / 2 * averK + averageLevel * (1 - averK);

        // беремо максимальну гучність шкали як середню, помножену на деякий коефіцієнт MAX_COEF
        maxLevel = (float) averageLevel * MAX_COEF;

        // перетворимо сигнал в довжину стрічки (де MAX_CH це половина кількості світлодіодів)
        Rlenght = map (RsoundLevel_f, 0, maxLevel, 0, MAX_CH);
        Llenght = map (LsoundLevel_f, 0, maxLevel, 0, MAX_CH);

        // обмежуємо до макс. числа світлодіодів
        Rlenght = constrain (Rlenght, 0, MAX_CH);
        Llenght = constrain (Llenght, 0, MAX_CH);

        animation (); // отрисовать
      }
    }

    // 3-5 режим - світломузика
    if (this_mode == 2 || this_mode == 3 || this_mode == 4) {
      analyzeAudio ();
      colorMusic [0] = 0;
      colorMusic [1] = 0;
      colorMusic [2] = 0;
      // низькі частоти, вибірка з 3 по 5 тон
      for (byte i = 3; i <6; i ++) {
        if (fht_log_out [i]> SPEKTR_LOW_PASS) {
          if (fht_log_out [i]> colorMusic [0]) colorMusic [0] = fht_log_out [i];
        }
      }
      // середні частоти, вибірка з 6 по 10 тон
      for (byte i = 6; i <11; i ++) {
        if (fht_log_out [i]> SPEKTR_LOW_PASS) {
          if (fht_log_out [i]> colorMusic [1]) colorMusic [1] = fht_log_out [i];
        }
      }
      // високі частоти, вибірка з 11 по 30 тон
      for (byte i = 11; i <31; i ++) {
        if (fht_log_out [i]> SPEKTR_LOW_PASS) {if (fht_log_out [i]> colorMusic [2]) colorMusic [2] = fht_log_out [i];
        }
      }
      for (byte i = 0; i <3; i ++) {
        colorMusic_aver [i] = colorMusic [i] * averK + colorMusic_aver [i] * (1 - averK); // загальна фільтрація
        colorMusic_f [i] = colorMusic [i] * k_freq + colorMusic_f [i] * (1 - k_freq); // локальна
        if (colorMusic_f [i]> ((float) colorMusic_aver [i] * MAX_COEF_FREQ)) {
          thisBright [i] = 255;
          colorMusicFlash [i] = 1;
        } Else colorMusicFlash [i] = 0;
        if (thisBright [i]> = 0) thisBright [i] - = SMOOTH_STEP;
        if (thisBright [i] <0) thisBright [i] = 0;
      }
      animation ();
    }
    if (this_mode == 5) {
      if ((long) millis () - strobe_timer> STROBE_PERIOD) {
        strobe_timer = millis ();
        strobeUp_flag = true;
        strobeDwn_flag = false;
      }
      if ((long) millis () - strobe_timer> light_time) {
        strobeDwn_flag = true;
      }
      if (strobeUp_flag) {// якщо настав час пахкати
        if (strobe_bright <255) // якщо яскравість не максимально
          strobe_bright + = STROBE_SMOOTH; // увелчіть
        if (strobe_bright> 255) {// якщо пробили макс. яскравість
          strobe_bright = 255; // залишити максимум
          strobeUp_flag = false; // прапор опустити
        }
      }
      
      if (strobeDwn_flag) {// ГАСН
        if (strobe_bright> 0) // якщо яскравість не мінімальний
          strobe_bright - = STROBE_SMOOTH; // зменшити
        if (strobe_bright <0) {// якщо пробили хв. яскравість
          strobeDwn_flag = false;
          strobe_bright = 0; // залишити 0
        }
      }
      animation ();
    }

    FastLED.show (); // відправити значення на стрічку
    FastLED.clear (); // очистити масив пікселів
    main_timer = millis (); // скинути таймер
  }
}

void animation () {
  // згідно з режимом
  switch (this_mode) {
    case 0:
      count = 0;
      for (int i = (MAX_CH - 1); i> ((MAX_CH - 1) - Rlenght); i--) {
        leds [i] = ColorFromPalette (myPal, (count * index)); // заливка по палітрі "від зеленого до червоного"
        count ++;
      }
      count = 0;
      for (int i = (MAX_CH); i <(MAX_CH + Llenght); i ++) {
        leds [i] = ColorFromPalette (myPal, (count * index)); // заливка по палітрі "від зеленого до червоного"
        count ++;
      }
      break;
    case 1:
      count = 0;
      for (int i = (MAX_CH - 1); i> ((MAX_CH - 1) - Rlenght); i--) {
        leds [i] = ColorFromPalette (RainbowColors_p, (count * index) / 2 - hue); // заливка по палітрі веселка
        count ++;
      }
      count = 0;
      for (int i = (MAX_CH); i <(MAX_CH + Llenght); i ++) {
        leds [i] = ColorFromPalette (RainbowColors_p, (count * index) / 2 - hue); // заливка по палітрі веселка
        count ++;
      }
      break;
    case 2:
      for (int i = 0; i <NUM_LEDS; i ++) {
        if (i <STRIPE) leds [i] = CHSV (HIGH_COLOR, 255, thisBright [2]);
        else if (i <STRIPE * 2) leds [i] = CHSV (MID_COLOR, 255, thisBright [1]);
        else if (i <STRIPE * 3) leds [i] = CHSV (LOW_COLOR, 255, thisBright [0]);
        else if (i <STRIPE * 4) leds [i] = CHSV (MID_COLOR, 255, thisBright [1]);
        else if (i <STRIPE * 5) leds [i] = CHSV (HIGH_COLOR, 255, thisBright [2]);
      }
      break;
    case 3:
      for (int i = 0; i <NUM_LEDS; i ++) {
        if (i <NUM_LEDS / 3) leds [i] = CHSV (HIGH_COLOR, 255, thisBright [2]);
        else if (i <NUM_LEDS * 2/3) leds [i] = CHSV (MID_COLOR, 255, thisBright [1]);
        else if (i <NUM_LEDS) leds [i] = CHSV (LOW_COLOR, 255, thisBright [0]);
      }
      break;
    case 4:
      if (colorMusicFlash [2]) for (int i = 0; i <NUM_LEDS; i ++) leds [i] = CHSV (HIGH_COLOR, 255, thisBright [2]);
      else if (colorMusicFlash [1]) for (int i = 0; i <NUM_LEDS; i ++) leds [i] = CHSV (MID_COLOR, 255, thisBright [1]);
      else if (colorMusicFlash [0]) for (int i = 0; i <NUM_LEDS; i ++) leds [i] = CHSV (LOW_COLOR, 255, thisBright [0]);
      else for (int i = 0; i <NUM_LEDS; i ++) leds [i] = BLACK;
      break;
    case 5:
      for (int i = 0; i <NUM_LEDS; i ++) leds [i] = CHSV (STROBE_COLOR, STROBE_SAT, strobe_bright);
      break;
  }
}

void autoLowPass () {
  // для режиму VU
  delay (10); // чекаємо ініціалізації АЦП
  int thisMax = 0; // максимум
  int thisLevel;
  for (byte i = 0; i <200; i ++) {
    thisLevel = analogRead (SOUND_R); // робимо 200 вимірювань
    if (thisLevel> thisMax) // шукаємо максимуми
      thisMax = thisLevel; // запам'ятовуємо
    delay (4); // чекаємо 4мс
  }
  LOW_PASS = thisMax + LOW_PASS_ADD; // нижній поріг як максимум тиші + якась величина
  // для режима спектра
 thisMax = 0;
   for (byte i = 0; i <100; i ++) {// робимо 100 вимірювань
     analyzeAudio (); // розбити в спектр
     for (byte j = 2; j <32; j ++) {// перші 2 канали - мотлох
       thisLevel = fht_log_out [j];
       if (thisLevel> thisMax) // шукаємо максимуми
         thisMax = thisLevel; // запам'ятовуємо
     }
     delay (4); // чекаємо 4мс
   }
   SPEKTR_LOW_PASS = thisMax + LOW_PASS_FREQ_ADD; // нижній поріг як максимум тиші

   if (EEPROM_LOW_PASS &&! AUTO_LOW_PASS) {
     EEPROM.updateInt (0, LOW_PASS);
     EEPROM.updateInt (2, SPEKTR_LOW_PASS);
   }
}
void analyzeAudio() {
  for (int i = 0 ; i < FHT_N ; i++) {
    int sample = analogRead(SOUND_R_FREQ);
    fht_input[i] = sample; // put real data into bins
  }
  fht_window();  // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run();     // process the data in the fht
  fht_mag_log(); // take the output of the fht
}
