#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <string>

using namespace std;

const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HOURSE_PER_PLAYER = 4;
const int MAX_ROW = 4;
const int MAX_COL = 4;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME_DISPLAY = 10;

const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const string FINISH_STRING = "FINISH";
const string START_STRING = "START";

bool haveWinner = false;
string Winner = "?";

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
	char DisplayID[MAX_NAME_DISPLAY];
	string PlayerID;

	Hourse(int posx = 0, int posy = 0, string playerid = "")
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
		for (int i = 0; i < (playerid.size() < MAX_NAME_DISPLAY) ? playerid.size() : MAX_NAME_DISPLAY; ++i)
		{
			DisplayID[i] = playerid[i];
		}
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
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				Grid[i][j].HourseID = -1;
				Grid[i][j].Effect = RIGHT;
			}
		}

		for (int i = 0; i < MAX_PLAYER_PER_MAP; +i)
		{
			PlayerID[i] = -1;
		}
	}

	void saveMap(string file_name)
	{
		ofstream file;
		file.open(file_name);
		file << Size << '\n';
		for (int i = 0; i < Size; ++i)
		{
			for (int j = 0; j < Size; ++j)
			{
				file << (int)Grid[i][j].Effect << ' ';
			}
			file << '\n';
		}
		file.close();
	}

	void loadMap(string file_name)
	{
		ifstream file;
		int x;
		file.open(file_name);
		file >> Size;
		for (int i = 0; i < Size; ++i)
		{
			for (int j = 0; j < Size; ++j)
			{
				file >> x;
				Grid[i][j].Effect = (CellEffect)x;
			}
		}
		file.close();
	}

	void moveHourse(const int& ID, const int& desX, const int& desY, const int Count)
	{
		if (isValidPath(ID, Hourses[ID].PosX, Hourses[ID].PosY, desX, desY, Count))
		{
			if (Hourses[Grid[desX][desY].HourseID].DisplayID != Hourses[ID].DisplayID)
			{
				if (Grid[desX][desY].HourseID != -1)
				{
					Hourses[Grid[desX][desY].HourseID].PosX = -1;
					Hourses[Grid[desX][desY].HourseID].PosY = -1;
				}
				Hourses[ID].PosX = desX;
				Hourses[ID].PosY = desY;
				Grid[desX][desY].HourseID = ID;
				if (Grid[desX][desY].Effect == FINISH)
				{
					haveWinner = true;
				}
			}
		}
		else cout << "You cannot go to this point. Please try again or end your turn\n"; 
	}

	bool isValidPath(const int &ID, const int& startX, const int& startY, const int& desX, const int& desY, const int Count)
	{
		if (Grid[desX][desY].HourseID != -1 &&
			Hourses[Grid[desX][desY].HourseID].DisplayID == Hourses[ID].DisplayID)
			return false;
		else if (!Count && (startX != desX) && (startY != desY)) 
			return false;
		else if (!Count && ((startX == desX) && (startY == desY))) 
			return true;
		else if (Grid[startX][startY].HourseID != -1) 
			return false;
		switch (Grid[startY][startX].Effect)
		{
		case UP:
			if (startX - 1 < 0) return false;
			return isValidPath(ID, startX - 1, startY, desX, desY, Count - 1);
		case DOWN:
			if (startX + 1 >= Size) return false;
			return isValidPath(ID, startX + 1, startY, desX, desY, Count - 1);
		case LEFT:
			if (startY - 1 < 0) return false;
			return isValidPath(ID, startX, startY - 1, desX, desY, Count - 1);
		case RIGHT:
			if (startY + 1 >= Size) return false;
			return isValidPath(ID, startX, startY + 1, desX, desY, Count - 1);
		default:
			return false;
		}
	}
};

int main()
{
	return 0;
}