#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8,7,6,5}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String inputCode, passCode;
bool isCorect = false;

#define corectPin 4
#define falsePin 5

void setup(){
  Serial.begin(115200);

  passCode = String("4000");

  pinMode(corectPin, OUTPUT);
  pinMode(falsePin, OUTPUT);
}
  
void loop(){
  char key = keypad.getKey();
    // just print the pressed key
   if (key){
    Serial.println(key);
  } 
  
  // this checkes if 4 is pressed, then do something. Here  we print the text but you can control something.
  if (key =='4'){
    Serial.println("Key 4 is pressed");
    inputCode += '4';
  }

  if (key =='0'){
    Serial.println("Key 0 is pressed");
    inputCode += '0';
  }

  if (key =='*'){
    Serial.println("Key * is pressed");
    inputCode = "";
    Serial.println("inputCode is: "+inputCode);
  }

  if (key =='#'){
    Serial.println("inputCode is: "+inputCode);
    Serial.println("Key # is pressed");
    if (inputCode == passCode){
      isCorect = true;
      Serial.println("The pincode is: true");
      digitalWrite(corectPin, HIGH);
      delay(1000);
      digitalWrite(corectPin, LOW);
    } else {
      isCorect = false;
      Serial.println("The pincode is: false");
      digitalWrite(falsePin, HIGH);
      delay(1000);
      digitalWrite(falsePin, LOW);
    }
    inputCode = "";
  }
}