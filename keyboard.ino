// Teensy 3.0 has the debug LED on pin 13
const int ledPin = 13;
const int capsLedPin = 4;
const int powerLedPin = 3;

const byte ROWS = 5; 
const byte COLS = 4;
int LAYERS = 3;

bool toggleBind = false;
int currLayer = 0;
int prevLayer = 0;
int shiftLayer = 1;

char layout[][ROWS][COLS] = {  
  {
    
  //layer 0 = normal
  {'sqrt','power','int','derivative'},  // '^' is defined as fn layer key, when held the the layer goes to the desired layer
  {'infinity','pi','sum','log'},
  {'forall','gtrthan','equals','plusminus'},
  {'mu','delta','theta',''},
  {'caps','','union','shift'}
  },{
    
  //layer 1 = shift layer
  {KEY_LEFT_ARROW,'^'},
  {KEY_RIGHT_ARROW,'#'}
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
  
  Keyboard.set_modifier(mod[0]);
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

// Calling this function will cycle to the next layer
void cycleLayer(){ 
  
  if(currLayer == (LAYERS - 1)) // Reached maximum layer, going back to first layer
    currLayer = 0;
    
  else
    currLayer++; // Increments to the next layer
}
// Toggles between two layers, the curret layer and desired layer
void toggleLayer(char keyHeld, int desLayer){ 
  
  if (holdKey(keyHeld)){
    prevLayer = currLayer; // Saves previous layer
    currLayer = desLayer; // Desired layer
  }
  
  else
    currLayer = prevLayer; // Returns to previous layer
}


// Macro sequence
void setKeyMap(char keypressed){ 
  // Modifiers:
  // KEY_LEFT_CTRL = 176
  // KEY_LEFT_ALT = 177
  // KEY_LEFT_SHIFT = 178
  
  //For Word Mode: need to send ALT+EQUALS first to open equation editor
	if (currLayer <= 1){
		setKey(177);
		setKey("=");
		sendKey();
		clearBuffer(); //clear the buffer to allow for new keypresses
		delay(1); //put in a short delay to not confuse the PC
	}
	
	//Check which key has been pressed and transform it into the necessary macro.
	if (keypressed == "mu"){ //Word: \mu
		setKey(92); //ASCII \ is 92
		setKey("m");
		setKey("u");
		setKey(" "); //send a space
	}else if(keypressed == "delta"){ //Word : \delta
		setKey(92); //ASCII \ is 92
		setKey("d");
		setKey("e");
		setKey("l");
		setKey("t");
		setKey("a");
		sendKey(); //can't send more than 6 keys at a time
		clearBuffer();
		setKey(" "); //send a space
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
    
    currLayer = desLayer; // Desire layer
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
          if(layout[currLayer][r][c] != ''){
            setKeyMap(layout[currLayer][r][c]); // Work out what to send and send it.
          }
      }
    }
    digitalWrite(col[c], LOW); //reset the current column to zero
  }
  
  holdLayer('shift', shiftLayer); // If the fn layer key is held, it changes the layer to the desired layer, when released, returns to previous layer
  
  delay(5);
}

