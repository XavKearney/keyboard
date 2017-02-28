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

byte row[ROWS] = {15,16,17,18,19}
byte col[COLS] = {20,21,22,23};

int key[] = {0,0,0,0,0,0};
char mod[] = {0,0};



void setup() {
  Serial.begin(9600);
  Serial.println("Setting up...");
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  for (int c = 0; c < COLS; c++){
    pinMode(col[c], INPUT);
  }
  for (int r = 0; r < ROWS; r++){
    pinMode(row[r], OUTPUT);
  } 
  
  Serial.println("Starting keyboard...");
  Keyboard.begin();
  
  Serial.println("Starting LCD...");
  LiquidCrystalFast lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
  lcd.begin(8, 2);
  Serial.println("Writing to LCD...");
  lcd.setCursor(0, 1);
  lcd.print("Starting");
  lcd.setCursor(0, 1);
  lcd.print("TESTMODE");
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
  
  if(holdKey('^')) // Prevent setting layer key into set_key or set_modifier
    return;
  
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
      if(i>5){ //can only send 6 keys at once
        sendKey();
        clearBuffer();
      }
      setKey(keypressed[i]); //set the key equal to this character
    }
  }
  Keyboard.releaseAll();
}

void loop() {

  for (int r = 0; r < ROWS; R++) {
    digitalWrite(row[r], HIGH); //drive each row high one by one
    for (int c = 0; c < COLS; c++){
      if (digitalRead(col[c])){ //check if each column is high, one by one
          // Checks to see if the key pressed is defined in the layout
          if(strcmp(layout[currLayer][r][c],"NULL") != 0){
            Serial.println("Detected column high...");
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
          }
      }
    }
    digitalWrite(row[r], LOW); //reset the current column to zero
  }
  delay(100);
}
