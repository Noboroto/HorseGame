#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>

using std::cin;
using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;

//CONSTANTS FILE NAME
const string ACCOUNTS_FILE = "accounts.txt";

//LIMITATION
const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HOURSE_PER_PLAYER = 4;
const int MAX_ROW = 10;
const int MAX_COL = 10;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME_DISPLAY = 9;
const int MID_NAME_DISPLAY = MAX_NAME_DISPLAY / 2;
const int MAX_ACCOUNT = 100;

//DISPLAY CHARACTER
const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const string FINISH_STRING = "FINISH";
const string START_STRING = "START";

bool haveWinner_ = false;
string Winner_ = "?";
string Message_ = "";
string UserInputString_ = "";
int UserInputInt_ = 0;

struct Player
{
	string Username;
	string Password;
	string Name;
	int NumOfHourse;
	int Win;
	int Tie;

	Player(string username = "", string password = "", string name = "", int win = 0, int tie = 0, int NumOfHour = MAX_HOURSE_PER_PLAYER)
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
int AccountCounter_ = 0;

void loadAccounts()
{
	ifstream file;
	file.open(ACCOUNTS_FILE);
	file >> AccountCounter_;
	AccountCounter_ = (AccountCounter_ > 100) ? 100 : AccountCounter_;
	string tmp;
	getline(file, tmp);
	for (int i = 0; i < AccountCounter_; ++i)
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
	file << AccountCounter_ << '\n';
	for (int i = 0; i < AccountCounter_; ++i)
	{
		file << Accounts_[i].Username << '\n';
		file << Accounts_[i].Password << '\n';
		file << Accounts_[i].Name << '\n';
		file << Accounts_[i].Win << ' ' << Accounts_[i].Tie << '\n';
	}
	file.close();
}

void createAccount()
{
	cout << "Start registration\n";
	Accounts_[AccountCounter_] = Player();
	bool isOK = false;
	while (!isOK)
	{	
		cout << "Username? ";
		isOK = true;
		cin >> UserInputString_;
		for (int i = 0; i < AccountCounter_; ++i)
		{
			if (Accounts_[i].Username == UserInputString_)
			{
				isOK = false;
				cout << "This username is exist! Please try another username!\n"; 
				break;
			}
		}
	}
	Accounts_[AccountCounter_].Username = UserInputString_;
	cout << "Password? ";
	cin >> UserInputString_;
	Accounts_[AccountCounter_].Password = UserInputString_;
	cout << "Name? ";
	cin >> UserInputString_;
	Accounts_[AccountCounter_].Name = UserInputString_;
	cout << "Registed!";
	AccountCounter_++;
	saveAccounts();
}

void removeAccount(string username = "")
{
	if (username == "")
	{
		cout << "Please enter the account's username you want to remove! ";
		cin >> username;
	}
	int id = -1;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (Accounts_[i].Username == username)
		{
			id = i;
			break;
		}
	}
	if (id == -1)
	{
		cout << "Cannot find the account! Removing account ended!\n";
	}
	else
	{
		bool isOK = false;
		cout << "Password? ";
		cin >> UserInputString_;
		if (UserInputString_ != Accounts_[id].Password) cout << "Wrong Password! Exit removing account!\n";
		else isOK = true;
		AccountCounter_--;
		for (int i = id; i < AccountCounter_; ++i)
		{
			Accounts_[i] = Accounts_[i + 1];
		}
		cout << "Removed!\n";
		saveAccounts();
	}
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

	Hourse(int playerid = 0, int order = 0, int posx = 0, int posy = 0)
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
		string tmp = Accounts_[playerid].Name;
		for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
		{
			DisplayID[i] = (i < tmp.size()) ? tmp[i] : ' ';
		}
		DisplayID[MAX_NAME_DISPLAY - 2] = ' ';
		DisplayID[MAX_NAME_DISPLAY - 1] = order + 1 + '0';
	}
};

struct Point 
{
	int X;
	int Y;
	int HourseID;
	CellEffect Effect;
	int ColorCode;
	int OwnerOrder;
	Point(int x = 0, int y = 0, CellEffect effect = BLOCK,int hourseid = -1, int colorcode = 7, int ownerorder = -1)
	{
		Effect = effect;
		X = x;
		Y = y;
		HourseID = hourseid;
		ColorCode = colorcode;
		OwnerOrder = -1;
	}
};

struct Map
{
	int PlayerID[MAX_PLAYER_PER_MAP];
	Point Grid[MAX_ROW][MAX_COL];
	Hourse Hourses[MAX_PLAYER_PER_MAP * MAX_HOURSE_PER_PLAYER];
	int Size;
	int MaxTurn;
	int MaxPlayer;
	int NumOfPlayer;

	Map(int size = 0, int maxturn = 0, int maxplayer = 1)
	{
		MaxTurn = (maxturn > size*size) ? maxturn : size*size;
		Size = size;
		MaxPlayer = maxplayer;
		NumOfPlayer = 0;
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				Grid[i][j].HourseID = -1;
				Grid[i][j].Effect = RIGHT;
			}
		}

		for (int i = 0; i < MAX_PLAYER_PER_MAP; ++i)
		{
			PlayerID[i] = -1;
		}
	}

	void AddAccount (string username)
	{
		for (int i = 0; i < AccountCounter_; ++i)
		{
			if (Accounts_[i].Username == username)
			{
				PlayerID[NumOfPlayer] = i;
				NumOfPlayer++;
			}
		}
	}

	void initialize()
	{
		for (int i = 0; i < MAX_PLAYER_PER_MAP * MAX_HOURSE_PER_PLAYER; ++i)
		{
			Hourses[i] = Hourse(PlayerID[i / MAX_HOURSE_PER_PLAYER], i % MAX_HOURSE_PER_PLAYER);
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
					string loser = Hourses[Grid[desX][desY].HourseID].DisplayID;
					string winner = Hourses[ID].DisplayID;
					Hourses[Grid[desX][desY].HourseID].PosX = -1;
					Hourses[Grid[desX][desY].HourseID].PosY = -1;
					Message_ += loser + " has been kicked by " + winner + '\n';
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
		else Message_ += "You cannot go to this point. Please try again or end your turn\n"; 
	}

	bool isValidPath(const int &ID, const int& startX, const int& startY, const int& desX, const int& desY, const int Count)
	{
		if (startX < 0 || startX >= Size || startY < 0 || startY >= Size)
			return false;
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
			return isValidPath(ID, startX - 1, startY, desX, desY, Count - 1);
		case DOWN:
			return isValidPath(ID, startX + 1, startY, desX, desY, Count - 1);
		case LEFT:
			return isValidPath(ID, startX, startY - 1, desX, desY, Count - 1);
		case RIGHT:
			return isValidPath(ID, startX, startY + 1, desX, desY, Count - 1);
		default:
			return false;
		}
	}

	void printHeader()
	{
		const int SIZE = MAX_NAME_DISPLAY + 2;
		char pattern[SIZE];
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int i = 0; i < SIZE; ++i)
		{
			pattern[i] = ' ';
		}
		for (int i = 1; i <= Size; ++i)
		{
			pattern[(SIZE / 2) - 1] = i / 10 + '0';
			pattern[(SIZE / 2)] = i % 10 + '0';
			for (int j = 0; j < SIZE; ++j) cout << pattern[j];
		}
		cout << '\n';
	}

	void printRowhorizontalLine()
	{
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int n = 1; n <= Size; ++n)
		{
			cout << ' ';
			const int DISPLAY_SIZE = MAX_NAME_DISPLAY;
			for (int i = 0; i < DISPLAY_SIZE; ++i)
			{
				cout << '-';
			}
			cout << ' ';
		}
		cout << '\n';
	}

	void printBlankVerticalLine()
	{
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int n = 1; n <= Size; ++n)
		{
			cout << '|';
			for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
			{
				cout << ' ';
			}
			cout << '|';
		}
		cout << '\n';
	}

	char getCharFromEffect(CellEffect effect)
	{
		switch (effect)
		{
		case BLOCK:
			return 'B';
		case UP:
			return UP_CHAR;
		case DOWN:
			return DOWN_CHAR;
		case LEFT:
			return LEFT_CHAR;
		case RIGHT:
			return RIGHT_CHAR;
		case START:
			return 'S';
		case FINISH:
			return 'F';
		default:
			return '?';
		}
	}

	void printNamedVerticalLine(int order = 0)
	{
		cout << " " << ((order < 10) ? "0" : "") << (order + 1) << " ";
		char show[MAX_NAME_DISPLAY];
		for (int n = 1; n <= Size; ++n)
		{
			if (Grid[order][n - 1].HourseID == -1)
			{
				for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
				{
					show[i] = ' ';
				}
				show[MID_NAME_DISPLAY] = getCharFromEffect(Grid[order][n - 1].Effect);
			}
			else
			{
				for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
				{
					show[i] = Hourses[Grid[order][n - 1].HourseID].DisplayID[i];
				}
			}
			cout << '|';
			for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
			{
				cout << show[i];
			}
			cout << '|';
		}
		cout << '\n';
	}

	void printMap()
	{
		printHeader();
		for (int i = 1; i <= Size; ++i)
		{
			printRowhorizontalLine();
			printBlankVerticalLine();
			printNamedVerticalLine(i - 1);
			printBlankVerticalLine();
			printRowhorizontalLine();
		}
	}
};

int main()
{
	createAccount();
	return 0;
}