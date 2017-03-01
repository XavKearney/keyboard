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



void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Setting up...");
  // initialize the digital pin as an output.
  
  pinMode(ledPin, OUTPUT);
  for (int c = 0; c < COLS; c++){
    
  Serial.println("Initialising pin:");
  Serial.print(col[c]);
    pinMode(col[c], INPUT);
  }
  for (int r = 0; r < ROWS; r++){
    Serial.println("Initialising pin:");
    Serial.print(row[r]);
    pinMode(row[r], OUTPUT);
  } 
  
  Serial.println("Starting keyboard...");
  Keyboard.begin();
  
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
    //Keyboard.write(keypress); UNCOMMENT ONCE KEYBOARD FIXED
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
    int i = 0;
    for (i = 0; i < len; i++){ //iterate through each character in the string
      setKey(keypressed[i]); //set the key equal to this character
    }
  }
  Keyboard.releaseAll();
  Serial.print("End key");
}

void loop() {
  for (int r = 0; r < ROWS; r++) {
    digitalWrite(row[r], HIGH); //drive each row high one by one
    for (int c = 0; c < COLS; c++){
      if (digitalRead(col[c])){ //check if each column is high, one by one
          // Checks to see if the key pressed is defined in the layout
          if(strcmp(layout[currLayer][r][c],"NULL") != 0){
            Serial.println("Detected high.");
            Serial.print("Col pin:");
            Serial.println(col[c]);
            Serial.print("Row pin:");
            Serial.println(row[r]);
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
          
          }
      }
    }
    digitalWrite(row[r], LOW); //reset the current row to zero
  }
  Serial.println("----");
  delay(100);
}
