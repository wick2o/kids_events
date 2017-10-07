static const struct {const char letter, *code;} MorseMap[] =
{
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "-.-." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', ".-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },
  { ' ', "   " }, //Gap between word, seven units 
    
  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },
    
  { '.', "·–·–·–" },
  { ',', "--..--" },
  { '?', "..--.." },
  { '!', "-.-.--" },
  { ':', "---..." },
  { ';', "-.-.-." },
  { '(', "-.--." },
  { ')', "-.--.-" },
  { '"', ".-..-." },
  { '@', ".--.-." },
  { '&', ".-..." },
};

byte seven_seg_digits[10][7] = { 
 // e,d,c,g,f,a,b 
  { 0,0,0,1,0,0,0 },  // 0 GOOD
  { 1,1,0,1,1,1,0 },  // 1  1 = off 0 = on
  { 0,0,1,0,1,0,0 },  // 2
  { 1,0,0,0,1,0,0 },  // 3
  { 1,1,0,0,0,1,0 },  // 4
  { 1,0,0,0,0,0,1 },  // 5
  { 0,0,0,0,0,0,1 },  // 6
  { 1,1,0,1,1,0,0 },  // 7
  { 0,0,0,0,0,0,0 },  // 8 GOOD
  { 1,1,0,0,0,0,0 },  // 9
};

String morse_code_messages[10] = {
  "SOS",
  "SOS SOS",
  "I LIKE TURTLES",
  "CREEPERS GONNA CREEP",
  "GAME OVER MAN",
  "THE CROW FLIES AT MIDNIGHT",
  "BEWARE THE PANDA",
  "THE TALOS HAS YOU",
  "25 DREAMS SOMETHING DIFFERENT",
  "THAT ONE TIME AT CIRCLE CITY CON",
};




int current_message_id = 0;
volatile int currently_playing_message = 0;
const int btnSelectPin = 3;
const int btnPlayStopPin = 4;
const int ledMorseFlasher = 13;

const int ledMorseFlashLength = 250;

int btnSelectState = LOW;
int btnSelectLastState = LOW;
unsigned long btnSelectDebounceDelay = 500;
unsigned long btnSelectLastDebounceMillis = 0;

int btnPlayStopState = LOW;
int btnPlayStopLastState = LOW;
unsigned long btnPlayStopDebounceDelay = 500;
unsigned long btnPlayStopLastDebounceMillis = 0;

void sevenSegWrite(byte digit) {
  byte pin = 5;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    digitalWrite(pin, seven_seg_digits[digit][segCount]);
    ++pin;
  }
}

String encode(const char *string)
{
  size_t i, j;
  String morseWord = "";
  
  for( i = 0; string[i]; ++i )
  {
    for( j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j )
    {
      if( toupper(string[i]) == MorseMap[j].letter )
      {
        morseWord += MorseMap[j].code;
        break;
      }
    }
    morseWord += ""; //Add tailing space to seperate the chars
  }

  return morseWord;  
}


void setup() {
  Serial.begin(9600);
  
  pinMode(btnPlayStopPin, INPUT); 
  pinMode(ledMorseFlasher, OUTPUT);
  pinMode(btnSelectPin, INPUT);
  
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  digitalWrite(ledMorseFlasher, HIGH );

  sevenSegWrite(current_message_id); 
}

void loop() {

  unsigned long currentMillis = millis();

  // am i currently playing a message?
  switch (currently_playing_message) {
    case 0:
      Serial.println("Not currently Playing a message");
      btnSelectState = digitalRead(btnSelectPin);
      btnPlayStopState = digitalRead(btnPlayStopPin);

      //Serial.println(btnPlayStopState);
      
      if (btnSelectState != btnSelectLastState) {
        btnSelectLastDebounceMillis = millis();
      }

      if (btnPlayStopState != btnPlayStopLastState) {
        btnPlayStopLastDebounceMillis = millis();
      }

      if ((currentMillis - btnPlayStopLastDebounceMillis) > btnPlayStopDebounceDelay) {
        btnPlayStopLastState = btnPlayStopState;
        if (btnPlayStopLastState == HIGH) {
          currently_playing_message = 1;
          digitalWrite(ledMorseFlasher, LOW);
        }
      }

      if (currently_playing_message != 1) {
        
        if ((currentMillis - btnSelectLastDebounceMillis) > btnSelectDebounceDelay) {
          if (btnSelectState != btnSelectLastState) {
            btnSelectLastState = btnSelectState;
            
            if (btnSelectLastState == HIGH) {
              
              if (current_message_id == 9) {
                current_message_id = 0;
              }
              else
              {
                current_message_id += 1;
              }
              Serial.print("Changing the current_message_id to: ");
              Serial.println(current_message_id);
              Serial.print("Current Morse Code Message is: ");
              Serial.println(morse_code_messages[current_message_id]);
              sevenSegWrite(current_message_id);
            }
          }
        }
      }

      break;
    case 1:
      //Serial.println("Yes");

          String StepOne = morse_code_messages[current_message_id];

          String morseWord = encode(StepOne.c_str());

          for(int i=0; i<=morseWord.length(); i++)
            {
              switch( morseWord[i] )
              {
                case '.': //dit
                  digitalWrite(ledMorseFlasher, LOW );
                  delay(ledMorseFlashLength);
                  digitalWrite(ledMorseFlasher, HIGH );
                  delay(ledMorseFlashLength);
                  break;
                case '-': //dah
                  digitalWrite(ledMorseFlasher, LOW );
                  delay(ledMorseFlashLength * 4);
                  digitalWrite(ledMorseFlasher, HIGH );
                  delay(ledMorseFlashLength);
                  break;
                case ' ': //gap
                  delay(ledMorseFlashLength * 1.5);
              }
              delay(ledMorseFlashLength);
            }

              currently_playing_message = 0;
            
      }

  }

  
