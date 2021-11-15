#include <iostream>
#include <Windows.h>

using namespace std;

const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HOURSE_PER_PLAYER = 4;
const int MAX_ROW = 4;
const int MAX_COL = 4;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME_DISPLAY = 10;

enum CellEffect
{
	BLOCK,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	START,
	FINISH
};

struct Settings
{

};

struct Hourse
{
	int PosX;
	int PosY;
	string PlayerID;

	Hourse(int posx = 0, int posy = 0, string playerid = "")
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
	}
};

struct Player
{
	string Username;
	string Password;
	string Name;
	int Score;
	
	Player(string username = "", string password = "", string name = "", int score = 0,
			int startx = 0, int starty = 0)
	{
		Username = username;
		Password = password;
		Name = name;
		Score = score;
	}

	bool canLogin(string usename, string password)
	{
		return (usename == Username) && (password == Password);
	}
};

struct Point 
{
	int X;
	int Y;
	int HourseID;
	CellEffect Effect;
	Point(int x = 0, int y = 0, CellEffect effect = BLOCK,int hourseid = -1)
	{
		Effect = effect;
		X = x;
		Y = y;
		HourseID = hourseid;
	}
};


struct Map
{
	int PlayerID[MAX_PLAYER_PER_MAP];
	Point Grid[MAX_ROW][MAX_COL];
	Hourse Hourses[MAX_PLAYER_PER_MAP * MAX_HOURSE_PER_PLAYER];
	int Size;

	Map(int size = 0)
	{
		Size = size;
	}

	bool isValid(const int& startX, const int& startY, const int& desX, const int& desY, const int Count)
	{
		if (!Count && (startX != desX) && (startY != desY)) return false;
		else if ((startX == desX) && (startY == desY)) return true;
		switch (Grid[startY][startX].Effect)
		{
		case UP:
			if (startX - 1 < 0) return false;
			return isValid(startX - 1, startY, desX, desY, Count - 1);
		case DOWN:
			if (startX + 1 >= Size) return false;
			return isValid(startX + 1, startY, desX, desY, Count - 1);
		case LEFT:
			if (startY - 1 < 0) return false;
			return isValid(startX, startY - 1, desX, desY, Count - 1);
		case RIGHT:
			if (startY + 1 >= Size) return false;
			return isValid(startX, startY + 1, desX, desY, Count - 1);
		default:
			return false;
		}
	}
};

int main()
{

}