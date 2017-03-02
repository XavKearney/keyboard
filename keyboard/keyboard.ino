//#include <LiquidCrystalFast.h>

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

int MODES = 3;

bool shift_On = false;
bool caps_On = false;
int currLayer = 0;
int prevLayer = 0;
int mode_counter = 0;

int capsLayer = 1;
int shiftLayer = 2;
int caps_shift = 3;

  /* DEFINE MODIFIERS AS:
    CTRL = #
    ALT = $
    SHIFT = %
    ESC = ¬
    ENTER = `
    LEFT_ARROW_KEY = <
    RIGHT_ARROW_KEY = >
    */
const char* layout[][ROWS][COLS] = {  
  {//layer 0 = word - normal 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\ge","\\simeq","+-"},
  {"\\infty","\\pi","\\sigma","$jem`¬"},
  {"\\sqrt","^2","$jei`¬","d/d"},
  },
  {//layer 1 = word - caps 
  {"caps","NULL","\\bigcup","NULL"},
  {"\\Mu","\\Delta","\\Theta","shift"},
  {"NULL","NULL","~=","NULL"},
  {"NULL","\\Pi","\\sum","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
  {//layer 2 = word - shift 
  {"caps","NULL","\\cap","NULL"},
  {"\\lambda","\\phi","\\omega","shift"},
  {"\\exists","\\le","\\ne","-+"},
  {"\\emptyset","\\angle","NULL","$jem>>`¬"},
  {"$jer>`","^","$jei>`","\\partial/<\\partial"},
  },
  {//layer 3 = word - shift+caps 
  {"caps","NULL","\\bigcap","NULL"},
  {"\\Lambda","\\Phi","\\Omega","shift"},
  {"NULL","NULL","NULL","NULL"},
  {"NULL","NULL","$jeg>`¬","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },



  {//layer 4 = latex - normal 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\geq","\\simeq","\\pm"},
  {"\\infty","\\pi","\\sigma","\\log"},
  {"\\sqrt{}<","^2","\\int  \\mathrm{d}x<<<<<<<<<<<<","\\frac{\\mathrm{d}}{\\mathrm{d}}<"},
  },
  {//layer 5 = latex - caps 
  {"caps","NULL","\\bigcup","NULL"},
  {"\\Mu","\\Delta","\\Theta","shift"},
  {"NULL","NULL","\\cong","NULL"},
  {"NULL","\\Pi","\\sum","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },
  {//layer 6 = latex - shift 
  {"caps","NULL","\\cap","NULL"},
  {"\\lambda","\\phi","\\omega","shift"},
  {"\\exists","\\leq","\\neq","\\mp"},
  {"\\emptyset","\\measuredangle","NULL","\\lim_{m \\to \\n}"},
  {"\\sqrt[n]{}<","^","\\int_a^b \\mathrm{d}x ","\\frac{\\partial}{\\partial}}"},
  },
  {//layer 7 = latex - shift+caps 
  {"caps","NULL","\\bigcap","NULL"},
  {"\\Lambda","\\Phi","\\Omega","shift"},
  {"NULL","NULL","NULL","NULL"},
  {"NULL","NULL","\\displaystyle\\sum_{k=m}^n","NULL"},
  {"NULL","NULL","NULL","NULL"},
  },



  {//layer 8 = unicode - normal 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\ge","\\simeq","+-"},
  {"\\infty","\\pi","\\Sigma","$jem`¬"},
  {"\\sqrt","^2","$jei`¬","d/d"},
  },
  {//layer 9 = unicode - caps 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\ge","\\simeq","+-"},
  {"\\infty","\\pi","\\Sigma","$jem`¬"},
  {"\\sqrt","^2","$jei`¬","capson"},
  },
  {//layer 10 = unicode - shift 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\ge","\\simeq","+-"},
  {"\\infty","\\pi","\\Sigma","$jem`¬"},
  {"\\sqrt","^2","$jei`¬","shifton"},
  },
  {//layer 11 = unicode - shift+caps 
  {"caps","NULL","\\cup","NULL"},
  {"\\mu","\\delta","\\theta","shift"},
  {"\\forall","\\ge","\\simeq","+-"},
  {"\\infty","\\pi","\\Sigma","$jem`¬"},
  {"\\sqrt","^2","$jei`¬","shiftcapson"},
  }  
};

byte row[ROWS] = {15,16,17,18,19};
byte col[COLS] = {20,21,22,23};

int key[] = {0,0,0,0,0,0};
char mod[] = {0,0};


//LiquidCrystalFast lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
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
  //lcd.begin(8, 2);
  //lcd.setCursor(0, 0);
  //lcd.print("Hello!");
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
  if(strcmp("#",&keypress) == 0){
    Keyboard.press(KEY_LEFT_CTRL); //NOT THE SAME FOR IOS
  }
  else if(strcmp("$",&keypress) == 0){
    Keyboard.press(KEY_LEFT_ALT);
  } 
  else if(strcmp("¬",&keypress) == 0){
    Keyboard.press(KEY_ESC);
  }
  else if(strcmp("`",&keypress) == 0){
    Keyboard.press(KEY_ENTER);
  }
  else if(strcmp("%",&keypress) == 0){
    Keyboard.press(KEY_LEFT_SHIFT);
    Serial.print("Shift");
  }else if(strcmp("\\",&keypress) == 0){
    Keyboard.press(KEY_BACKSLASH);
  }else if(strcmp("<",&keypress) == 0){
    Keyboard.press(KEY_LEFT_ARROW);
  }else if(strcmp(">",&keypress) == 0){
    Keyboard.press(KEY_RIGHT_ARROW);
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


// Helper function to clear the buffer
void clearBuffer(){
  
  for(int x = 0; x < 6; x++){ key[x] = 0; }
  for(int x = 0; x < 2; x++){ mod[x] = 0; }
  
}

// Detects when a key is held down, returns true if held down, false if not
bool holdKey(char keypress){
  
  if(key[0] == keypress ||
     key[1] == keypress ||
     key[2] == keypress ||
     key[3] == keypress ||
     key[4] == keypress ||
     key[5] == keypress){
    return true;
  }
  
  return false;
}

// Toggles between two layers, the current layer and desired layer
void toggleMode(){ 
  
	//if both shift and caps held
	currLayer = (currLayer + 4) % 12;
}

// Macro sequence
void setKeyMap(const char* keypressed){ 
/* DEFINE MODIFIERS AS:
    CTRL = £
    ALT = $
    SHIFT = %
    */
	if(strcmp("caps",keypressed) == 0){ // caps toggle
		currLayer = currLayer - 2 * (currLayer % 2) + 1;
	} else if(strcmp("shift",keypressed) == 0){
	}
	else {
    int len = strlen(keypressed); //get the length of the string
    Serial.print(len);
    int i = 0;
    for (i = 0; i < len; i++){ //iterate through each character in the string
      if(i>5){ //can only send 6 keys at once
        Serial.println("Greater than 6.");
      }
      setKey(keypressed[i]); //set the key equal to this character
    }
    
  Keyboard.press(KEY_SPACE);
  Keyboard.releaseAll();
	}
  
}

// Goes to desired layer when keyHeld is pressed, returns to previous layer when released 

void loop() {
  for (int r = 0; r < ROWS; r++) {
    digitalWrite(row[r], HIGH); //drive each row high one by one
    for (int c = 0; c < COLS; c++){
      if (digitalRead(col[c])){ //check if each column is high, one by one
        if((strcmp(layout[currLayer][r][c],"shift") == 0) && !shift_On){
          currLayer = currLayer + 2;
          shift_On = true;
        }
        if((strcmp(layout[currLayer][r][c],"caps") == 0)){
          caps_On = true;
        }
          // Checks to see if the key pressed is defined in the layout
          if(strcmp(layout[currLayer][r][c],"NULL") != 0){
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
          }
      }else if((strcmp(layout[currLayer][r][c],"shift") == 0) && shift_On){
        currLayer = currLayer - 2;
        shift_On = false;
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
    digitalWrite(ledPin,HIGH);
    currLayer = (currLayer + 4) % (MODES * 4);
    mode_counter = 0;
  }
  delay(100);
}
