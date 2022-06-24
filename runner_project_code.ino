/*WARNING:Keep the volume of the device where you are simulating the circuit LOW!
The piezo will make your ears burst. XD
    ~Sincerely,Aditya Sankhla.
*/

#include <LiquidCrystal.h>

/*My entry for ElectroLITe event.
Features:
Fully functioning Runner game.
There are two buttons,Jump and Duck.
Custom obstacles created.(stickman figure,obstacle in the form of 'A' which is the first letter of my name (because why not) and a frown face in the sky.)
Jumping,ducking and dying have SOUND effects.
At the end,prints the score and you may press the button to play again.
*/

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int JUMP_PIN = 2;
const int BUZZER_PIN = 5;
const int DUCK_PIN = 3; 

const int JUMP_PITCH = 2700; 
const int JUMP_PITCH_DURATION = 50; 
const int DUCK_PITCH = 1350; 
const int DUCK_PITCH_DURATION = 50;
const int DIE_PITCH = 200;
const int DIE_PITCH_DURATION = 500;
const int TICKSPEED = 90;
const int JUMP_LENGTH = 3;

/*For custom characters,use this,
https://www.engineersgarage.com/making-custom-characters-on-lcd-using-arduino/
*/

byte stickStep1[8] = { //custom characters on the LCD
  B01110,
  B01110,
  B00101,
  B11111,
  B10100,
  B00110,
  B11001,
  B00001,
};
byte stickStep2[8] = {
  B01110,
  B01110,
  B00101,
  B11111,
  B10100,
  B00110,
  B01011,
  B01000,
};
byte stickJump[8] = {
  B01110,
  B01110,
  B00100,
  B11111,
  B00100,
  B11111,
  B10001,
  B00000,
};
byte stickDuck[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B01110,
  B11111,
  B00100,
  B11111,
};
byte nameInitial[8] = {
  B11111,
  B10001,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
};
byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001

};
byte smilie[8] {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001

};

//DONE,tickspeed for the score to be recorded needs to be done still...

volatile int jumpPhase = JUMP_LENGTH + 1;
int gameTick = 0;
int crowX = 40;
int hillX = 25;
bool playerY = 0;
volatile bool ducking = LOW;
bool loopBreaker = 1;
bool crowGo = 0;
int score = 0;

void setup() {
  pinMode(JUMP_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, nameInitial);
  lcd.createChar(1, stickStep1);
  lcd.createChar(2, stickStep2);
  lcd.createChar(3, stickJump);
  lcd.createChar(4, stickDuck);
  lcd.createChar(5, frownie);
  lcd.createChar(6, smilie);
  attachInterrupt(digitalPinToInterrupt(JUMP_PIN), seeJumping, RISING);
  attachInterrupt(digitalPinToInterrupt(DUCK_PIN), seeDucking, CHANGE);
  /*try with attachInterrupt to check whether it works.
  Remember to debug the seeJumping and seeDucking
  */
}

void loop() {
  playerY = 0;
  if (jumpPhase < JUMP_LENGTH) {
    playerY = 1;
  }

  drawSprites();

  loopBreaker = 1;
  if (hillX < 16) {
    if (crowX < hillX) {
      hillX += 8;
      loopBreaker = 0;
    }
    if (loopBreaker) {
      lcd.setCursor(hillX, 1);
      lcd.write((byte)0);
    }
  }
  if (hillX < 1) {
    if (jumpPhase < JUMP_LENGTH) {
      score++;
      hillX = 16 + rand() % 8;
    } else {
      endGame();
    }
  }
  if (crowX < 16) {
    lcd.setCursor(crowX, 0);
    if (gameTick % 8 < 4) {
      lcd.write((byte)5);
    } else {
      lcd.write((byte)6);
    }
  }
  if (crowX < 1) {
    if (ducking) {
      score++;
      crowX = 24 + rand() % 16;
    } else {
      endGame();
    }
  }
  lcd.setCursor(0, playerY);
  lcd.print(" ");
  jumpPhase++;
  hillX--;
  crowGo = !crowGo;
  crowX -= crowGo;
  gameTick++;
  delay(TICKSPEED);
}

/*
For contrast control,try this link,might work,idk: 
https://create.arduino.cc/projecthub/zurrealStudios/lcd-backlight-and-contrast-control-6d3452
*/

void endGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(7, 0);
  lcd.print(score);
  tone(BUZZER_PIN, DIE_PITCH, DIE_PITCH_DURATION);
  while (!digitalRead(JUMP_PIN)) {
    lcd.setCursor(0, 1);
    if (millis() % 500 < 250) {
      lcd.print("Jump to Play");
    } else {
      lcd.print("                ");
    }
  }
  lcd.clear();
  score = 0;
  hillX = 25;
  crowX = 40;
}

void drawSprites() {
  lcd.setCursor(0, 1 - playerY);

  if (!ducking) {
    if (!playerY) {
      if ((gameTick % 4) < 2 ) {
        lcd.write((byte)1);
      } else {
        lcd.write((byte)2);
      }
    } else {
      lcd.write((byte)3);
    }
  } else {
    lcd.write((byte)4);
  }
  lcd.setCursor(1, 1);
  lcd.print("               ");
  lcd.setCursor(1, 0);
  lcd.print("               ");
}
void seeJumping() {
  if (jumpPhase > (JUMP_LENGTH + 2) && !ducking) {
    jumpPhase = 0;
    tone(BUZZER_PIN, JUMP_PITCH, JUMP_PITCH_DURATION); //tone ke liye change the pitch later.
  }

}
void seeDucking() {
  ducking = digitalRead(DUCK_PIN);
  if (ducking) {
    jumpPhase = JUMP_LENGTH;
    tone(BUZZER_PIN, DUCK_PITCH, DUCK_PITCH_DURATION);//tone ke liye change the pitch later.
  }
}



//That’s it everyone! Thanks for reviewing my project.
