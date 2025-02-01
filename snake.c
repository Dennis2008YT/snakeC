/*Libraries and directives*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define _WIN32_WINNT 0x0601

/*Classes*/
struct Position {
	int x, y;
};
struct Square {
	struct Position pos;
	char c;
	char ctop, cbot;
	char rect[2][4];
};
struct Canvas {
	struct Position ConsoleSize;
	int area;
	char *canv;
};
struct Snake {
	struct Square head;
	struct Square *body;
	struct Position *previousPos;
	int lenght;
};

/*Functions*/
void Start(); //Is called at the start
bool Update(); //Is called every frame
void callUpdate(int); //calls update every frame
void createCanvas(); //creates the canvas
void defaultInitSquare(struct Square*); //initilazies a square
void draw(struct Square); //draws a square
void DrawCanvas(struct Canvas); //prints the canvas
void randPosition(struct Position*); //randomizes a position
bool checkOverlap(struct Square, struct Square); //check if two squares overlap
void saveHighscore();  //saves the highscore to a file
void initSnake(struct Snake*); //initalizes the snake
void setSnakeBodyPos(struct Snake*); //sets the positions for the snakes body
bool Input(); //gets the input from the user
void move(struct Position*); //changes a position
void freePointers(); //frees the memory taken with malloc()

/*Global variables and constants*/
#define FPS 30
#define FILEHS "snakeHighscore.txt"
#define squareCharacter 219
#define squareCharacterTop 223
#define squareCharacterBottom 220
struct Canvas canvas;
struct Square apple;
struct Snake snake;
int score = 0;
int highscore;
char direction = 'R'; //the direction the snake if going towards [R=Right, L=Left, U=Up, D=Down]

int main() {
	Start();
	callUpdate(FPS);
	if(score>highscore) {
		highscore = score;
		saveHighscore();
	}
	freePointers();
	return 0;
}

void Start() {
	srand(time(NULL));
	//creates the canvas and initializes stuff
		createCanvas();
		defaultInitSquare(&apple);
		initSnake(&snake);
		randPosition(&apple.pos);
	//takes the highscore from a file and puts it in a variable
		FILE *fileHighscore = fopen(FILEHS, "r");
		fscanf(fileHighscore, "%i", &highscore);
		fclose(fileHighscore);
}

bool Update() {
	DrawCanvas(canvas); //draws the canvas
	//checks the overlapping of the apple and of the body with the head of the snake
	for(int i = 0; i < snake.lenght; i++) { //checks overlap of head with body
		if(checkOverlap(snake.head, snake.body[i])) return false;
	}
    if(checkOverlap(snake.head, apple)) { //checks overlap of head with apple
		score++;
		randPosition(&apple.pos);
		snake.lenght++;
		snake.body = (struct Square*)realloc(snake.body, snake.lenght*sizeof(struct Square));
		snake.previousPos = (struct Position*)realloc(snake.previousPos, (snake.lenght+1)*sizeof(struct Position));
		defaultInitSquare(&snake.body[snake.lenght-1]);
	}
	//takes the input from the player and moves the snake
	if(!Input()) return false;
	move(&snake.head.pos);
	//setSnakeBodyPos(&snake);
	return true;
}

void setSnakeBodyPos(struct Snake *snk)
{
	snk->previousPos[0] = snk->head.pos;
	for(int i = 1; i < snk->lenght; i++)
	{
		snk->previousPos[i] = snk->body[i-1].pos;
		snk->previousPos[i].x -= 3;
		snk->previousPos[i].y -= 1;
	}
	move(&snake.head.pos);
	//snk->body[0].pos = snk->previousPos[0];
	/*for(int i = 0; i < snk->lenght; i++)
	{
		snk->body[i+1].pos = snk->previousPos[i];
	}*/
}

void freePointers() {
	free(snake.body);
	snake.body = NULL;
	free(canvas.canv);
	canvas.canv = NULL;
}

bool Input() {
	if(GetKeyState('A') & 0x8000) {
		if(direction != 'R') {
			direction = 'L';
		}
		return true;
    }
	else if(GetKeyState('D') & 0x8000) {
		if(direction != 'L') {
			direction = 'R';
		}
		return true;
    }
    else if(GetKeyState('W') & 0x8000) {
		if(direction != 'D') {
			direction = 'U';
		}
		return true;
    }
    else if(GetKeyState('S') & 0x8000) {
		if(direction != 'U') {
			direction = 'D';
		}
		return true;
    }
    else if(GetKeyState('\x1B') & 0x8000) {
		return false;
    }
	return true;
}

void move(struct Position *pos) {
	switch(direction) {
		case 'U': {
			if(pos->y <= 0) {
				pos->y += canvas.ConsoleSize.y-2;
			}
			else {
				pos->y -= 1;
			}
			break;
		}
		case 'D': {
			if(pos->y >= canvas.ConsoleSize.y - 2) {
				pos->y = 0;
			}
			else {
				pos->y += 1;
			}
			break;
		}
		case 'R': pos->x += 2; break;
		case 'L': pos->x -= 2; break;
	}
}

void initSnake(struct Snake *snk) {
	defaultInitSquare(&snk->head);
	snk->lenght = 0;
	snk->body = (struct Square*)malloc(0);
	snk->previousPos = (struct Position*)malloc(sizeof(struct Position));
}

void saveHighscore() {
	FILE *fileHighscore = fopen(FILEHS, "w");
	fprintf(fileHighscore, "%i\r", score);
	fclose(fileHighscore);
	free(fileHighscore);
}

void defaultInitSquare(struct Square *square) {
	square->c = 219;
	for(int i = 0; i < sizeof(square->rect)/sizeof(square->rect[0]); i++) {
		for(int j = 0; j < sizeof(square->rect[0]); j++) {
			square->rect[i][j] = square->c;
		}
	}
	square->pos.x = 0; square->pos.y = 0;
}

void createCanvas() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	canvas.ConsoleSize.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	canvas.ConsoleSize.y = csbi.srWindow.Bottom - csbi.srWindow.Top;
	canvas.area = canvas.ConsoleSize.x * canvas.ConsoleSize.y;
	canvas.canv = (char*)malloc(canvas.area+1);
	for(int i = 0; i < canvas.area; i++) {
		canvas.canv[i] = ' ';
	}
	canvas.canv[canvas.area] = '\0';
}

void callUpdate(int fps) {
	bool repeat = true;
	while(repeat) {
		bool acceptableTimeDiff = false;
		clock_t now = clock();
		while(!acceptableTimeDiff) {
			clock_t now2 = clock();
			if((now2 - now) >= (float)1000/(float)fps) {
				acceptableTimeDiff = true;
			}
		}
		repeat = Update();
	}
}

bool checkOverlap(struct Square sq1, struct Square sq2) {
	for(int i = 0; i < (sizeof(sq1.rect)/sizeof(sq1.rect[0])); i++) {
		for(int j = 0; j < sizeof(sq1.rect[0]); j++) {
			for(int i2 = 0; i2 < (sizeof(sq1.rect)/sizeof(sq2.rect[0])); i2++) {
				for(int j2 = 0; j2 < sizeof(sq2.rect[0]); j2++) {
					if(sq1.pos.x + ((sq1.pos.y + i) * canvas.ConsoleSize.x) + j == sq2.pos.x + ((sq2.pos.y + i2) * canvas.ConsoleSize.x) + j2) {
						return true;
					}
				}		
			}
		}
	}
	return false;
}

void DrawCanvas(struct Canvas canvas) {
	createCanvas();
	draw(apple);
	draw(snake.head);
	for(int i = 0; i < snake.lenght; i++) {
		draw(snake.body[i]);
	}
	system("cls");
	printf("Score: %i\n%s", score, canvas.canv);
}

void draw(struct Square sq) {
	for(int i = 0; i < (sizeof(sq.rect)/sizeof(sq.rect[0])); i++) {
		for(int j = 0; j < sizeof(sq.rect[0]); j++) {
			canvas.canv[sq.pos.x + ((sq.pos.y + i) * canvas.ConsoleSize.x) + j] = sq.rect[i][j];
		}
	}
}

void randPosition(struct Position *pos) {
	pos->x = rand()%(canvas.ConsoleSize.x-1);
	pos->y = rand()%(canvas.ConsoleSize.y-1);
}