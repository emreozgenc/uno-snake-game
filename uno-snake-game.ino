#include<LCD5110_Graph.h>
LCD5110 screen(8,9,10,11,12);
const short SWITCH_PIN = 7;
const short STATE_MENU = 0;
const short STATE_GAME = 1;
const short AXIS_X = A0;
const short AXIS_Y = A1;
const short TOP = 700;
const short DOWN = 300;
const short D_UP = 0;
const short D_DOWN = 1;
const short D_LEFT = 2;
const short D_RIGHT = 3;
short snakeDirection = D_RIGHT;
short programState = STATE_MENU;
short difficultySelection = 0;
bool gameInit = true;

typedef struct n {
  struct n * next;
  short x;
  short y;
}node;

node *snake;

const String startMessage = "YILAN OYUNU";
const String difficulties[] = {"Kolay", "Orta", "Zor"};

extern uint8_t TinyFont[];
extern uint8_t SmallFont[];
// 84*48 pixels

node * addNode(short x, short y) {
  if(snake == NULL) {
    snake = (node *)malloc(sizeof(node));
    snake->next = NULL;
    snake->x = x;
    snake->y = y;
    return snake;
  }

  node * iter = snake;
  while(iter->next != NULL)
    iter = iter->next;
  node * temp = (node*)malloc(sizeof(node));
  temp->x = x;
  temp->y = y;
  temp->next = NULL;
  iter->next = temp;
  return snake;
}


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

void renderSnake() {
  node *iter = snake;
  while(iter != NULL) {
    short x = iter->x;
    short y = iter->y;
    screen.drawRect(x, y, x+1, y+1);
    iter = iter->next;
  }
}

void moveSnake() {
  node * head;
  node * iter = snake;

  while(iter->next != NULL) {
    iter = iter->next;
  }
  head = iter;
  
  switch(snakeDirection) {
    case D_RIGHT:
      head->x += 1;
      break;
    case D_LEFT:
      head->x -= 1;
      break;
    case D_UP:
      head->y -= 1;
      break;
    case D_DOWN:
      head->y += 1;
      break;
  }

  if(head->x > 84) {
    head->x = 0;
  }
  if(head->y > 48) {
    head->y = 0;
  }
  if(head->x < 0) {
    head->x = 84;
  }
  if(head->y < 0) {
    head->y = 48;
  }
  
  
  iter = snake;
  while(iter->next != NULL) {
    iter->x = iter->next->x;
    iter->y = iter->next->y;
    iter = iter->next;
  }
  
}

void decideSnakeDirection() {
  short x = analogRead(AXIS_X);
  short y = analogRead(AXIS_Y);

  if(x > TOP && snakeDirection != D_LEFT) {
    snakeDirection = D_RIGHT;
    return;
  }

  if(x < DOWN && snakeDirection != D_RIGHT) {
    snakeDirection = D_LEFT;
    return;
  }

  if(y > TOP && snakeDirection != D_UP) {
    snakeDirection = D_DOWN;
    return;
  }

  if(y < DOWN && snakeDirection != D_DOWN) {
    snakeDirection = D_UP;
    return;
  }
}
void handleGame() {
  if(gameInit) {
    snake = addNode(0,0);
    snake = addNode(2,0);
    snake = addNode(4,0);
    gameInit = false;
  }
  decideSnakeDirection();
  moveSnake();
}

void renderGame() {
  renderSnake();
}


void loop() {
  screen.clrScr();
  switch(programState){
    case STATE_MENU:
      handleMenu();
      renderMenu();
      break;
    case STATE_GAME:
      handleGame();
      renderGame();
      break;
  }
  screen.update();
  delay(200);

}
