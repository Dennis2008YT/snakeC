//Librerie e direttive
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define _WIN32_WINNT 0x0601

//Classi

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
struct Coin {
	float value;
	struct Position pos;
	char c;
};
struct Snake {
	struct Position *positions;
	struct Position Head; 
};

//Funzioni
bool Update(); //Is called every frame
void Start(); //Is called at the start
bool Update(); //Is called every frame
void callUpdate(int); //calls update every frame
void createCanvas(); //creates the canvas
void defaultInitSquare(struct Square *square);
void draw(struct Square); //draws the square passed as parameter
void DrawCanvas(struct Canvas); //prints the canvas
void spawnCoin(); //creates a coin
void checkCoin(); //checks if the coin is taken
void salvaFile(FILE*);

//Variabili e costanti globali
#define FPS 60
#define FILESOLDI "soldiKebab.txt"
#define squareCharacter 219
#define squareCharacterTop 223
#define squareCharacterBottom 220
struct Square player;
struct Canvas canvas;
struct Coin coin;
float money = 0;

//The Main
int main()
{
	defaultInitSquare(&player);
    srand(time(NULL));
	Start();
	//FILE *fileSoldi = fopen(FILESOLDI, "w");
	callUpdate(FPS);
	//salvaFile(fileSoldi);
	//fclose(fileSoldi);
	return 0;
}

void Start()
{
	createCanvas();
	spawnCoin();
	//FILE *fileSoldi = fopen(FILESOLDI, "r");
	//fscanf(fileSoldi, "%.2f", &money);
	//fclose(fileSoldi);
}

bool Update()
{
    checkCoin();
	DrawCanvas(canvas);
    if(GetKeyState('A') & 0x8000)
    {
		player.pos.x -= 2;
    }
	else if(GetKeyState('D') & 0x8000)
    {
		player.pos.x += 2;
    }
    else if(GetKeyState('W') & 0x8000)
    {
		if(player.pos.y <= 0)
		{
			player.pos.y += canvas.ConsoleSize.y;
		}
		else
		{
			player.pos.y -= 1;
		}
    }
    else if(GetKeyState('S') & 0x8000)
    {
		if(player.pos.y >= canvas.ConsoleSize.y)
		{
			player.pos.y -= canvas.ConsoleSize.y;
		}
		else
		{
			player.pos.y += 1;
		}
    }
    else if(GetKeyState('\x1B') & 0x8000)
    {
		return false;
    }
	return true;
}

void defaultInitSquare(struct Square *square)
{
	square->c = 219;
	for(int i = 0; i < sizeof(square->rect)/sizeof(square->rect[0]); i++)
	{
		for(int j = 0; j < sizeof(square->rect[0]); j++)
		{
			square->rect[i][j] = square->c;
		}
	}
	square->pos.x = 5; square->pos.y = 5;
}

void createCanvas()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	canvas.ConsoleSize.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	canvas.ConsoleSize.y = csbi.srWindow.Bottom - csbi.srWindow.Top;
	canvas.area = canvas.ConsoleSize.x * canvas.ConsoleSize.y;
	canvas.canv = (char*)malloc((canvas.area+1)*sizeof(char));
	for(int i = 0; i < canvas.area; i++)
	{
		canvas.canv[i] = ' ';
	}
	canvas.canv[canvas.area] = '\0';
}

void callUpdate(int fps) { //function to call the update at a fixed frame rate, used to keep clean both the Update() and the main() functions
	//Calls Update at a fixed frame rate
	bool repeat = true;
	while(repeat)
	{
		bool acceptableTimeDiff = false;
		clock_t now = clock();
		while(!acceptableTimeDiff)
		{
			clock_t now2 = clock();
			if((now2 - now) >= (float)1000/(float)fps)
			{
				acceptableTimeDiff = true;
			}
		}
		repeat = Update();
	}
}

void checkCoin()
{
	for(int i = 0; i < (sizeof(player.rect)/sizeof(player.rect[0])); i++)
	{
		for(int j = 0; j < sizeof(player.rect[0]); j++)
		{
			if(coin.pos.x + coin.pos.y == player.pos.x + ((player.pos.y + i) * canvas.ConsoleSize.x) + j)
			{
				money += coin.value;
				spawnCoin();
			}
		}
	}
}

void DrawCanvas(struct Canvas canvas)
{
	createCanvas();
	canvas.canv[coin.pos.x + coin.pos.y] = coin.c;
	draw(player);
	system("cls");
	printf("Money: %.2f\n%s", money, canvas.canv);
}

void draw(struct Square sq)
{
	for(int i = 0; i < (sizeof(sq.rect)/sizeof(sq.rect[0])); i++)
	{
		for(int j = 0; j < sizeof(sq.rect[0]); j++)
		{
			canvas.canv[sq.pos.x + ((sq.pos.y + i) * canvas.ConsoleSize.x) + j] = sq.rect[i][j];
		}
	}
}

void spawnCoin()
{
	coin.pos.x = rand()%canvas.ConsoleSize.x;
	coin.pos.y = (rand()%canvas.ConsoleSize.y) * canvas.ConsoleSize.x;
	coin.value = (rand()%5==0) ? 1 : 0.5;
	if(coin.value == 1)
	{
		coin.c = 'O';
	}
	else
	{
		coin.c = 'o';
	}
}