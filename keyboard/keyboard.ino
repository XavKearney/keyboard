// Teensy 3.0 has the debug LED on pin 13
const int ledPin = 13;
const int capsLedPin = 4;
const int powerLedPin = 3;

const byte ROWS = 5; 
const byte COLS = 4;
int LAYERS = 12;
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
  {
	  /* DEFINE MODIFIERS AS:
	  CTRL = £
	  ALT = $
	  SHIFT = %
	  
    */
  //layer 0 = Word Normal
  {"\sqrt ","£%+2£%+","\int","derivative"},  // '^' is defined as fn layer key, when held the the layer goes to the desired layer
  {"\infty","\pi","\sum","\log"},
  {"\forall",">=","~=","+-"},
  {"\mu","\delta","\theta",NULL},
  {"caps",NULL,"\cup","shift"}
  },{
    
  //layer 1 = Word Shifted
  {"NULL","^"}, //KORAL WAS HEREs
  {"NULL","#"}
  }
  
};

byte row[ROWS] = {15,19};
byte col[COLS] = {20,23};

int key[] = {0,0,0,0,0,0};
char mod[] = {0,0};



void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  for (int c = 0; c < COLS; c++){
    pinMode(col[c], OUTPUT);
  }
  for (int r = 0; r < ROWS; r++){
    pinMode(row[r], INPUT);
  } 
}

// This function will take keypresses passed to it (in the form of a char, for no particular reason)
// and add them to set of six keys that will be passed to the computer when Keyboard.send_now() is called.

// Basically, this collects the currently pressed keys and stores them until they can be passed to the computer.
void setKey(char keypress){
  
  // Look for unused keys in the buffer  
  int i, j;
  for(i = 0; key[i] != 0; i++){}
  for(j = 0; mod[j] != 0; j++){}
 
  // Catch Modifiers
  if(keypress == 176){
    mod[j] = KEY_LEFT_CTRL;
  }
  else if(keypress == 177){
    mod[j] = KEY_LEFT_ALT;
  }
  else if(keypress == 178){
    mod[j] = KEY_LEFT_SHIFT;
  }
  else{
    key[i] = keypress;
  }
  
  if(holdKey('^')) // Prevent setting layer key into set_key or set_modifier
    return;
  
  // Hold keypresses in buffer
  Keyboard.set_modifier(mod[0]|mod[1]);
  Keyboard.set_key1(key[0]);
  Keyboard.set_key2(key[1]);
  Keyboard.set_key3(key[2]);
  Keyboard.set_key4(key[3]);
  Keyboard.set_key5(key[4]);
  Keyboard.set_key6(key[5]);
  
}

// This method sends the depressed keys and clears the buffer.
void sendKey(){
  
  Keyboard.send_now();
  clearBuffer();
  
  Keyboard.set_modifier(mod[0] | mod[1]);
  Keyboard.set_key1(key[0]);
  Keyboard.set_key2(key[1]);
  Keyboard.set_key3(key[2]);
  Keyboard.set_key4(key[3]);
  Keyboard.set_key5(key[4]);
  Keyboard.set_key6(key[5]);
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

// Toggles between two layers, the curret layer and desired layer
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
	if(strcmp("caps",keypressed) == 0){ // caps toggle added to setKeyMap
		currLayer = currLayer + 4 * (currLayer % 2) - 2; //if already on a caps layer, AKA an even layer, caps toggles off, otherwise on
	} else if(strcmp("shift",keypressed) == 0){ // shift toggle added to setKeyMap
		currLayer = currLayer + 4 * (currLayer % 2) - 2; //TODO: FIX THIS LOGIC
	} else {
    int len = strlen(keypressed); //get the length of the string
    int i = 0;
    for (i = 0; i < len; i++){
      if(i>5){ //can only send 6 keys at once
        sendKey();
        clearBuffer();
      }
	  if(strcmp("£",keypressed[i]){
		  setKey(176)
	  } else if(strcmp("$",keypressed[i]){
		  setKey(177)
	  } else if(strcmp("%",keypressed[i]){
		  setKey(178)
	  }else{
      setKey(keypressed[i]);
	  }
    }
	}
	sendKey();
}

// Goes to desired layer when keyHeld is pressed, returns to previous layer when released 
void holdLayer(char keyHeld, int desLayer){
  
  if(holdKey(keyHeld)){
    
    if(!toggleBind){ // Saves the previous layer, using boolean to prevent updating prevLayer more than once
      prevLayer = currLayer;
      toggleBind = 1;
    }
    
    currLayer = currMode + desLayer; // Desired layer
  }
  
  else{
    
    if(toggleBind){ 
      toggleBind = !toggleBind; // Resets boolean
    }
    
    currLayer = prevLayer; // Returns to previous layer
  }
}

void loop() {

  for (int c = 0; c < COLS; c++) {
    digitalWrite(col[c], HIGH); //drive each column high one by one
    for (int r = 0; r < ROWS; r++){
      if (digitalRead(row[r])){ //check if each row is high, one by one
        
          // Checks to see if the key pressed is defined in the layout
          if(layout[currLayer][r][c] =! NULL){
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
          }
      }
    }
    digitalWrite(col[c], LOW); //reset the current column to zero
  }
  /*TODO: FIX THIS
   * if (holdKey(shiftKey) && holdKey(capsKey)){
	  toggleMode();
  }else{
	holdLayer('shift', shiftLayer); // Checks if shift is held and if so, moves to shiftLayer
  } */
  delay(5);
}
