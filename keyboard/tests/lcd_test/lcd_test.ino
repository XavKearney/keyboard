#include <LiquidCrystalFast.h>

// Teensy 3.0 has the debug LED on pin 13
const int ledPin = 13;
const int powerLedPin = 3;
const int LCD_RS = 2;
const int LCD_RW = 3;
const int LCD_EN = 4;
const int LCD_D4 = 5;
const int LCD_D5 = 6;
const int LCD_D6 = 7;
const int LCD_D7 = 8;

const byte ROWS = 5; 
const byte COLS = 4;
int LAYERS = 1;
int MODES = 3;

bool toggleBind = false;
int currLayer = 0;
int prevLayer = 0;

//int wordLayer = 0;
//int latexLayer = 4;
//int asciiLayer = 8;

int capsLayer = 1;
int shiftLayer = 2;
int caps_shift = 3;

int currMode = 0; //side note: (always) int currMode == (currLayer % 4) / 4; <-- this isn't true?

const char* layout[][ROWS][COLS] = {  
  {//layer 0 = normal 
  {"1","2","3","4"},
  {"5","6","7","8"},
  {"9","10","11","12"},
  {"13","14","15","16"},
  {"17","18","19","20"},
  }
  
};

byte row[ROWS] = {15,16,17,18,19};
byte col[COLS] = {20,21,22,23};

int key[] = {0,0,0,0,0,0};
char mod[] = {0,0};


LiquidCrystalFast lcd (LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
//LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("Starting...");
  lcd.begin(8,2);
  Serial.println("Begun...");
  lcd.print("Hello!");
  
  Serial.println("Printed...");
}


void setKey(char keypress){
  /* DEFINE MODIFIERS AS:
    CTRL = £
    ALT = $
    SHIFT = %
    */
  // Look for unused keys in the buffer  
  int i, j;
  for(i = 0; key[i] != 0; i++){}
  for(j = 0; mod[j] != 0; j++){}
 
  // Catch Modifiers
  if(strcmp("£",&keypress) == 0){
    Keyboard.press(KEY_LEFT_CTRL); //NOT THE SAME FOR IOS
  }
  else if(strcmp("$",&keypress) == 0){
    Keyboard.press(KEY_LEFT_ALT);
  }
  else if(strcmp("%",&keypress) == 0){
    Keyboard.press(KEY_LEFT_SHIFT);
    Serial.print("Shift");
  }else if(strcmp("\\",&keypress) == 0){
    Keyboard.press(92);
    Serial.print("Slash");
  }else if(strcmp("~",&keypress) == 0){
    Keyboard.releaseAll();
    Serial.print("Release");
  }
  else{
    Keyboard.write(keypress);
    Serial.print(keypress);
  }
  
  
}

// Macro sequence
void setKeyMap(const char* keypressed){ 
/* DEFINE MODIFIERS AS:
    CTRL = £
    ALT = $
    SHIFT = %
    */
  if(strcmp("caps",keypressed) == 0){ // caps toggle added to setKeyMap
    currLayer = currLayer + 4 * (currLayer % 2) - 2; //if already on a caps layer, AKA an even layer, caps toggles off, otherwise on
  } else {
    int len = strlen(keypressed); //get the length of the string
    Serial.println("Length of string:");
    Serial.print(len);
    int i = 0;
    for (i = 0; i < len; i++){ //iterate through each character in the string
      setKey(keypressed[i]); //set the key equal to this character
    }
  }
  Keyboard.releaseAll();
}

void loop() {
  delay(500);
}
