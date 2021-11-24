#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

//LIMITATION
const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HORSE_PER_PLAYER = 4;
const int MAX_ROW = 10;
const int MAX_COL = 10;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME = 7;
const int MAX_CHAR_DISPLAY = 9;
const int MID_NAME_DISPLAY = MAX_CHAR_DISPLAY / 2;
const int MAX_ACCOUNT = 100;

//DISPLAY CHARACTER
const char DEFAULT_CHAR = '?';
const char BLOCK_CHAR = 'B';
const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const char START_CHAR = 'S';
const char FINISH_CHAR = 'F';

int winnerID = -1;
int UserInputInt_ = 0;

struct Player
{
	char Username [MAX_NAME + 1];
	int Win;
	int Tie;

	Player(int win = 0, int tie = 0)
	{
		memset(Username, '\0', MAX_NAME + 1);
		Win = win;
		Tie = tie;
	}
};

Player Accounts_[MAX_ACCOUNT];
int AccountCounter_ = 0;

int OnlineID[MAX_ACCOUNT];
int Online = 0;

void loadAccounts()
{
	ifstream file;
	int counter;
	file.open("accounts.txt");
	if (file.fail()) return;
	file >> counter;
	AccountCounter_ = (counter > 100) ? 100 : counter + 1;
	AccountCounter_++;
	for (int i = 1; i < AccountCounter_; ++i)
	{
		Accounts_[i] = Player();
		file >> Accounts_[i].Username;
		file >> Accounts_[i].Win;
		file >> Accounts_[i].Tie;
	}
	file.close();
}

void saveAccounts()
{
	ofstream file;
	file.open("accounts.txt");
	file << AccountCounter_ << '\n';
	for (int i = 1; i < AccountCounter_; ++i)
	{
		file << Accounts_[i].Username << ' ';
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

struct Horse
{
	int PosX;
	int PosY;
	char DisplayID[MAX_CHAR_DISPLAY + 1];
	int PlayerID;

	Horse(int playerid = 0, int order = 0, int posx = 0, int posy = 0)
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
		strcpy(DisplayID, Accounts_[playerid].Username);
		DisplayID[strlen(DisplayID)] = ' ';
		DisplayID[strlen(DisplayID)] = order + 1 + '0';
		while (strlen(DisplayID) < MAX_CHAR_DISPLAY)
		{
			DisplayID[strlen(DisplayID)] = ' ';
		}
	}
};

struct Point 
{
	int X;
	int Y;
	int HorseID;
	CellEffect Effect;
	int ColorCode;
	int OwnerOrder;
	Point(int x = 0, int y = 0, CellEffect effect = BLOCK,int hourseid = -1, int colorcode = 7, int ownerorder = -1)
	{
		Effect = effect;
		X = x;
		Y = y;
		HorseID = hourseid;
		ColorCode = colorcode;
		OwnerOrder = -1;
	}
};

struct Map
{
	int PlayerID[MAX_PLAYER_PER_MAP];
	Point Grid[MAX_ROW][MAX_COL];
	Horse Horses[MAX_PLAYER_PER_MAP * MAX_HORSE_PER_PLAYER];
	int Size;
	int MaxTurn;
	int NumOfPlayer;
	int HorsePerPlayer;
	int currentTurn;

	Map(int size = 0, int maxturn = 0, int horseperplayer = 1)
	{
		MaxTurn = (maxturn > size*size) ? maxturn : size*size;
		Size = size;
		NumOfPlayer = 0;
		HorsePerPlayer = horseperplayer;
		currentTurn = 0;
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				Grid[i][j].HorseID = -1;
				Grid[i][j].Effect = RIGHT;
			}
		}

		Grid[0][0].Effect = START;
		Grid[Size - 1][Size - 1].Effect = FINISH;

		for (int i = 0; i < MAX_PLAYER_PER_MAP; ++i)
		{
			PlayerID[i] = 0;
		}
	}

	void addAccount()
	{
		cout << "Username? ";
		char name[MAX_NAME + 1];
		int pos = -1;
		cin.getline(name, MAX_NAME + 1, '\n');
		cin.clear();
		cin.ignore(INT_MAX, '\n');
		for (int i = 0; i < AccountCounter_; ++i)
		{
			if (!strcmp(name, Accounts_[i].Username))
			{
				pos = i;
				break;
			}
		}
		if (pos < 0)
		{
			Accounts_[AccountCounter_] = Player();
			strcpy(Accounts_[AccountCounter_].Username, name);
			pos = AccountCounter_;
			AccountCounter_++;
			saveAccounts();
		}
		PlayerID[NumOfPlayer] = pos;
		NumOfPlayer++;
	}

	void initialize()
	{
		for (int i = 0; i < NumOfPlayer * HorsePerPlayer; ++i)
		{
			Horses[i] = Horse(PlayerID[i / (NumOfPlayer * HorsePerPlayer)], i % (NumOfPlayer * HorsePerPlayer));
		}
	}

	void loadMap(int id)
	{
		ifstream file;
		int x;
		switch (id)
		{
		case 1:
			file.open("defaultthreethree.txt");
			break;
		case 2:
			file.open("defaultfivefive.txt");
			break;
		case 3:
			file.open("defaultsevenseven.txt");
			break;
		case 4:
			file.open("spiralthreethree.txt");
			break;
		case 5:
			file.open("spiralfivefive.txt");
			break;
		case 6:
			file.open("spiralsevenseven.txt");
			break;
		}
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

	void moveHorse(const int& ID, const int& desX, const int& desY, const int Count)
	{
		if (isValidPath(ID, Horses[ID].PosX, Horses[ID].PosY, desX, desY, Count))
		{
			if (Horses[Grid[desX][desY].HorseID].DisplayID != Horses[ID].DisplayID)
			{
				if (Grid[desX][desY].HorseID != -1)
				{
					Horses[Grid[desX][desY].HorseID].PosX = -1;
					Horses[Grid[desX][desY].HorseID].PosY = -1;
					cout << Horses[Grid[desX][desY].HorseID].DisplayID << " has been kicked by " << Horses[ID].DisplayID << '\n';
				}
				Horses[ID].PosX = desX;
				Horses[ID].PosY = desY;
				Grid[desX][desY].HorseID = ID;
				if (Grid[desX][desY].Effect == FINISH)
				{
					winnerID = Horses[ID].PlayerID;
				}
			}
		}
		cout << "You cannot go to this point. Please try again or end your turn\n"; 
	}

	bool isValidPath(const int &ID, const int& startX, const int& startY, const int& desX, const int& desY, const int Count)
	{
		if (startX < 0 || startX >= Size || startY < 0 || startY >= Size)
			return false;
		if (Grid[desX][desY].HorseID != -1 &&
			Horses[Grid[desX][desY].HorseID].DisplayID == Horses[ID].DisplayID)
			return false;
		else if (!Count && (startX != desX) && (startY != desY)) 
			return false;
		else if (!Count && ((startX == desX) && (startY == desY))) 
			return true;
		else if (Grid[startX][startY].HorseID != -1) 
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
		case START:
			return isValidPath(ID, startX, startY + 1, desX, desY, Count - 1);
		default:
			return false;
		}
	}

	void printHeader()
	{
		const int SIZE = MAX_CHAR_DISPLAY + 2;
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
			const int DISPLAY_SIZE = MAX_CHAR_DISPLAY;
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
			for (int i = 0; i < MAX_CHAR_DISPLAY; ++i)
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
			return BLOCK_CHAR;
		case UP:
			return UP_CHAR;
		case DOWN:
			return DOWN_CHAR;
		case LEFT:
			return LEFT_CHAR;
		case RIGHT:
			return RIGHT_CHAR;
		case START:
			return START_CHAR;
		case FINISH:
			return FINISH_CHAR;
		default:
			return DEFAULT_CHAR;
		}
	}

	void printNamedVerticalLine(int order = 0)
	{
		cout << " " << ((order < 10) ? "0" : "") << (order + 1) << " ";
		char show[MAX_CHAR_DISPLAY];
		for (int n = 1; n <= Size; ++n)
		{
			if (Grid[order][n - 1].HorseID == -1 || Grid[order][n - 1].Effect == START)
			{
				for (int i = 0; i < MAX_CHAR_DISPLAY; ++i)
				{
					show[i] = ' ';
				}
				show[MID_NAME_DISPLAY] = getCharFromEffect(Grid[order][n - 1].Effect);
			}
			else
			{
				for (int i = 0; i < MAX_CHAR_DISPLAY; ++i)
				{
					show[i] = Horses[Grid[order][n - 1].HorseID].DisplayID[i];
				}
			}
			cout << '|';
			for (int i = 0; i < MAX_CHAR_DISPLAY; ++i)
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

Map startGame(int BoardID, int NumOfPlayer, int NumOfHorse)
{
	Map test = Map(0, 1, NumOfHorse);
	test.loadMap(BoardID);
	char x[2];
	for (int i = 0; i < NumOfPlayer; ++i)
	{
		cout << "Fill information of player " << i + 1 << '\n';
		test.addAccount();
	}
	test.initialize();
	return test;
}

Map Preparing()
{
	char UserCharInput_[5];
	int MapID, NumOfPlayer, HorsePerPlayer, DiceFrom, DiceTo;

	Select_map:
	cout << "Select your map by type a number\n";
	cout << "1. Default 3x3 map\n";
	cout << "2. Default 5x5 map\n";
	cout << "3. Default 7x7 map\n";
	cout << "4. Spiral 3x3 map\n";
	cout << "5. Spiral 5x5 map\n";
	cout << "6. Spiral 7x7 map\n";
	cin >> UserCharInput_;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	if (UserCharInput_[0] < '1' || UserCharInput_[0] > '6')
	{
		cout << "Wrong format, please try again!\n";
		goto Select_map;
	}
	MapID = UserCharInput_[0] - '0';
	Select_NumOfPlayer:
	cout << "How many players? (2 -> 4) ";
	cin >> UserCharInput_;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	if (UserCharInput_[0] < '2' || UserCharInput_[0] > '4')
	{
		cout << "Wrong format, please try again!\n";
		goto Select_NumOfPlayer;
	}
	NumOfPlayer = UserCharInput_[0] - '0';
	Select_HorsePerPlayer:
	cout << "How many horse per player? (1 -> 4) ";
	cin >> UserCharInput_;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	if (UserCharInput_[0] < '1' || UserCharInput_[0] > '4')
	{
		cout << "Wrong format, please try again!\n";
		goto Select_HorsePerPlayer;
	}
	HorsePerPlayer = UserCharInput_[0] - '0';
	return startGame(MapID, NumOfPlayer, HorsePerPlayer);
}

int main()
{
	Accounts_[AccountCounter_] = Player();
	Accounts_[AccountCounter_].Username[0] = 'B';
	Accounts_[AccountCounter_].Username[1] = 'o';
	Accounts_[AccountCounter_].Username[2] = 't';
	AccountCounter_++;
	loadAccounts();

	while (true)
	{
		system("cls");
		cout << "In this game, whoever appears first in the FINISH POINT wins.\n";
		cout << "The game will end after square of map's size turns, and all players will tie\n";
		Map MainGame = Preparing();
		MainGame.moveHorse(0, 0, 3, 3);
		MainGame.moveHorse(1, 0, 2, 2);
		MainGame.printMap();
		system("pause");
	}
	return 0;
}