#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int backLightPin = 9;
const int brightnessPin = 10;
const int brightnessThreshold = 30;
const int loadingIntervalDelay = 1;
const int lcdColsCount = 16;
const int joyStickXPin = A1;
const int joyStickYPin = A0;
const int joyStickBtnPin = 6;

int readyKeyPressed = 0;
int joyStickX;
int joyStickY;
int joyStickBtn = 1;
int gameStarted = 0;
int countDownDone = 0;
int majidScreenIndex = 16;
int majidNewScreenIndex = 0;
int brickScreenIndex = 15;
char screenArray[] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '|', 'A', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;
int lastDebounceTime1Lock = 0;
int lastDebounceTime2Lock = 0;
int lastDebounceTime3Lock = 0;
int lastDebounceTime4Lock = 0;
unsigned long debounceDelay = 100;

byte armsDown[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};
byte brick[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
};

int loadingFinished = 0;

void loadGame(){
  setLcdBrightness(255);
  turnLcdOn();
  showTextAnimate("BEE", strlen("BEE"));
  showTextAnimate("PRESENTS", strlen("PRESENTS"));
  showTextAnimate("Ready?", strlen("Ready?"));
  delay(500);
  lcd.setCursor(1, 1);
  lcd.print("Press Start Btn");
  loadingFinished = 1;
}
void setBacklightBrightness(uint8_t brightness){
  analogWrite(backLightPin, brightness);
}
void setLcdBrightness(uint8_t brightness){
  if(brightness > brightnessThreshold){
    analogWrite(brightnessPin, brightness);
  }
}
void turnLcdOn(){
  for(int i = 0; i < 256; i++){
    setBacklightBrightness(i);
    delay(loadingIntervalDelay);
  }
}
void gameOver(){
  loadingFinished = 0;
  gameStarted = 0;
  turnLcdOff();
  turnLcdOn();
  lcd.clear();
  showTextAnimate("Game Over", strlen("Game Over"));
  delay(500);
  lcd.setCursor(1, 1);
  lcd.print("Press Start Btn");
  loadingFinished = 1;
  countDownDone = 0;
  joyStickBtn = HIGH;
  
}
void turnLcdOff(){
  for(int i = 255; i > 0; i--){
    setBacklightBrightness(i);
    delay(loadingIntervalDelay);
  }
}
void hideCharacters(){
  for(int i = brightnessThreshold; i < 256; i++){
    setLcdBrightness(i);
    delay(loadingIntervalDelay);
  }
}
void showCharacters(){
  for(int i = 255; i > 0; i--){
    setLcdBrightness(i);
    delay(loadingIntervalDelay);
  }
}
void showTextAnimate(const char *text, int len){
  int cursorX = (lcdColsCount - len) / 2;
  hideCharacters();
  lcd.clear();
  lcd.setCursor(cursorX, 0);
  lcd.print(text);
  showCharacters();
}
void renderScreen(){
  //lcd.clear();
  //lcd.setCursor(0, 0);
  for(int i = 0;i < 32; i++){
    lcd.setCursor(i%16, i/16);
    if(screenArray[i] == 'A'){
      lcd.write((byte)0);
    }else if(screenArray[i] == '|'){
      lcd.write((byte)1);
    }else{
      lcd.print(" ");
    }
  }
}
void calculateMajidPos(){
  screenArray[majidScreenIndex] = '0';
  if(screenArray[majidNewScreenIndex] != '|'){
    screenArray[majidNewScreenIndex] = 'A';
  }else{
    gameOver();
  }
  majidScreenIndex = majidNewScreenIndex;
}
void calculateBrickPos(){
  static int frameCounter = 0;
  long randNumber = random(0,2);
  frameCounter++;
  if(frameCounter >= 2){
    if(brickScreenIndex != 0){
      screenArray[brickScreenIndex] = '0';
      brickScreenIndex--;
      if(screenArray[brickScreenIndex] != 'A'){
        screenArray[brickScreenIndex] = '|';
      }else{
        gameOver();
      }
    }else{
      screenArray[brickScreenIndex] = '0';
      if(randNumber%2 == 0){
        if(screenArray[15] != 'A'){
          screenArray[15] = '|';
          brickScreenIndex = 15;
        }else{
          gameOver();
        }
      }else{
        if(screenArray[31] != 'A'){
          screenArray[31] = '|';
          brickScreenIndex = 31;
        }else{
          gameOver();
        }
      }
    }
    frameCounter = 0;
  }
}

void setup() {
  pinMode(brightnessPin, OUTPUT);
  pinMode(backLightPin, OUTPUT);
  pinMode(joyStickBtnPin, INPUT_PULLUP);
  pinMode(joyStickXPin, INPUT);
  pinMode(joyStickYPin, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, armsDown);
  lcd.createChar(1, brick);
  Serial.begin(9600);
  loadGame();
}
  
void loop() {
  while(!loadingFinished){
  }
  while(joyStickBtn && !gameStarted){
    joyStickBtn = digitalRead(joyStickBtnPin);
  }
  gameStarted = 1;
  if(!countDownDone){
    lcd.clear();
    lcd.setCursor(0, 0);
    showTextAnimate("3", strlen("3"));
    showTextAnimate("2", strlen("2"));
    showTextAnimate("1", strlen("1"));
    hideCharacters();
    lcd.clear();
    showCharacters();
    countDownDone = 1;
  }
  renderScreen();
  joyStickX = analogRead(joyStickXPin);
  joyStickY = analogRead(joyStickYPin);
  if(joyStickX < 300){
    if(!lastDebounceTime1Lock){
      lastDebounceTime1 = millis(); 
      lastDebounceTime1Lock = 1;
    }
    if ((millis() - lastDebounceTime1) > debounceDelay) {
      if(majidNewScreenIndex != 15 && majidNewScreenIndex != 31){
        majidNewScreenIndex++;
      }
      lastDebounceTime1Lock = 0;
    }
  }
  if(joyStickX > 700){
    if(!lastDebounceTime2Lock){
      lastDebounceTime2 = millis();
      lastDebounceTime2Lock = 1;
    }
    if ((millis() - lastDebounceTime2) > debounceDelay) {
      if(majidNewScreenIndex != 0 && majidNewScreenIndex != 16){
        majidNewScreenIndex--;
      }
      lastDebounceTime2Lock = 0;
    }
  }
  if(joyStickY > 700){
    if(!lastDebounceTime3Lock){
      lastDebounceTime3 = millis();
      lastDebounceTime3Lock = 1;
    }
    if ((millis() - lastDebounceTime3) > debounceDelay) {
      if(majidNewScreenIndex < 15){
        majidNewScreenIndex += 16;
      } 
      lastDebounceTime3Lock = 0;
    }
  }
  if(joyStickY < 300){
    if(!lastDebounceTime4Lock){
      lastDebounceTime4 = millis();
      lastDebounceTime4Lock = 1;
    }
    if ((millis() - lastDebounceTime4) > debounceDelay) {
      if(majidNewScreenIndex > 15){
        majidNewScreenIndex -= 16;
      }
      lastDebounceTime4Lock = 0;
    }
  }
  calculateMajidPos();
  calculateBrickPos();
  //delay(10);
}
