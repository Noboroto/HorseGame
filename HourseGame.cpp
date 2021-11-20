#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

//CONSTANTS FILE NAME
const string ACCOUNTS_FILE = "accounts.txt";

//LIMITATION
const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HOURSE_PER_PLAYER = 4;
const int MAX_ROW = 4;
const int MAX_COL = 4;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME_DISPLAY = 10;
const int MAX_ACCOUNT = 100;

//DISPLAY CHARACTER
const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const string FINISH_STRING = "FINISH";
const string START_STRING = "START";

bool haveWinner_ = false;
string Winner = "?";

struct Player
{
	string Username;
	string Password;
	string Name;
	int Win;
	int Tie;

	Player(string username = "", string password = "", string name = "", int win = 0, int tie = 0)
	{
		Username = username;
		Password = password;
		Name = name;
		Win = win;
		Tie = tie;
	}

	bool canLogin(string usename, string password)
	{
		return (usename == Username) && (password == Password);
	}
};

Player Accounts_[MAX_ACCOUNT];
int AccountCounter;

void loadAccounts()
{
	ifstream file;
	file.open(ACCOUNTS_FILE);
	file >> AccountCounter;
	AccountCounter = (AccountCounter > 100) ? 100 : AccountCounter;
	string tmp;
	getline(file, tmp);
	for (int i = 0; i < AccountCounter; ++i)
	{
		Accounts_[i] = Player();
		getline(file, tmp);
		Accounts_[i].Username = tmp;
		getline(file, tmp);
		Accounts_[i].Password = tmp;
		getline(file, tmp);
		Accounts_[i].Name = tmp;
		file >> Accounts_[i].Win;
		file >> Accounts_[i].Tie;
	}
	file.close();
}

void saveAccounts()
{
	ofstream file;
	file.open(ACCOUNTS_FILE);
	file << AccountCounter << '\n';
	for (int i = 0; i < AccountCounter; ++i)
	{
		file << Accounts_[i].Username << '\n';
		file << Accounts_[i].Password << '\n';
		file << Accounts_[i].Name << '\n';
		file << Accounts_[i].Win << ' ' << Accounts_[i].Tie << '\n';
	}
	file.close();
}

int getDice(int from = 1, int to = 6)
{
	srand(time(NULL));
	return rand() % (to - from + 1) + from;
}

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
	int PlayerID;

	Hourse(int posx = 0, int posy = 0, int playerid = 0)
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
		string tmp = Accounts_[playerid].Name;
		for (int i = 0; i < (tmp.size() < MAX_NAME_DISPLAY) ? tmp.size() : MAX_NAME_DISPLAY; ++i)
		{
			DisplayID[i] = tmp[i];
		}
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
	int MaxTurn;

	Map(int size = 0, int maxturn = 0)
	{
		MaxTurn = (maxturn > size*size) ? maxturn : size*size;
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
					haveWinner_ = true;
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

	void printMap()
	{

	}
};

int main()
{
	return 0;
}