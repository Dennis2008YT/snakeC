//Librerie e direttive
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define _WIN32_WINNT 0x0601
using namespace std;

//Classi
class Position
{
	public:
	int x, y;
};

class Square
{
	public:
	Position pos;
	char c = 219;
	char ctop = 223, cbot= 220;
	char rect[2][5] = {{c, c, c, c, '\0'}, {c, c, c, c, '\0'}};
};

class Coin
{
	public:
	float value;
	Position pos;
	char c;
};

class Canvas
{
	public:
	Position ConsoleSize;
	int area = ConsoleSize.x * ConsoleSize.y;
	char *canv;
};

//Funzioni
bool Update(); //Is called every frame
void callUpdate(); //calls update every frame
void Start(); //Is called at the start
void draw(Square); //draws the square passed as parameter
void DrawCanvas(); //prints the canvas
void createCanvas(); //creates the canvas
void spawnCoin(); //creates a coin
void checkCoin(); //checks if the coin is taken
void salvaFile(FILE*);

//Variabili e costanti globali
#define FPS 60
#define FILESOLDI "soldiKebab.txt"
Square player;
Canvas canvas;
Coin coin;
float money = 0;

//The Main
int main()
{
    srand(time(NULL));
	Start();
	callUpdate();
	return 0;
}

void Start()
{
	createCanvas();
	spawnCoin();
	player.pos.x = 5; player.pos.y = 5;
	FILE *fileSoldi = fopen(FILESOLDI, "r");
	fscanf(fileSoldi, "%.2f", &money);
	fclose(fileSoldi);
}

bool Update()
{
    checkCoin();
	DrawCanvas();
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

void salvaFile(FILE *fileSoldi)
{
	fprintf(fileSoldi, "%.2f\r", money);
}

void checkCoin()
{
	for(int i = 0; i < (sizeof(player.rect)/sizeof(player.rect[0])); i++)
	{
		for(int j = 0; j < sizeof(player.rect[0]) - 1; j++)
		{
			if(coin.pos.x + coin.pos.y == player.pos.x + ((player.pos.y + i) * canvas.ConsoleSize.x) + j)
			{
				money += coin.value;
				spawnCoin();
			}
		}
	}
}

void DrawCanvas()
{
	createCanvas();
	canvas.canv[coin.pos.x + coin.pos.y] = coin.c;
	draw(player);
	system("cls");
	printf("Money: %.2f\n%s", money, canvas.canv);
}

void draw(Square sq)
{
	for(int i = 0; i < (sizeof(player.rect)/sizeof(player.rect[0])); i++)
	{
		for(int j = 0; j < sizeof(player.rect[0]) - 1; j++)
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

void createCanvas()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	canvas.ConsoleSize.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	canvas.ConsoleSize.y = csbi.srWindow.Bottom - csbi.srWindow.Top;
	canvas.area = canvas.ConsoleSize.x * canvas.ConsoleSize.y;
	canvas.canv = new char[canvas.area];
	for(int i = 0; i < canvas.area; i++)
	{
		canvas.canv[i] = ' ';
	}
	canvas.canv[canvas.area-1] = '\0';
}

void callUpdate() //function to call the update at a fixed frame rate, used to keep clean both the Update() and the main() functions
{
	//Calls Update at a fixed frame rate
	FILE *fileSoldi = fopen(FILESOLDI, "w");
	bool repeat = true;
	while(repeat)
	{
		bool acceptableTimeDiff = false;
		clock_t now = clock();
		while(!acceptableTimeDiff)
		{
			clock_t now2 = clock();
			if((now2 - now) >= (float)1000/(float)FPS)
			{
				acceptableTimeDiff = true;
			}
		}
		repeat = Update();
	}
	salvaFile(fileSoldi);
	fclose(fileSoldi);
}