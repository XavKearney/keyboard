#include <LiquidCrystalFast.h>

// Teensy 3.0 has the debug LED on pin 13
const int ledPin = 13;
const int LCD_RS = 2;
const int LCD_RW = 3;
const int LCD_EN = 4;
const int LCD_D4 = 5;
const int LCD_D5 = 6;
const int LCD_D6 = 7;
const int LCD_D7 = 8;

const byte ROWS = 5; 
const byte COLS = 4;

int MODES = 3;
int currMode = 0;
int currLayer = 0;
int mode_counter = 0;

bool shift_On = false;
bool caps_On = false;
bool caps_Lock = false;


  /* DEFINE MODIFIERS AS:
    ALT_GR = #
    ALT = ?
    SHIFT = %
    ESC = @
    ENTER = *
    LEFT_ARROW_KEY = <
    RIGHT_ARROW_KEY = >
    */
const char* modes[3] = {
  "Word ","LaTeX","ASCII"
};
    
const char* layout[][ROWS][COLS] = {  
  {//layer 0: Word_Normal
  {"caps","NULL","\\cup ","NULL"},
  {"\\mu ","\\delta ","\\theta ","shift"},
  {"\\forall ","\\ge ","\\simeq ","+- "},
  {"\\infty ","\\pi ","\\sigma ","?jem @@"},
  {"\\sqrt ","^2 ","?jei @","d/d <<<"},
  },
  {//layer 1: Word_Caps
  {"caps","NULL","\\bigcup ","NULL"},
  {"\\Mu ","\\Delta ","\\Theta ","shift"},
  {"NULL","NULL","~= ","NULL"},
  {"NULL","\\Pi ","\\sum ","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
  {//layer 2: Word_Shift
  {"caps","NULL","\\cap ","NULL"},
  {"\\lambda ","\\phi ","\\omega ","shift"},
  {"\\exists ","\\le ","\\ne ","-+ "},
  {"\\emptyset ","\\angle ","NULL","?jem>>*@` <<"},
  {"?jer> @@","^","?jei> @@","\\partial/\\partial <<<"},
  },
  {//layer 3: Word_Shift+Caps
  {"caps","NULL","\\bigcap ","NULL"},
  {"\\Lambda ","\\Phi ","\\Omega ","shift"},
  {"NULL","NULL","NULL","NULL"},
  {"NULL","NULL","?jeg> @@","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },



  {//layer 4: LaTeX_Normal
  {"caps","NULL","\\cup ","NULL"},
  {"\\mu ","\\delta ","\\theta ","shift"},
  {"\\forall ","\\geq ","\\simeq ","\\pm "},
  {"\\infty ","\\pi ","\\sigma ","\\log "},
  {"\\sqrt{} <<","^2 ","\\int  \\,dx <<<<<<","\\frac{\\mathrm{d}}{\\mathrm{d}}<<<<<<<<<<<<<<"},
  },
  {//layer 5: LaTeX_Caps
  {"caps","NULL","\\bigcup ","NULL"},
  {"\\Mu ","\\Delta ","\\Theta ","shift"},
  {"NULL","NULL","\\cong ","NULL"},
  {"NULL","\\Pi ","\\sum ","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
  {//layer 6: LaTeX_Shift
  {"caps","NULL","\\cap ","NULL"},
  {"\\lambda ","\\phi ","\\omega ","shift"},
  {"\\exists ","\\leq ","\\neq ","\\mp "},
  {"\\emptyset ","\\measuredangle ","NULL","\\lim_{m \\to n} <<<<<<<<"},
  {"\\sqrt[n]{}<","^","\\int_{a}^{b} \\,dx<<<<<<<<<<","\\frac{\\partial }{\\partial }}<<<<<<<<<<<<<"},
  },
  {//layer 7: LaTeX_Shift+Caps
  {"caps","NULL","\\bigcap ","NULL"},
  {"\\Lambda ","\\Phi ","\\Omega ","shift"},
  {"NULL","NULL","NULL","NULL"},
  {"NULL","NULL","\\displaystyle\\sum_{k=m}^{n}<<<<<","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },



  {//layer 8: ASCII_Normal
  {"caps","NULL","222a","NULL"},
  {"03bc","03b4","03b8","shift"},
  {"2200","2265","2248","00b1"},
  {"221e","03C0","03c3","NULL"},
  {"221a","00b2","222b","NULL"},
  },
  {//layer 9: ASCII_Caps
  {"caps","NULL","22c3","NULL"},
  {"039c","0394","03f4","shift"},
  {"NULL","NULL","2243","NULL"},
  {"NULL","03a0","03a3","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
  {//layer 10: ASCII_Shift
  {"caps","NULL","2229","NULL"},
  {"03bb","03c6","03c9","shift"},
  {"2203","2264","2260","2213"},
  {"2205","2220","NULL","NULL"},
  {"221b","00b3","NULL","2202/2202"},
  },
  {//layer 11: ASCII_Shift+Caps
  {"caps","NULL","22c2","NULL"},
  {"039b","03a6","03a9","shift"},
  {"NULL","NULL","NULL","NULL"},
  {"NULL","NULL","2211","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
};

byte row[ROWS] = {15,16,17,18,19};
byte col[COLS] = {20,21,22,23};
byte mu[] = {
  B00000,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B10000,
  B10000
};
LiquidCrystalFast lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  lcd.begin(8,2);
  lcd.createChar(0, mu);
  lcd.setCursor(2,1);
  lcd.print("Board");
  lcd.setCursor(0,0);
  lcd.print("Power On");
  lcd.setCursor(1,1);
  lcd.write(0);
  Serial.println("Printed...");
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  for (int c = 0; c < COLS; c++){
    pinMode(col[c], INPUT);
  }
  for (int r = 0; r < ROWS; r++){
    pinMode(row[r], OUTPUT);
  } 
  Serial.begin(9600);
  Keyboard.begin();
  delay(500);
  lcd.clear();
  lcd.print("Mode:");
  lcd.setCursor(0,1);
  lcd.print(modes[currMode]);
}


void setKey(char keypress){
  /* DEFINE MODIFIERS AS:
    CTRL = £
    ALT = ?
    SHIFT = %
    */
  // Catch Modifiers
  if(keypress == '?'){
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.release(KEY_LEFT_ALT);
  }
  else if(keypress == '<'){
    Keyboard.press(KEY_LEFT_ARROW);
    Serial.println("DETECTED");
    Keyboard.release(KEY_LEFT_ARROW);
  }
  else if(keypress == '@'){
    Keyboard.press(KEY_ESC);
    Keyboard.release(KEY_ESC);
  }
  else if(keypress == '*'){
    Keyboard.press(KEY_ENTER);
  }
  else if(keypress == '%'){
    Keyboard.press(KEY_LEFT_SHIFT);
    Serial.print("Shift");
  }
  else if(keypress == '>'){
    Keyboard.press(KEY_RIGHT_ARROW);
    Serial.println("DETECTED");
    Keyboard.release(KEY_RIGHT_ARROW);
  }
  else if(keypress == '`'){
    Keyboard.releaseAll();
    Serial.print("Release");
  }
  else if(keypress == '#'){
    Keyboard.press(KEY_NUM_LOCK);
    Keyboard.press(KEY_RIGHT_ALT);
    Serial.println("DETECTED");
    Keyboard.release(KEY_RIGHT_ALT);
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
    ALT = ?
    SHIFT = %
    */
  
	if(strcmp("caps",keypressed) == 0){ // caps toggle
		currLayer = currLayer - 2 * (currLayer % 2) + 1;
    caps_Lock = !caps_Lock;
    if(caps_Lock){        
        lcd.setCursor(7,1);
        lcd.print("C");
    }else{
      lcd.setCursor(7,1);
      lcd.print(" ");
    }
	} else if(strcmp("shift",keypressed) == 0){
	}
	else {
    //if(currMode == 2){
    //  Keyboard.press(KEY_LEFT_ALT);
    //  Keyboard.press('`');
    //}
    int len = strlen(keypressed); //get the length of the string
    Serial.print(len);
    int i = 0;
    for (i = 0; i < len; i++){ //iterate through each character in the string
      setKey(keypressed[i]); //set the key equal to this character
    }
    
  Keyboard.releaseAll();
	}
  
}

void loop() {

  for (int r = 0; r < ROWS; r++) {
    digitalWrite(row[r], HIGH); //drive each row high one by one
    for (int c = 0; c < COLS; c++){
      if (digitalRead(col[c])){ //check if each column is high, one by one
        if((strcmp(layout[currLayer][r][c],"shift") == 0) && !shift_On){
          currLayer = currLayer + 2;
          shift_On = true;
          lcd.setCursor(6,1);
          lcd.print("S");
        }
        if((strcmp(layout[currLayer][r][c],"caps") == 0)){
          caps_On = true;
          delay(100);
          lcd.setCursor(7,1);
          lcd.print("C");
        }
          // Checks to see if the key pressed is defined in the layout
          if(strcmp(layout[currLayer][r][c],"NULL") != 0){
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
            while(digitalRead(col[c]) && (strcmp(layout[currLayer][r][c],"shift") != 0) && (strcmp(layout[currLayer][r][c],"caps") != 0)){
              delay(5);
            }
          }
      }else if((strcmp(layout[currLayer][r][c],"shift") == 0) && shift_On){
        currLayer = currLayer - 2;
        shift_On = false;
        lcd.setCursor(6,1);
        lcd.print(" ");
      }else if(strcmp(layout[currLayer][r][c],"caps") == 0){
        caps_On = false;
      }
    }
    digitalWrite(row[r], LOW); //reset the current column to zero
  }
  if(shift_On && caps_On){
    mode_counter++;
  }else{
    digitalWrite(ledPin,LOW);
    mode_counter = 0;
  }
  if(mode_counter > 4){
    currMode = (currMode + 1) % MODES;
    lcd.setCursor(0,1);
    lcd.print(modes[currMode]);
    digitalWrite(ledPin,HIGH);
    currLayer = currMode * 4;
    caps_Lock = false;
    shift_On = false;
    lcd.setCursor(6,1);
    lcd.print("  ");
    mode_counter = 0;
    delay(300);
  }
  delay(50);
}
