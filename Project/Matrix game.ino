#include <Wire.h>
#include <LiquidCrystal.h>
#include <LedControl.h>

//Game elements

typedef struct Snake Snake;
struct Snake{
  int head[2];
  int body[40][2];
  int len;
  int dir[2];
};

typedef struct Food Food;
struct Food{
  int rowPosition;
  int colPosition;  
};

byte pic[8] = {0, 0, 0, 0, 0, 0, 0, 0};

Snake snake = {{1, 5}, {{0, 5}, {1, 5}}, 2, {1, 0}};
Food food = {(int)random(0, 8), (int)random(0, 8)};

int score = 0;

float oldTime = 0;
float timer = 0;
float updateRate = 5;

//Custom LED Matrix Symbols

const byte matrixSize = 8;

const byte greetingSymbol[matrixSize][matrixSize] = {
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

const byte mainMenuSymbol[matrixSize][matrixSize] = {
  {0, 1, 1, 1, 0, 0, 0, 0},
  {0, 1, 0, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 0, 0, 1, 1},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0}
};

byte highscoresSymbol[matrixSize][matrixSize] = {
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0}
};

byte settingsSymbol[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 1, 1},
  {0, 0, 0, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 1, 1, 1, 0, 0, 0},
  {1, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};

byte howToPlaySymbol[matrixSize][matrixSize] = {
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
};

byte aboutSymbol[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {1, 0, 1, 1, 1, 1, 0, 1},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0}
};


// Custom LCD characters
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};


//Wiring legend

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

const byte buzzerPin = 3;
const byte buttonPin = 2;

const int pinJoystickAxisX = A0;
const int pinJoystickAxisY = A1;

//LED Matrix

LedControl matrixLed = LedControl(dinPin, clockPin, loadPin, 1);

byte matrixBrightness = 6;

bool matrixChanged = true;

//Joystick 

const int joystickAxisPins[] = {pinJoystickAxisX, pinJoystickAxisY};

bool joystickAxisMoved[2];
bool joystickAxisTimerWasStarted[2];

unsigned long long joystickAxisTimerStart[2];
volatile unsigned long lastDebounceTime;

const int indexXAxis = 0;
const int indexYAxis = 1;

const int minJoystickThreshold = 450;
const int maxJoystickThreshold = 600;

const int debounceDelay = 50;


//Main menu

String menuItems[] = {"Start game", "Highscores", "Settings", "How to play", "About"};

//About + scrolling parameters

String aboutLink = "https://github.com/A-Bogdan/IntroductionToRobotics";

int stringStart, stringStop = 0;
int scrollCursor = 16;

//How to play

String howToPlayMsg = "Joystick: move the snake";

//Navigation

int currentMenu = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;

const byte topRow = 0;
const byte bottomRow = 1;

const byte downArrowIndex = 2;
const byte upArrowIndex = 1; 
const byte menuCursorIndex= 0;

//Game States

unsigned long long gameStartTimer;
const int greetingMessageTime = 3000;
bool greetingEnded = false;

byte gameState;

const byte greetingState = 0;
const byte mainMenuState = 1;
const byte inGameState = 2;
const byte highscoresState = 3;
const byte settingsState = 4;
const byte howToPlayState = 5;
const byte aboutState = 6;
const byte gameEndedState = 7;

//State redirection

const byte mainMenuRedirect[]={inGameState, highscoresState, settingsState, howToPlayState, aboutState};

//Button 

byte buttonState = LOW;
byte isButtonReleasedState = LOW;
bool wasButtonPressed;
bool buttonTrigger;

//LCD

const byte lcdWidth = 16;
const byte lcdHeight = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, CHANGE);

  lcd.begin(lcdWidth, lcdHeight);
  lcd.clear();

  matrixLed.shutdown(0, false); //enables power
  matrixLed.setIntensity(0, matrixBrightness);
  matrixLed.clearDisplay(0);

  lcd.createChar(menuCursorIndex, menuCursor);
  lcd.createChar(upArrowIndex, upArrow);
  lcd.createChar(downArrowIndex, downArrow);

  updateMatrix(greetingSymbol);
  gameState = greetingState;
  gameStartTimer = millis();

}

void loop() {

  mainMenuDraw();
  drawCursor();
  operateMainMenu();
  delay(20);

}

void updateMatrix(byte matrix[][8]){
  for (int row = 0; row < matrixSize; row++){
    for(int col = 0; col < matrixSize; col++){
      matrixLed.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void buttonInterrupt(){
  if (digitalRead(buttonPin) != isButtonReleasedState && !wasButtonPressed && !buttonTrigger){
    lastDebounceTime = millis();
    wasButtonPressed = true;
  } else if (millis() - lastDebounceTime > debounceDelay && wasButtonPressed) {
    buttonTrigger = true;
    wasButtonPressed = false;
  }
}

void mainMenuDraw(){
  lcd.clear();

  lcd.setCursor(1, topRow);
  lcd.print(menuItems[currentMenu]);

  lcd.setCursor(1,bottomRow);
  lcd.print(menuItems[currentMenu+1]);

  if (currentMenu == 0){

    lcd.setCursor(15,bottomRow);
    lcd.write(byte(downArrowIndex));
  } else if (currentMenu > 0 && currentMenu < maxMenuPages) {
    
      lcd.setCursor(15, bottomRow);
      lcd.write(byte(downArrowIndex));

      lcd.setCursor(15, topRow);
      lcd.write(byte(upArrowIndex));
  } else if (currentMenu == maxMenuPages){

      lcd.setCursor(15, topRow);
      lcd.write(byte(upArrowIndex));
  }
}

void drawCursor() {

  for (int i = 0; i < 2; i++){
    lcd.setCursor(0, i);
    lcd.print(" ");
  }

  if (currentMenu % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, topRow);
      lcd.write(byte(menuCursorIndex));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, bottomRow);
      lcd.write(byte(menuCursorIndex));
    }
  }
  if (currentMenu % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, bottomRow);
      lcd.write(byte(menuCursorIndex));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, topRow);
      lcd.write(byte(menuCursorIndex));
    }
  }

}

void operateMainMenu() {

  switch(gameState){
    case greetingState:
      state1();
      break;
    case mainMenuState:
      state2();      
      break;
    case inGameState:
      state3();
      break;
    case highscoresState:
      state4();
      break;
    case settingsState:
      state5();
      break;
    case howToPlayState:
      state6();
      break;
    case aboutState:
      state7();
      break;
    case gameEndedState:
      state8();
      break;
  };

}

void moveJoystickAxis(byte axis, bool inGame) {


  int joystickAxisInput = analogRead(joystickAxisPins[axis]);

  if ( joystickAxisInput < minJoystickThreshold || joystickAxisInput > maxJoystickThreshold) {
    if(!joystickAxisTimerWasStarted[axis]){

        joystickAxisTimerStart[axis] = millis();
        joystickAxisTimerWasStarted[axis] = true;

    }

    if(millis() - joystickAxisTimerStart[axis] > debounceDelay && (!joystickAxisMoved[axis] || inGame)){
      joystickAxisMoved[axis] = true;

      if (gameState != inGameState){
        tone(buzzerPin, 600, 100);
      }

      switch(gameState) {
        case mainMenuState:
          navigateMenu(currentMenu, cursorPosition, joystickAxisInput);
          break;
        case inGameState:
          moveSnake(axis, joystickAxisInput);          
          break;
      };
    }   
  } else {
      joystickAxisTimerWasStarted[axis] = false;
      joystickAxisMoved[axis] = false;
  }



}

void navigateMenu(int& currentMenu, int& cursorPosition, int joystickAxisInput){

  if (joystickAxisInput > maxJoystickThreshold){

    if(currentMenu == 0){
      cursorPosition = cursorPosition - 1;
      cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
    }

    if(currentMenu % 2 == 0 && cursorPosition % 2 == 0){
      currentMenu = currentMenu - 1;
      currentMenu = constrain(currentMenu, 0, maxMenuPages);
    }

    if (currentMenu % 2 != 0 and cursorPosition % 2 != 0) {
      currentMenu = currentMenu - 1;
      currentMenu = constrain(currentMenu, 0, maxMenuPages);
    } 

    cursorPosition = cursorPosition - 1;
    cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
   

  } else if (joystickAxisInput < minJoystickThreshold){
      if (currentMenu % 2 == 0 and cursorPosition % 2 != 0) {
        currentMenu = currentMenu + 1;
         currentMenu = constrain(currentMenu, 0, maxMenuPages);
      }

      if (currentMenu % 2 != 0 and cursorPosition % 2 == 0) {
        currentMenu = currentMenu + 1;
        currentMenu = constrain(currentMenu, 0, maxMenuPages);
      }

      cursorPosition = cursorPosition + 1;
      cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
 
  }

}

//Greeting on bootup
void state1() {

  lcd.clear();

  lcd.setCursor(5, topRow);
  lcd.print("Snake!");

  delay(20);

  if (!greetingEnded && millis() - gameStartTimer > greetingMessageTime) {
    gameState = mainMenuState;
    greetingEnded = true;
  } 

}


//Main menu
void state2() {

  updateMatrix(mainMenuSymbol);
  moveJoystickAxis(indexYAxis, false);

  if (buttonTrigger) {
    gameState = mainMenuRedirect[cursorPosition];
    buttonTrigger = false;  
  }
}

//The game

void state3() {

  float deltaTime = calculateDeltaTime();
  timer += deltaTime;

  lcd.clear();

  lcd.setCursor(3, bottomRow);
  lcd.print("Score : ");
  lcd.print(score);
  delay(10);

  moveJoystickAxis(indexXAxis, true);
  moveJoystickAxis(indexYAxis, true);

  if(timer > 1000/updateRate){
    timer = 0;
    update();
  }

  render();
}

void moveSnake(byte axis, int joystickAxisInput) {

  if (axis == indexXAxis){
    if(joystickAxisInput < minJoystickThreshold && snake.dir[indexYAxis]==0){
      snake.dir[indexXAxis] = 0;
      snake.dir[indexYAxis] = -1;
    } else if (joystickAxisInput > maxJoystickThreshold && snake.dir[indexYAxis]==0) {
        snake.dir[indexXAxis] = 0;
        snake.dir[indexYAxis] = 1;
    }
  } else if (axis == indexYAxis){
    if(joystickAxisInput < minJoystickThreshold && snake.dir[indexXAxis]==0){
      snake.dir[indexXAxis] = -1;
      snake.dir[indexYAxis] = 0;
    } else if (joystickAxisInput > maxJoystickThreshold && snake.dir[indexXAxis]==0) {
        snake.dir[indexXAxis] = 1;
        snake.dir[indexYAxis] = 0;
    }    
  }

}

float calculateDeltaTime(){
  float currentTime = millis();
  float delta = currentTime - oldTime;
  oldTime = currentTime;
  return delta;
}

void reset(){

  for(int j = 0; j < 8; j++){
    pic[j] = 0;
  }

}

void update(){

  reset();

  int newHead[2] = {snake.head[0]+snake.dir[0], snake.head[1]+snake.dir[1]};

//Going out of bounds

  if(newHead[0]==8){
    newHead[0] = 0;
  } else if (newHead[0] == -1) {
      newHead[0] = 7;
  } else if (newHead[1] == 8) {
    newHead[1] = 0;
  } else if(newHead[1] == -1) {
    newHead[1] = 7;
  }

//Snake collides with itself

  for(int j = 0; j < snake.len; j++){
    if (snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1]){
      snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}}; //Reinitialize snake
      food = {(int)random(0,8),(int)random(0,8)}; //Reinitialize food
      gameState = gameEndedState;
    }
  }

//Check if snake reached the food

  if(newHead[0] == food.rowPosition && newHead[1] == food.colPosition){
    tone(buzzerPin, 2000, 50);
    score = score + 10;
    snake.len++;
    food.rowPosition = (int)random(0, 8);
    food.colPosition = (int)random(0, 8);
  } else {
    removeFirst();
  }

  snake.body[snake.len - 1][0] = newHead[0];
  snake.body[snake.len - 1][1] = newHead[1];

  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];

  for(int j = 0; j < snake.len; j++){
    pic[snake.body[j][0]] |= 128 >> snake.body[j][1];
  }

  pic[food.rowPosition] |= 128 >> food.colPosition;
  
}

void render(){
  
  for(int i = 0; i < 8; i++) {
    matrixLed.setRow(0, i, pic[i]);
  }

}

void removeFirst(){

  for(int j = 1; j < snake.len; j++){
    snake.body[j - 1][0] = snake.body[j][0];
    snake.body[j - 1][1] = snake.body[j][1];
  }

}


//Highscores section

void state4() {

  updateMatrix(highscoresSymbol);

  lcd.clear();

  if(buttonTrigger){
    gameState = mainMenuState;
    buttonTrigger = false;
  }  
}  

//Settings section

void state5(){

  updateMatrix(settingsSymbol);

  lcd.clear();

  if(buttonTrigger){
    gameState = mainMenuState;
    buttonTrigger = false;
  }  
}

//How to play section

void state6(){

  updateMatrix(howToPlaySymbol);

  lcd.clear();

  lcd.setCursor(0, topRow);
  lcd.print(howToPlayMsg.substring(stringStart, stringStop));

  lcd.setCursor(1, bottomRow);
  lcd.print("Get the food!");

  delay(300);

  if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop) {
    stringStart = stringStop = 0;
    scrollCursor = 16;
  } else if (stringStop == howToPlayMsg.length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }  

  if(buttonTrigger){
    gameState = mainMenuState;
    buttonTrigger = false;
  }

}

//About section
void state7() { 

  updateMatrix(aboutSymbol);

  lcd.clear();
  lcd.setCursor(5, topRow);
  lcd.print("About:");
    
  lcd.setCursor(scrollCursor, bottomRow);
  lcd.print(aboutLink.substring(stringStart, stringStop));

  delay(300);

  if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop) {
    stringStart = stringStop = 0;
    scrollCursor = 16;
  } else if (stringStop == aboutLink.length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }

  if(buttonTrigger){
    gameState = mainMenuState;
    buttonTrigger = false;
  }
}

//Game over

void state8() {
  lcd.clear();

  lcd.setCursor(3, topRow);
  lcd.print("Game Over!");

  lcd.setCursor(3, bottomRow);
  lcd.print("Score: ");
  lcd.print(score);
  delay(20);

  tone(buzzerPin, 1500, 1000);


  if(millis() - oldTime > 2000){
    gameState = mainMenuState;
    score = 0;
  }
}
















