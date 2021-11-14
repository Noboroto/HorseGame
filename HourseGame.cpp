#include <iostream>
#include <Windows.h>

using namespace std;

const int HOURSES_PER_PLAYER = 4;
const int MAX_ROW = 4;
const int MAX_COL = 4;
const int EDGE_OF_POINT = 10;
const int SPACE_BETWEEN_POINT = 2;

struct Settings
{

};

struct Hourse
{
	int PosX;
	int PosY;
	Hourse(int posx = 0, int posy = 0)
	{
		PosX = posx;
		PosY = posy;
	}
};

struct Player
{
	string Username;
	string Password;
	string Name;
	int Score;
	
	Hourse OwnHourse[HOURSES_PER_PLAYER];

	Player(string username = "", string password = "", string name = "", int score = 0,
			int startx = 0, int starty = 0)
	{
		Username = username;
		Password = password;
		Name = name;
		Score = score;
		for (int i = 0; i < HOURSES_PER_PLAYER; ++i)
		{
			OwnHourse[i] = Hourse(startx, starty);
		}
	}

	bool isCorrect(string usename, string password)
	{
		return (usename == Username) && (password == Password);
	}
};

struct Point 
{
	int X;
	int Y;
	Point(int x = 0, int y = 0)
	{
		X = x;
		Y = y;
	}
};


struct Map
{
	Player Players[4];
	Point Grid[MAX_ROW][MAX_COL];
	int Size;

	Map(int size = 0)
	{
		Size = 0;
	}
};

int main()
{

}