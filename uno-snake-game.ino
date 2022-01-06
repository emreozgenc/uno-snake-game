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
bool foodSpawned = false;

typedef struct n {
  struct n * next;
  short x;
  short y;
}node;

node *snake;
node food;

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
    screen.drawRect(x, y, x+3, y+3);
    iter = iter->next;
  }
}

node * getSnakeHead() {
  node * iter = snake;
  while(iter->next != NULL) {
    iter = iter->next;
  }
  return iter;
}

void moveSnake() {
  node * head = getSnakeHead();
  
  switch(snakeDirection) {
    case D_RIGHT:
      head->x += 4;
      break;
    case D_LEFT:
      head->x -= 4;
      break;
    case D_UP:
      head->y -= 4;
      break;
    case D_DOWN:
      head->y += 4;
      break;
  }

  if(head->x >= 84) {
    head->x = 0;
  }
  if(head->y >= 48) {
    head->y = 0;
  }
  if(head->x < 0) {
    head->x = 84;
  }
  if(head->y < 0) {
    head->y = 48;
  }
  
  
  node *iter = snake;
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

bool checkFoodSpawnLocation(short x, short y) {
  node *iter = snake;
  while(iter != NULL) {
    if(iter->x == x && iter->y == y)
      return false;

    iter = iter->next;
  }

  if(x%4 != 0)
    return false;
  if(y%4 != 0)
    return false;

  return true;
}

void spawnFood() {
  if(!foodSpawned) {
    randomSeed(analogRead(A3));
    short x = random(0, 80);
    short y = random(0, 44);

    while(!checkFoodSpawnLocation(x, y)) {
      x = random(0, 80);
      y = random(0, 44);
    }

    food.x = x;
    food.y = y;
    foodSpawned = true;
  }
}

void checkFoodCollision() {
  node * head = getSnakeHead();
  if(food.x == head->x && food.y == head->y) {
    foodSpawned = false;
    moveSnake();
    switch(snakeDirection) {
      case D_LEFT:
        snake = addNode(food.x - 4, food.y);
        return;
      case D_RIGHT:
        snake = addNode(food.x + 4, food.y);
        return;
      case D_UP:
        snake = addNode(food.x, food.y - 4);
        return;
      case D_DOWN:
        snake = addNode(food.x, food.y + 4);
        return;
    }
  }
}

void renderFood() {
  screen.drawLine(food.x, food.y, food.x + 3, food.y + 3);
  screen.drawLine(food.x + 3, food.y, food.x, food.y + 3);
}

void handleGame() {
  if(gameInit) {
    snake = addNode(0,0);
    snake = addNode(4,0);
    snake = addNode(8,0);
    snake = addNode(12,0);
    gameInit = false;
  }
  decideSnakeDirection();
  moveSnake();
  checkFoodCollision();
  spawnFood();
}

void renderGame() {
  renderSnake();
  renderFood();
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
  delay(150);

}
