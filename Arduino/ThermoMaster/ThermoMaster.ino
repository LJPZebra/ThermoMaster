/* ====================================================================
  ________                              __  ___           __           
 /_  __/ /_  ___  _________ ___  ____  /  |/  /___ ______/ /____  _____
  / / / __ \/ _ \/ ___/ __ `__ \/ __ \/ /|_/ / __ `/ ___/ __/ _ \/ ___/
 / / / / / /  __/ /  / / / / / / /_/ / /  / / /_/ (__  ) /_/  __/ /    
/_/ /_/ /_/\___/_/  /_/ /_/ /_/\____/_/  /_/\__,_/____/\__/\___/_/     

Control for the ThermoMaster setup.

Copyright Laboratoire Jean Perrin (LJP)
Written by Raphael Candelier (raphael.candelier@upmc.fr)

===================================================================== */

// --- INCLUDES --------------------------------------------------------

#include <Adafruit_MAX31856.h> // Thermocouples
#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_ST7735.h>   // Hardware-specific library
#include <SPI.h>

// --- #DEFINEs --------------------------------------------------------

// Color definitions
#define BLACK   0x0000
#define GRAY    0x8410
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE  0xFA60
#define YELLOW  0xFFE0 
#define LIME    0x07FF
#define GREEN   0x07E0
#define CYAN    0x07FF
#define AQUA    0x04FF
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define PINK    0xF8FF

// --- PINOUTS ---------------------------------------------------------

// --- IR Light

const int pLight = 3;

// --- Temperature sensors

// NB: Use of Hardware SPI:
// pin 11 (ISCP 4) is MOSI/SDI
// pin 12 (ICSP 1) is MISO/SDO
// pin 13 (ISCP 3) is CLK
const int plSensor = 7;
const int prSensor = 8;

// --- Peltier control (PWM compatible pins)
const int pRc = 5;
const int pRh = 6;
const int pLh = 9;
const int pLc = 10;

// --- Screen
const int pTFT_CS = A0;
const int pTFT_DC = A1;
const int pTFT_RST = A2;

// --- DEFINITIONS ----------------------------------------------------

boolean bRegul = false;
boolean bDirect = false;

float lTarget = 28;
float rTarget = 28;

float Pcoeff = 75;
float Icoeff = 0.55; 
float Dcoeff = 50; 

float lErr = 0,     rErr = 0;
float lErrRef = 0,  rErrRef = 0;
int ErrIntTime = 10;
float lErrInt = 0,  rErrInt = 0;
float lCmd = 0,     rCmd = 0;

float tRef;

// --- INITIALIZATIONS ------------------------------------------------

// --- Thermocouples

Adafruit_MAX31856 lTC = Adafruit_MAX31856(plSensor);
Adafruit_MAX31856 rTC = Adafruit_MAX31856(prSensor);

// --- Screen
Adafruit_ST7735 screen = Adafruit_ST7735(pTFT_CS, pTFT_DC, pTFT_RST);

void setup() { 

  // --- IR light
  pinMode(pLight, OUTPUT);
  analogWrite(pLight, 128);
  
  // --- Serial communication
  Serial.begin(115200);
  Serial.setTimeout(1);

  // --- Peltier modules
  pinMode(pLc, OUTPUT);
  pinMode(pLh, OUTPUT);
  pinMode(pRc, OUTPUT);
  pinMode(pRh, OUTPUT);
  analogWrite(pLc, 0);
  analogWrite(pLh, 0);
  analogWrite(pRc, 0);
  analogWrite(pRh, 0);

  // Temperature sensors
  lTC.begin();
  rTC.begin();
  lTC.setThermocoupleType(MAX31856_TCTYPE_K);
  rTC.setThermocoupleType(MAX31856_TCTYPE_K);

  // --- Screen initialization
  pinMode(pTFT_CS, OUTPUT);
  pinMode(pTFT_DC, OUTPUT);
  
  screen.initR(INITR_BLACKTAB);
  screen.fillScreen(ST7735_BLACK);
  screen.setRotation(3);

  // Targets
  screen.drawLine(20, 9, 26, 9, WHITE);
  screen.drawLine(24, 7, 26, 9, WHITE);
  screen.drawLine(24, 11, 26, 9, WHITE);
  screen.drawLine(100, 9, 106, 9, WHITE);
  screen.drawLine(104, 7, 106, 9, WHITE);
  screen.drawLine(104, 11, 106, 9, WHITE);

  screen.drawLine(0, 85, 159, 85, WHITE);
  
  // --- PID parameters

  screen.setTextColor(WHITE, BLACK);
  screen.setTextSize(1);

  screen.setCursor(5,93);
  screen.print("P");
  
  screen.setCursor(5,105);
  screen.print("I");

  screen.setCursor(5,117);
  screen.print("D");

  // --- Control variables

  /// Text
  screen.setCursor(58,93);
  screen.print(" Te");
  
  screen.setCursor(58,105);
  screen.print(" Te");
  screen.drawChar(58, 105, 227, WHITE, BLACK, 1);

  screen.setCursor(58,117);
  screen.print("dTe");
  
  
  // --- Time reference
  tRef = micros();
  
}

void loop() {
    
  // === INPUTS ======================================================
    
  if (Serial.available()) {   
  
    String cmd = Serial.readString();
    cmd.trim();
      
    // --- Identifier
    if (cmd.equals("getId")) { Serial.println("ThermoMaster"); }

    // --- Light
    if (cmd.substring(0,5).equals("light")) {
      int tmp = cmd.substring(6).toInt();
      analogWrite(pLight, tmp);
    }

    // --- Direct control
    if (cmd.substring(0,2).equals("Lh")) {
      
      int val = cmd.substring(3).toInt();
      Serial.println("Heating left at " + String(100*val/255) + "%");
      bRegul = false; 
      bDirect = true;
      lCmd = 255;
      
    } else if (cmd.substring(0,2).equals("Lc")) {
      
      int val = cmd.substring(3).toInt();
      Serial.println("Cooling left at " + String(100*val/255) + "%");
      bRegul = false; 
      bDirect = true;
      lCmd = -255;
      
    } else if (cmd.substring(0,2).equals("Rh")) {
      
      int val = cmd.substring(3).toInt();
      Serial.println("Heating right at " + String(100*val/255) + "%");
      bRegul = false; 
      bDirect = true;
      rCmd = 255;
      
    } else if (cmd.substring(0,2).equals("Rc")) {
      int val = cmd.substring(3).toInt();
      Serial.println("Cooling right at " + String(100*val/255) + "%");
      bRegul = false;
      bDirect = true;
      rCmd = -255;
    }
    
    // --- Regulation
    if (cmd.equals("start")) {
      bRegul = true;
      bDirect = false;
      lErrInt = ErrIntTime*lErr;
      rErrInt = ErrIntTime*rErr;
    }
    if (cmd.equals("stop")) { bRegul = false; bDirect = false; }

    if (cmd.substring(0,1).equals("P")) { Pcoeff = cmd.substring(2).toFloat(); }
    if (cmd.substring(0,1).equals("I")) { Icoeff = cmd.substring(2).toFloat(); }
    if (cmd.substring(0,1).equals("D")) { Dcoeff = cmd.substring(2).toFloat(); }
      
    // --- Set target temperatures
    if (cmd.substring(0,3).equals("set")) {
      
      int k;
      for (int i = 4; i<=cmd.length(); i++) {
        if (cmd.charAt(i) == ' ') { k = i; }
      }
        
      char bufferLeft[10];
      cmd.substring(4,k).toCharArray(bufferLeft, 10);
      lTarget = atof(bufferLeft);

      char bufferRight[10];
      cmd.substring(k+1).toCharArray(bufferRight, 10);
      rTarget = atof(bufferRight);

      Serial.print(lTarget);
      Serial.print(" - ");
      Serial.println(rTarget);
      
    }

  }

  // === MEASUREMENT =================================================

  float lTemp = lTC.readThermocoupleTemperature();
  float rTemp = rTC.readThermocoupleTemperature();
  unsigned long t = micros();

  // Send data to the program
  Serial.print("Data ");   
  Serial.print(t);
  Serial.print(" ");
  Serial.print(lTemp);
  Serial.print(" ");
  Serial.println(rTemp);

  // === CONTROL ====================================================

  // --- Get errors
  lErr = lTarget - lTemp;
  rErr = rTarget - rTemp;

  // --- Integrals
  lErrInt = (lErrInt + lErr)*(1-1/ErrIntTime);
  rErrInt = (rErrInt + rErr)*(1-1/ErrIntTime);
  if (lErrInt>99) { lErrInt = 99; }
  if (rErrInt>99) { rErrInt = 99; }
   
  if (bRegul) {
    
    // --- Get new commands
    lCmd = lErr*Pcoeff + lErrInt*Icoeff + (lErr-lErrRef)*Dcoeff;
    rCmd = rErr*Pcoeff + rErrInt*Icoeff + (rErr-rErrRef)*Dcoeff;

    // Boundaries
    if (lCmd<-255) { lCmd = -255; }
    if (lCmd>255) { lCmd = 255; }
    if (rCmd<-255) { rCmd = -255; }
    if (rCmd>255) { rCmd = 255; }
    
  } else if (!bDirect) {

      lCmd = 0;
      rCmd = 0;
      
  }
  
  // --- Apply commands
  if (lCmd>=0) {                    // Left, heat
    analogWrite(pLc, 0);
    analogWrite(pLh, round(lCmd));
  } else {                          // Left, cool
    analogWrite(pLh, 0);
    analogWrite(pLc, round(-lCmd));
  }

  if (rCmd>=0) {                    // Right, heat
    analogWrite(pRc, 0);
    analogWrite(pRh, round(rCmd));
  } else {                          // Right, cool
    analogWrite(pRh, 0);
    analogWrite(pRc, round(-rCmd));
  }
 
  // === DISPLAY ======================================================

  // Serial.println("Left: " + String(lTemp) + " / " + String(lErr) + " / "  + String(lErr-lErrRef) + " / "  + String(lErrInt) + " | " + String(lCmd) + " | " + String(lP) + " / " + String(lI) + " / " + String(lD));

  // --- Temperatures

  screen.setTextColor(WHITE, BLACK);
  screen.setTextSize(1);

  screen.setCursor(30,5);
  screen.print(lTarget);

  screen.setCursor(110,5);
  screen.print(rTarget);

  // Left temperature
  
  screen.setTextSize(3);

  int i1 = (int) lTemp;
  int i2 = lTemp*100 - i1*100;

  screen.setCursor(5,25);
  if (i1<10) { screen.print("0"); }
  screen.print(i1);
  screen.setCursor(40,32);
  screen.setTextSize(2);
  screen.print(".");
  if (i2<10) { screen.print("0"); }
  screen.print(i2);

  // Right temperature
    
  screen.setTextSize(3);

  i1 = (int) rTemp;
  i2 = rTemp*100 - i1*100;

  screen.setCursor(85,25);
  if (i1<10) { screen.print("0"); }
  screen.print(i1);
  screen.setCursor(120,32);
  screen.setTextSize(2);
  screen.print(".");
  if (i2<10) { screen.print("0"); }
  screen.print(i2);

  // --- Heating / Cooling

  screen.setTextSize(1);

  // Left

  screen.setCursor(15, 60);
  if (lCmd==0) { 
    screen.setTextColor(WHITE, BLACK);
    screen.print(" ");
  } else if (lCmd<0) { 
    screen.setTextColor(CYAN, BLACK); 
    screen.print("C");
  } else { 
    screen.setTextColor(RED, BLACK); 
    screen.print("H");
  }

  screen.setCursor(40, 60);
  screen.print((int) abs(lCmd)*100/255);
  screen.print("%  ");

  // Right

  screen.setCursor(100, 60);
  if (rCmd==0) { 
    screen.setTextColor(WHITE, BLACK);
    screen.print(" ");
  } else if (rCmd<0) { 
    screen.setTextColor(CYAN, BLACK); 
    screen.print("C");
  } else { 
    screen.setTextColor(RED, BLACK); 
    screen.print("H");
  }

  screen.setCursor(120, 60);
  screen.print((int) abs(rCmd)*100/255);
  screen.print("%  ");

  // --- PID Parameters
  screen.setTextColor(YELLOW, BLACK);
  screen.setTextSize(1);

  screen.setCursor(20,93);
  screen.print(Pcoeff);
  
  screen.setCursor(20,105);
  screen.print(Icoeff);

  screen.setCursor(20,117);
  screen.print(Dcoeff);

  // --- Control loop variables

  // Left
  screen.setCursor(85,93);
  screen.print(lErr);

  screen.setCursor(85,105);
  screen.print(lErrInt);

  screen.setCursor(85,117);
  screen.print(lErr-lErrRef);
  
  // Right
  screen.setCursor(125,93);
  screen.print(rErr);

  screen.setCursor(125,105);
  screen.print(rErrInt);

  screen.setCursor(125,117);
  screen.print(rErr-rErrRef);
  
  // --- Update error references --------------------------------------
  
  lErrRef = lErr;
  rErrRef = rErr;
  
}

// === COLOR FUNCTIONS ================================================

word RGB(byte R, byte G, byte B) { return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) ); }
