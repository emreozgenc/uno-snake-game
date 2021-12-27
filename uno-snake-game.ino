#include<LCD5110_Graph.h>
LCD5110 screen(8,9,10,11,12);
const short SWITCH_PIN = 7;
const short STATE_MENU = 0;
const short STATE_GAME = 1;
const short AXIS_X = A0;
const short AXIS_Y = A1;
const short TOP = 700;
const short DOWN = 300;
short programState = STATE_MENU;
short difficultySelection = 0;

const String startMessage = "YILAN OYUNU";
const String difficulties[] = {"Kolay", "Orta", "Zor"};

extern uint8_t TinyFont[];
extern uint8_t SmallFont[];
// 84*48 pixels


void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  screen.InitLCD();
  Serial.begin(9600);

}

void handleMenu() {
  bool switchPressed = !digitalRead(SWITCH_PIN);
  if(switchPressed) 
    programState = STATE_GAME;

  short stickValue = analogRead(AXIS_Y);

  if(stickValue > TOP) {
    difficultySelection++;
    return;
  }
  if(stickValue < DOWN) {
    difficultySelection--;
    return;
  }
  
}

void renderMenu() {
  screen.setFont(SmallFont);
  screen.print(startMessage, CENTER, 0);

  screen.setFont(TinyFont);
  for(int i=0;i<3;i++) {
    screen.print(difficulties[i], CENTER, 12+(i*10));
  }
  //30, 54
  int y = 10*((abs(difficultySelection)%3) + 1);
  screen.drawRect(30, y, 54, y+8);
}

void loop() {
  screen.clrScr();
  switch(programState){
    case STATE_MENU:
      handleMenu();
      renderMenu();
      break;
    case STATE_GAME:
      break;
  }
  screen.update();
  delay(200);

}
