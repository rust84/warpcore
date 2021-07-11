#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#include <Adafruit_NeoPixel.h>
#define FASTLED_INTERNAL
#include "FastLED.h"


#define DEBUG



/*=========================================================================
    APPLICATION SETTINGS

      FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
     
                                Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                                running this at least once is a good idea.
     
                                When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                                Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
         
                                Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    PIN                       Which pin on the Arduino is connected to the NeoPixels?
    NUMPIXELS                 How many NeoPixels are attached to the Arduino?
    -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE     0
#define CW 1
#define CCW 0
#define ON 1
#define OFF 0
/*=========================================================================*/

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 6
#define MODE_PIN 11
//#define CLOCK_PIN 9
#define SerialSpeed 9600

// How many LEDs in your strip?
#define NUM_LEDS 33    // Total number of LEDs
#define NUMPIXELS 33

// How are the LEDs distributed?
#define SegmentSize 4    // How many LEDs in each "Magnetic Constrictor" segment
#define TopLEDcount 14    // LEDs above the "Reaction Chamber"
#define ReactionLEDcount 2  // LEDs inside the "Reaction Chamber"
#define BottomLEDcount 17	// LEDs below the "Reaction Chamber"

// Default Settings
#define DefaultWarpFactor 1	// 1-9
#define DefaultMainHue 160	// 1-255	1=Red 32=Orange 64=Yellow 96=Green 128=Aqua 160=Blue 192=Purple 224=Pink 255=Red
#define DefaultSaturation 255	// 1-255
#define DefaultBrightness 255	// 1-255
#define DefaultPattern 1	// 1-5		1=Standard 2=Breach 3=Rainbow 4=Fade 5=Slow Fade


// Initialize internal variables
#define PulseLength SegmentSize*2
#define TopChases TopLEDcount/PulseLength+1*PulseLength
#define TopLEDtotal TopLEDcount+ReactionLEDcount
#define TopDiff TopLEDcount-BottomLEDcount
#define RateMultiplier 6
byte MainHue = DefaultMainHue;
byte ReactorHue = DefaultMainHue;
byte LastHue = DefaultMainHue;
byte WarpFactor = DefaultWarpFactor;
byte LastWarpFactor = DefaultWarpFactor;
byte Rate = RateMultiplier * WarpFactor;
byte Saturation = DefaultSaturation;
byte Brightness = DefaultBrightness;
byte Pattern = DefaultPattern;
byte Pulse;
boolean Rainbow = false;
boolean Fade = false;
boolean SlowFade = false;// Default Settings

// Serial input variables
const byte numChars = 20;
char receivedChars[numChars];
char tempChars[numChars];	// temporary array for use when parsing

// Parsing variables
byte warp_factor = WarpFactor;
byte hue = MainHue;
byte saturation = Saturation;
byte brightness = Brightness;
byte pattern = Pattern;

bool newData = false;

uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;
uint8_t animationState = 1;

byte oldSwitchState = HIGH;  // assume switch open because of pull-up resistor
const unsigned long debounceTime = 10;  // milliseconds
unsigned long switchPressTime;  // when the switch last changed state

int pos = 0, dir = 1; // Position, direction of "eye" for larson scanner animation
int midPixel = NUMPIXELS / 2;
int pixarray[NUMPIXELS][4] = {{0}}; //This is used in several functions to manipulate multiple pixels at once.
/*
int randr1;
int randr2;
int randg1;
int randg2;
int randb1;
int randb2;
int randrs;
int randgs;
int randbs;
*/
int SOLIDCOLOR = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 6);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
#include "C:\Users\russell.hall\Documents\Arduino\NeoPixel\mypixels\mypixels.ino"

// Define the array of LEDarray
CRGB LEDarray[NUM_LEDS];
// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];
byte animsequence[]={0,1,2,3,4,7,8,'A','B','C','D','E','F','L','O','P','R','S','W'};
byte animindex=0;
int numanims=18;

void setup() {
  delay(2000); // 2 second delay for recovery
  pinMode (MODE_PIN, INPUT_PULLUP);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(LEDarray, NUM_LEDS);
  //FastLED.setCorrection( CRGB( 255, 200, 245) );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500);
  FastLED.setBrightness(Brightness);

  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // off
  }
  pixels.show();
  colorWipe(pixels.Color(180, 180, 180), 20);
  delay(1000);
  colorWipe(pixels.Color(0, 0, 250), 20);
  delay(1000);
//  colorWipe(pixels.Color(0, 50, 250), 15);
//  loadpixarray();
//  delay(2000);
  setall(1);
  showpixarray();
  pixels.show();
  spookydownpixarray(30);
  colorWipe(pixels.Color(0, 0, 0), 15);
  pixels.show();
  wipepixarray();

  Serial.begin(SerialSpeed);
#ifdef DEBUG  
  Serial.println(F("Adafruit Bluefruit Neopixel Color Picker Example"));
  Serial.println(F("------------------------------------------------"));
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));
#endif  
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
#ifdef DEBUG  
  Serial.println( F("OK!") );
#endif
  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }
  /* Disable command echo from Bluefruit */
  ble.echo(false);
#ifdef DEBUG  
  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();
#endif  
  ble.verbose(false);  // debug info is a little annoying after this point!
  /* Wait for connection */
/*
  while (! ble.isConnected()) {
    delay(500);
  }
*/
#ifdef DEBUG
  Serial.println(F("***********************"));
  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
#endif
  ble.setMode(BLUEFRUIT_MODE_DATA);
#ifdef DEBUG
  Serial.println(F("***********************"));
#endif
  //  strcpy(tempChars, "<2, 160, 220, 255, 1>");
  //  updateSettings();
  PrintInfo();
}

void loop() {
  boolean pressed = 0;

  /* Wait for new data to arrive */
  //  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  uint8_t len = readPacket(&ble, 5);
  //  if (len == 0) return;

 byte switchState = digitalRead (MODE_PIN);
  // has it changed since last time?
  if (switchState != oldSwitchState){
    // debounce
    if (millis () - switchPressTime >= debounceTime)
       {
       switchPressTime = millis ();  // when we closed the switch 
       oldSwitchState =  switchState;  // remember for next time 
       if (switchState == LOW){
#ifdef DEBUG
          Serial.println ("Switch closed.");
#endif
          pressed=0;
          SOLIDCOLOR=0;
          len=255;
          if(animindex>numanims) animindex=0;
          animationState=animsequence[animindex];
          animindex++;
//          strcpy(receivedChars, "2, 160, 220, 255, 2");
//          newData = true;
          }  // end if switchState is LOW
       else{
#ifdef DEBUG
          Serial.println ("Switch opened.");
#endif
          }  // end if switchState is HIGH
       }  // end if debounce time up
    }  // end of state change


  
  if (len != 0) {
    if (len != 255){
    /* Got a packet! */
    printHex(packetbuffer, len);
    // Color
    if (packetbuffer[1] == 'C') {
      SOLIDCOLOR = 1;
      //animationState=" ";
      uint8_t red = packetbuffer[2];
      uint8_t green = packetbuffer[3];
      uint8_t blue = packetbuffer[4];
#ifdef DEBUG
      Serial.print (F("RGB #"));
      if (red < 0x10) Serial.print("0");
      Serial.print(red, HEX);
      if (green < 0x10) Serial.print("0");
      Serial.print(green, HEX);
      if (blue < 0x10) Serial.print("0");
      Serial.println(blue, HEX);
#endif
      for (uint8_t i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(red, green, blue));
      }
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
   // Buttons
    if (packetbuffer[1] == 'B' || packetbuffer[1] == 'S' || packetbuffer[1] == 's') {
      if (packetbuffer[1] != 'S' && packetbuffer[1] != 's') {
        uint8_t buttnum = packetbuffer[2] - '0';
        pressed = packetbuffer[3] - '0';
#ifdef DEBUG
        Serial.print (F("Button ")); Serial.print(buttnum);
#endif
        animationState = buttnum;
        if (pressed) {
#ifdef DEBUG
          Serial.println(F(" pressed"));
#endif          
        SOLIDCOLOR = 0;
        } else {
#ifdef DEBUG
          Serial.println(F(" released"));
#endif
          SOLIDCOLOR = 0;
        }
      }
      else {
        animationState = packetbuffer[2];
        SOLIDCOLOR = 0;
      }
    }
    }
    
    // <Warp Factor, Hue, Saturation, Brightness, Pattern>
    // For Charging
    if (animationState == 0 && ! pressed) {
      SOLIDCOLOR = 1;
      //animationState=" ";
      for (uint8_t i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      pixels.show(); // This sends the updated pixel color to the hardware.


//      strcpy(receivedChars, "1, 1, 1, 1, 1");
//      newData = true;
//      colorWipe(pixels.Color(0, 0, 0), 15);
//      pixels.show();
//      wipepixarray();

    }
    // <Warp Factor, Hue, Saturation, Brightness, Pattern>
    if (animationState == 1 && ! pressed) {
      strcpy(receivedChars, "2, 160, 220, 255, 1");
      newData = true;
    }

    if (animationState == 2  && ! pressed) {
      strcpy(receivedChars, "2, 160, 220, 255, 2");
      newData = true;
    }

    if (animationState == 3 && ! pressed) {
      strcpy(receivedChars, "2, 96, 220, 255, 1");
      newData = true;
    }
    if (animationState == 4) {
      strcpy(receivedChars, "1, 255, 220, 255, 3");
      newData = true;
    }
    if (animationState == 5 && ! pressed) {
      if (WarpFactor < 9) {
        WarpFactor++;
        LastWarpFactor = WarpFactor;
        Rate = RateMultiplier * WarpFactor;
      }
    }

    if (animationState == 6 && ! pressed) {
      if (WarpFactor > 1) {
        WarpFactor--;
        LastWarpFactor = WarpFactor;
        Rate = RateMultiplier * WarpFactor;
      }
    }

    if (animationState == 7 && ! pressed) {
      strcpy(receivedChars, "2, 160, 220, 255, 4");
      newData = true;
    }

    if (animationState == 8 && ! pressed) {
      strcpy(receivedChars, "2, 160, 220, 255, 5");
      newData = true;
    }
    if (animationState == 'A' || animationState == 'a' ) {
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      larsonScanner(0, 20);
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
///Red wipes with fast chase after    
    if (animationState == 'B' || animationState == 'b' ) {
        for(int cycles=0;cycles<10;cycles++){
            chase(150, 0, 0, 1, 0, NUMPIXELS-1, 10);                                     //stayon: 1=YES, !1=NO
            delay(300);
            chase(0, 0, 0, 1, 0, NUMPIXELS-1, 5);                                     //stayon: 1=YES, !1=NO
          }

/*        for(int cycles=0;cycles<15;cycles++){
           chasecircle(150, 0, 0, 1, 0, 0, 5);                                 //Direct: CW=1, CCW = !1, stayon: 1=YES, !1=NO
        }
*/    }

///rotate random colors around
    if (animationState == 'C' || animationState == 'c' ) {
      for (int i = 0; i < NUMPIXELS; i++) { //load the array with random colors
        setpixarray(i, 1, random(60), random(30), random(40));
      }
      showpixarray(); pixels.show();
      setall(1);
      for (int i = 0; i < 250; i++) {
        rotatepixarray(CW);
        showpixarray();
        pixels.show();
        delay(60);
      }
      chase(0, 0, 0, 1, NUMPIXELS-1, 0, 20);
      wipepixarray();
    }


//purple dot pingponging
    if (animationState == 'D' || animationState == 'd' ) {
      for (int i = 0; i < 15; i++) {
        pingpong(200, 0, 200, 200, 0, 200, CW,  0, 20);
        pingpong(200, 0, 200, 200, 0, 200, CCW, 0, 20);
      }
      loadpixarray();
      setall(ON);
      spookydownpixarray(40);
      wipepixarray();
    }

///fast chases     
    if (animationState == 'E' || animationState == 'e' ) {
        for(int cycles=0;cycles<25;cycles++){
           chasecircle(0, 200, 200, 1, 0, 0, 10);                                 //Direct: CW=1, CCW = !1, stayon: 1=YES, !1=NO
        }
    }


///random blue flashes
    if (animationState == 'F' || animationState == 'f' ) {
      sparkleonecolor(0, 50, 200, 2, 300, 30);
    }    

///fire    
    if (animationState == 'I' || animationState == 'i' ) {
      for (int i = 0; i < 700; i++) {
        Fire(55, 120, 15);
      }
      wipepixarray();
      flash(0, 0, 0, 1, 0);
    }

/// Blue lightning
    if (animationState == 'L' || animationState == 'l' ) {
        for(int cycles=0;cycles<10;cycles++){
          lightning(0, 0, 200, 1, 5);
        }
    }

///ORANGE!!
    if (animationState == 'O' || animationState == 'o' ) {
    for (int i = 0; i < NUMPIXELS; i++) {
        setpixarray(i, ON, 250, 50, 0);
      }
      spookyuppixarray(50);
     delay(10000);
      spookydownpixarray(40);
    chase(0, 0, 0, 1, NUMPIXELS - 1, 0, 10);
    wipepixarray();
    }
///purple on blue chase
    if (animationState == 'P' || animationState == 'p' ) {
    chase(0, 0, 200, 1, 0, NUMPIXELS - 1, 0);            //void chase(int r, int g, int b, int stayon, int startpixel, int endpixel, int delaytime) { //Direct: CW=1, CCW = !1
    delay(1000);
    for (int i = 0; i < 15; i++) {
      //Purple on blue pixel chase up and down the staircase
        chase(200, 0, 200, 0, 0, NUMPIXELS - 1, 20);
        chase(200, 0, 200, 0, NUMPIXELS - 1, 0, 20);
      }
    chase(0, 0, 200, 1, 0, NUMPIXELS - 1, 0);            //void chase(int r, int g, int b, int stayon, int startpixel, int endpixel, int delaytime) { //Direct: CW=1, CCW = !1
    delay(1000);
    chase(0, 0, 0, 1, NUMPIXELS - 1, 0, 10);
    }
///rainbow
    if (animationState == 'R' || animationState == 'r' ) {
      for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      pixels.setBrightness(255);
      rainbowCycle(10);
      pixels.show(); // This sends the updated pixel color to the hardware.
      loadpixarray();
      showpixarray();
      pixels.show();
      chase(0, 0, 0, 1, NUMPIXELS-1, 0, 20);
      wipepixarray();
//     colorWipe(pixels.Color(0, 0, 0), 20);
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
    
///sparkles
    if (animationState == 'S' || animationState == 's' ) {
      sparkle(4, 200, 500, 30);
    }    

///Theater Chase Rainbow
    if (animationState == 'T' || animationState == 't' ) {
      theaterChaseRainbow(40);
      chase(0, 0, 0, 1, 0, NUMPIXELS-1, 5);                                     //stayon: 1=YES, !1=NO
    }

///White Wipes
    if (animationState == 'W' || animationState == 'w' ) {
      for (int i = 0; i < 10; i++) {
        chase(200, 200, 200, 1, 0, NUMPIXELS - 1, 10);
        delay(200);
        chase(0, 0, 0, 1,0, NUMPIXELS - 1, 5);
      }
      wipepixarray();
    }
  }
  
  receiveSerialData();
  if (!SOLIDCOLOR) {
    if (newData == true) {
      strcpy(tempChars, receivedChars); // this is necessary because strtok() in parseData() replaces the commas with \0
      parseData();
      updateSettings();
      newData = false;
    }
    if (Pattern == 1) {
      standard();
    } else if (Pattern == 2) {
      breach();
    } else if (Pattern == 3) {
      rainbow();
    } else if (Pattern == 4) {
      fade();
    } else if (Pattern == 5) {
      slowFade();
    } else {
      standard();
    }
  }
}


void standard() {
  ReactorHue = MainHue;
  chase();
}

void breach() {
  byte breach_diff = 255 - LastHue;
  byte transition_hue = LastHue + (breach_diff / 2);
  if (ReactorHue < 255) {
    incrementReactorHue();
  }
  if (ReactorHue > transition_hue && MainHue < 255) {
    incrementMainHue();
  }
  if (ReactorHue >= 255 && MainHue >= 255) {
    MainHue = LastHue;
    ReactorHue = MainHue + 1;
  }
  Rate = (((ReactorHue - MainHue) / (breach_diff / 9) + 1) * RateMultiplier);
  WarpFactor = Rate / RateMultiplier;
  chase();
}

void rainbow() {
  Rainbow = true;
  chase();
  Rainbow = false;
}

void fade() {
  Fade = true;
  chase();
  Fade = false;
}

void slowFade() {
  SlowFade = true;
  chase();
  SlowFade = false;
}

void incrementHue() {
  incrementMainHue();
  incrementReactorHue();
}

void incrementReactorHue() {
  if (MainHue == 255) {
    ReactorHue = 1;
  } else {
    ReactorHue++;
  }
}

void incrementMainHue() {
  if (MainHue == 255) {
    MainHue = 1;
  } else {
    MainHue++;
  }
}

void chase() {
  if (Pulse == PulseLength - 1) {
    Pulse = 0;
    if (SlowFade == true) {
      incrementHue();
    }
  } else {
    Pulse++;
  }
  if (Fade == true) {
    incrementHue();
  }
  // Ramp LED brightness
  for (int value = 32; value < 255; value = value + Rate) {
    if (Rainbow == true) {
      incrementHue();
    }
    // Set every Nth LED
    for (int chases = 0; chases < TopChases; chases = chases + PulseLength) {
      byte Top =  Pulse + chases;
      byte Bottom = NUM_LEDS + TopDiff - (Pulse + chases) - 1;
      if (Top < TopLEDtotal) {
        LEDarray[Top] = CHSV(MainHue, Saturation, value);
      }
      if (Bottom > TopLEDcount && Bottom < NUM_LEDS) {
        LEDarray[Bottom] = CHSV(MainHue, Saturation, value);
      }
    }
    // Keep reaction chamber at full brightness even though we chase the leds right through it
    for (int reaction = 0; reaction < ReactionLEDcount; reaction++) {
      LEDarray[TopLEDcount - reaction] = CHSV(ReactorHue, Saturation, 255);
    }
    fadeToBlackBy( LEDarray, NUM_LEDS, (Rate * 0.5));	// Dim all LEDs by Rate/2
    FastLED.show();					// Show set LEDs
  }
}

void receiveSerialData() {
  static bool recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char helpMarker = '?';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc == helpMarker) {
      PrintInfo();
    } else if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else if (rc == endMarker) {
        receivedChars[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() {
  char * strtokIndx;			// this is used by strtok() as an index
  strtokIndx = strtok(tempChars, ",");	// get the first part of the string
  warp_factor = atoi(strtokIndx);		// convert this part to an integer
  strtokIndx = strtok(NULL, ",");		// this continues where the previous call left off
  hue = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  saturation = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  brightness = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  pattern = atoi(strtokIndx);
}

void updateSettings() {
  //	if (pattern > 0 && pattern < 6 && pattern != Pattern) {
  if (pattern > 0 && pattern < 6) {
    /*		warp_factor = DefaultWarpFactor;
    		Rate = RateMultiplier * WarpFactor;
    		hue = DefaultMainHue;
    		saturation = DefaultSaturation;
    		brightness = DefaultBrightness;
    */
    Pattern = pattern;
#ifdef DEBUG
    Serial.print(F("Color Pattern Set To "));
    Serial.println(Pattern);
#endif
    //		updateSettings();
  }
  //	  else {
  //		if (warp_factor > 0 && warp_factor < 10 && warp_factor != LastWarpFactor) {
  if (warp_factor > 0 && warp_factor < 10) {
    WarpFactor = warp_factor;
    LastWarpFactor = warp_factor;
    Rate = RateMultiplier * WarpFactor;
#ifdef DEBUG
    Serial.print(F("Warp Factor Set To "));
    Serial.println(warp_factor);
#endif
  }
  //		if (hue > 0 && hue < 256 && hue != LastHue) {
  if (hue > 0 && hue < 256) {
    MainHue = hue;
    ReactorHue = hue;
    LastHue = hue;
#ifdef DEBUG
    Serial.print(F("Color Hue Set To "));
    Serial.println(hue);
#endif
  }
  //		if (saturation > 0 && saturation < 256 && saturation != Saturation) {
  if (saturation > 0 && saturation < 256) {
    Saturation = saturation;
#ifdef DEBUG
    Serial.print(F("Color Saturation Set To "));
    Serial.println(saturation);
#endif
  }
  if (brightness > 0 && brightness < 256) {
    FastLED.setBrightness(brightness);
    Brightness = brightness;
#ifdef DEBUG
    Serial.print(F("Brightness Set To "));
    Serial.println(brightness);
#endif
  }
  //	}
  newData = false;
}

void PrintInfo() {
#ifdef DEBUG
  Serial.println(F("******** Help ********"));
  Serial.println(F("Input Format - <2, 160, 220, 255, 1>"));
  Serial.println(F("Input Fields - <Warp Factor, Hue, Saturation, Brightness, Pattern>"));
  Serial.println(F("Warp Factor range - 1-9"));
  Serial.println(F("Hue range - 1-255 1=Red 32=Orange 64=Yellow 96=Green 128=Aqua 160=Blue 192=Purple 224=Pink 255=Red"));
  Serial.println(F("Saturation range - 1-255"));
  Serial.println(F("Brightness range - 1-255"));
  Serial.println(F("Pattern - 1-5 1=Standard 2=Breach 3=Rainbow 4=Fade 5=Slow Fade"));
  Serial.println(F(""));
  Serial.println(F("** Current Settings **"));
  Serial.print(F(" <"));
  Serial.print(WarpFactor);
  Serial.print(F(", "));
  Serial.print(MainHue);
  Serial.print(F(", "));
  Serial.print(Saturation);
  Serial.print(F(", "));
  Serial.print(Brightness);
  Serial.print(F(", "));
  Serial.print(Pattern);
  Serial.println(F(">"));
  Serial.println(F("**********************"));
#endif
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}


// c==0 for red otherwise blue
void larsonScanner(uint32_t c, uint8_t wait) {
  int j;
  for (uint16_t i = 0; i < pixels.numPixels() + 5; i++) {
  if(c==0){
    // Draw 5 pixels centered on pos.  setPixelColor() will clip any
    // pixels off the ends of the strip, we don't need to watch for that.
    pixels.setPixelColor(pos - 2, 0x600000); // Dark red
    pixels.setPixelColor(pos - 1, 0xb40100); // Medium red
    pixels.setPixelColor(pos , 0xff0200); // Center pixel is brightest
    pixels.setPixelColor(pos + 1, 0xb40100); // Medium red
    pixels.setPixelColor(pos + 2, 0x600000); // Dark red
  }
  else{
    // Draw 5 pixels centered on pos.  setPixelColor() will clip any
    // pixels off the ends of the strip, we don't need to watch for that.
    pixels.setPixelColor(pos - 2, 0x003b85); // Dark red
    pixels.setPixelColor(pos - 1, 0x005ed2); // Medium red
    pixels.setPixelColor(pos , 0x00c0ff); // Center pixel is brightest
    pixels.setPixelColor(pos + 1, 0x005ed2); // Medium red
    pixels.setPixelColor(pos + 2, 0x003b85); // Dark red
  }  
    pixels.show();
    delay(wait);
    // Rather than being sneaky and erasing just the tail pixel,
    // it's easier to erase it all and draw a new one next time.
    for (j = -2; j <= 2; j++) pixels.setPixelColor(pos + j, 0);

    // Bounce off ends of strip
    pos += dir;
    if (pos < 0) {
      pos = 1;
      dir = -dir;
    } else if (pos >= pixels.numPixels()) {
      pos = pixels.numPixels() - 2;
      dir = -dir;
    }
  }
  //colorWipe(pixels.Color(0, 0, 0), 20);
}


/*
void flashRandom(int wait, uint8_t howmany) {

  for (uint16_t i = 0; i < howmany; i++) {
    // get a random pixel from the list
    int j = random(pixels.numPixels());

    // now we will 'fade' it in 5 steps
    for (int x = 0; x < 5; x++) {
      int r = red * (x + 1); r /= 5;
      int g = green * (x + 1); g /= 5;
      int b = blue * (x + 1); b /= 5;

      pixels.setPixelColor(j, pixels.Color(r, g, b));
      pixels.show();
      delay(wait);
    }
    // & fade out in 5 steps
    for (int x = 5; x >= 0; x--) {
      int r = red * x; r /= 5;
      int g = green * x; g /= 5;
      int b = blue * x; b /= 5;

      pixels.setPixelColor(j, pixels.Color(r, g, b));
      pixels.show();
      delay(wait);
    }
  }
  // LEDs will be off when done (they are faded to 0)
}
*/

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, c);  //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


///fire stuff
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUMPIXELS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUMPIXELS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUMPIXELS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUMPIXELS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUMPIXELS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
// *** REPLACE TO HERE ***

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  pixels.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  pixels.setPixelColor(Pixel, pixels.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUMPIXELS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
