#include <string>

using namespace std;

struct compai{//computer temporary data for tactic1 and tactic2
	int shootcandidates[10][2];//used to sink ship if it is found
	int numcandidates = 0;//how many positions above are
	int nextfield[4][2];//used to cross-like attack for tactic2
	int nextf = 0;//how many positions above are
	int tactic = 0;//difficulty level
};
struct ship{//ship containing its coordinates and size
	int xy[4][2];
	int hits;
};
struct board{//board containing every field status ('=' - empty/unknown, '.' - miss, 1234 - unharmed part of ship, '!' - hit part of ship, '\' - sunk ship), placed ships (unless it is shooting board) and remaining ships on board
	char fields[10][10]={0};//10x10 board
	ship ships[10];//ships hidden within board
	int iship;//remaining ships during game
};
struct cursorxy{//player cursor, moved by wsad/arrows input
	int x;
	int y;
};
class Battleships{
	//class containing all data and functions
	public://only one action is public - constructor
		Battleships(){
			run();
			delete this;
		}

	private:
		cursorxy cursor;
		compai ai;
		string message = "";//messages displayed during game

		void screene(int i);
		void screen(board *b, int kk);
		void screend(board *b, board *bb, int code);
		bool checkpoint(board *b, int x, int y);
		bool singleplacement(board *b, int k[4][2], int toplace, int kk);
		void com_placement(board *b);
		bool psplacement(board *b, int size, int kk);
		void my_placement(board *b);
		void shipsink(board *b, int id, board *bb);
		void shiphit(board *b, board *bb);
		bool nextfrepeat(int x, int y);
		void compmove(board *b, board *bb);
		void clearboard(board *b);
		void selectopt(board *b);
		char endgame(int e, board *myb, board *sb, board *cpb, board *csb);
		int run();
};


int main(){
	Battleships *game = new Battleships;
	return 0;
}
