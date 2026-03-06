#include <time.h>
#include <string>
#include <ncurses.h>
#include <cstring>

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

		void screene(int i){//clears whole screen, prints only text, used twice
			clear();
			if (i){
				mvprintw(2,0,"Select your ships placement option");
				mvprintw(3,0,"r - random placement by program");
				mvprintw(4,0,"m - manual placement by user");
				move(5,0);
			}
			else{
				mvprintw(0,3,"Welcome in Battleships");
				mvprintw(2,0,"Select difficulty level: 0-2");
				mvprintw(3,0,"0 - completely random attacks");
				mvprintw(4,0,"1 - tries to sunk ship once hit");
				mvprintw(5,0,"2 - uses cross-like attack to find ships");
				move(6,0);
			}
			refresh();
		}

		void screen(board *b, int kk){//prints player board during placement, kk-ship id
			clear();
			move(0,0);
			char ctemp[message.length()+1];
			char csingle[1];
			strcpy(ctemp, message.c_str());
			addstr(ctemp);
			message = "";
			for (int i=0; i<10; i++){
				for (int j=0; j<10; j++){
					csingle[0] = b->fields[i][j];
					if (cursor.x == j && cursor.y == i){
						attron(A_BOLD);
						attron(A_UNDERLINE);
						mvprintw(3+i,3+j,csingle);
						attroff(A_UNDERLINE);
						attroff(A_BOLD);
					}
					else{
						mvprintw(3+i,3+j,csingle);
					}
				}
			}
			mvprintw(14,0,"Set ships coordinations. wsad/ARROWS - change square, x/ENTER - select square.");
			mvprintw(15,0,"The ship may bend but its parts must be connected and it cannot touch other ships.");
			if (kk == 9) mvprintw(16,0,"Current ship to place: 4-masted.");
			else if (kk>6) mvprintw(16,0,"Current ship to place: 3-masted.");
			else if (kk>3) mvprintw(16,0,"Current ship to place: 2-masted.");
			else mvprintw(16,0,"Current ship to place: 1-masted.");
			refresh();
		}

		void screend(board *b, board *bb, int code = 0){//prints ingame boards; b - player board, bb - player shooting board, code - game state
			clear();
			move(0,0);
			char ctemp[message.length()+1];
			char csingle[1];
			strcpy(ctemp, message.c_str());
			addstr(ctemp);
			mvprintw(2,3,"Your board");
			mvprintw(2,17,"Computer board");
			for (int i=0; i<10; i++){
				for (int j=0; j<10; j++){
					csingle[0] = b->fields[i][j];
					mvprintw(3+i,3+j,csingle);
				}
				for (int j=0; j<10; j++){
					csingle[0] = bb->fields[i][j];
					if (cursor.x == j && cursor.y == i){
						attron(A_BOLD);
						attron(A_UNDERLINE);
						mvprintw(3+i,20+j,csingle);
						attroff(A_UNDERLINE);
						attroff(A_BOLD);
					}
					else mvprintw(3+i,20+j,csingle);
				}
			}
			switch(code){
				case 0:
					mvprintw(14,0,"x/ENTER - attack target, wsad/ARROWS - change target,");
					mvprintw(15,0,"q - quit program, r - abandon game");
					break;
				case 1:
					mvprintw(14,0,"You win!");
					mvprintw(15,0,"Play again? y/n ");
					break;
				case 2:
					mvprintw(14,0,"Computer wins!");
					mvprintw(15,0,"Play again? y/n ");
					break;
				case 3:
					mvprintw(14,0,"Left game ... ");
					mvprintw(15,0,"Play again? y/n ");
					break;
			}
			message = "";
			refresh();
		}

		bool checkpoint(board *b, int x, int y){//checks x,y position whether it is fine to place
			if (x<0 || x>9 || y<0 || y>9) return false;
				for (int i=y-1; i<=y+1; i++){
					if (i>9) break;
					for (int j=x-1; j<=x+1; j++){
						if (i<0 || j<0) continue;
						else if (j>9) continue;
						else if (b->fields[i][j] != '=' && b->fields[i][j] != '?'){
							return false;
					}
				}
			}
			return true;
		}

		bool singleplacement(board *b, int k[4][2], int toplace, int kk){//tries to place ship containing first position, b - 	board, k - ship coordinates, toplace - ship size, kk - ship id
			int xyposible[10][2] = {{k[0][0]-1,k[0][1]},{k[0][0]+1,k[0][1]},{k[0][0],k[0][1]-1},{k[0][0],k[0][1]+1}};
			int temp = 4;//candidates counter
			int kpos = 1;//current ship part to place
			int toplacetemp = toplace;//number of remaining parts to place
			while (temp){
				int xy = rand() % temp;
				if (checkpoint(b, xyposible[xy][0], xyposible[xy][1])){
					k[kpos][0] = xyposible[xy][0];
					k[kpos][1] = xyposible[xy][1];
					int newpos[4][2] = {{k[kpos][0]-1,k[kpos][1]},{k[kpos][0]+1,k[kpos][1]},{k[kpos][0],k[kpos][1]-1},{k[kpos][0],k[kpos][1]+1}};
					kpos++;
					if (!--toplacetemp){//is ship fully placed?
						for(int i=0; i<toplace; i++){//write to the board
							b->fields[k[i][1]][k[i][0]] = '0'+toplace;
							b->ships[kk].xy[i][0] = k[i][0];
							b->ships[kk].xy[i][1] = k[i][1];
						}
						b->ships[kk].hits = toplace;
						return true;
					}
					bool bpos;
					for (int j=4; j>0; j--){//check new candidates
						bpos = true;
						for (int i=temp-1; i>=0; i--){
							if (newpos[j][0]<0 || newpos[j][1]<0 || newpos[j][0]>9 || newpos[j][1]>9){
								bpos = false;//out of bounds
								break;
							}
							else if(newpos[j][0] == xyposible[i][0] && newpos[j][1] == xyposible[i][1]){
								bpos = false;//already exists
								break;
							}
							for (int t=kpos; t>=0; t--){
								if (newpos[j][0] == k[t][0] && newpos[j][1] == k[t][1]){
									bpos = false;//already placed
									break;
								}
							}
						}
						if (bpos){//add new candidate
							xyposible[temp][0] = newpos[j][0];
							xyposible[temp][1] = newpos[j][1];
							temp++;
						}
					}
				}
				if (temp>1){//replace chosen candidate with last one
					xyposible[xy][0] = xyposible[temp-1][0];
					xyposible[xy][1] = xyposible[temp-1][1];
				}
				temp--;//removed
			}
			return false;//no candidates left
		}

		void com_placement(board *b){//random ship placement
			int x = random()%10, y = random()%10;
			int iplacements;
			int temp;
			for (int k=10; k>=0; k--){
				if (checkpoint(b,x,y)){//is random position fine?
					int kk[4][2]={{x,y}};//ship parts positions
					if (k==10){
						if (!singleplacement(b, kk, 4, k-1)) k++;
					}
					else if (k>7){
						if (!singleplacement(b, kk, 3, k-1)) k++;
					}
					else if (k>4){
						if (!singleplacement(b, kk, 2, k-1)) k++;
					}
					else if (k>0){
						b->fields[y][x] = '1';
						b->ships[k-1].xy[0][0] = x;
						b->ships[k-1].xy[0][1] = y;
						b->ships[k-1].hits = 1;
					}
					else{
						b->iship = 10;
						return;
					}
				}
				else k++;
				x = random()%10;
				y = random()%10;
			}
		}

		bool psplacement(board *b, int size, int kk){//manual ship placement. b - player board, size - ship size, kk - ship id
			int c;
			int placepoints[4][2] = {0};//ships parts positions
			int temp = 0;//number of placed parts
			screen(b, kk);
			while (temp<size){
				c = getch();
				switch (c){
					case 'w':
					case 'W':
					case KEY_UP:
						if (cursor.y>0) cursor.y--;
						screen(b, kk);
						break;
					case 'a':
					case 'A':
					case KEY_LEFT:
						if (cursor.x>0) cursor.x--;
						screen(b, kk);
						break;
					case 'd':
					case 'D':
					case KEY_RIGHT:
						if (cursor.x<9) cursor.x++;
						screen(b, kk);
						break;
					case 's':
					case 'S':
					case KEY_DOWN:
						if (cursor.y<9) cursor.y++;
						screen(b, kk);
						break;
					case 'x':
					case 'X':
					case KEY_ENTER:
					case '\n':
						if (checkpoint(b, cursor.x, cursor.y)){//if selected position is fine
							placepoints[temp][0] = cursor.x;
							placepoints[temp][1] = cursor.y;
							b->fields[cursor.y][cursor.x] = '?';//position noted
							bool bposible = false;
							for (int i=0; i<temp; i++){//check earlier placed positions ('?')
								int tx = placepoints[i][0]-cursor.x;
								int ty = placepoints[i][1]-cursor.y;
								if (!tx && !ty){
									bposible = false;
									break;
								}
								else if (tx == 1 && !ty || tx == -1 && !ty || !tx && ty == 1 || !tx && ty == -1){
									bposible = true;
								}
							}
							if (!bposible && temp){//if parts are not connected, remove '?'
								for (int j=0; j<=temp; j++) b->fields[placepoints[j][1]][placepoints[j][0]] = '=';
								message = "Incorrext placement.";
								return false;
							}
							screen(b, kk);
							temp++;
						}
						else{
							for (int j=0; j<temp; j++) b->fields[placepoints[j][1]][placepoints[j][0]] = '=';
							message = "Incorrext placement.";
							return false;
						}
				}
			}
			for (int k=0; k<size; k++){//place ship on the board
				b->fields[placepoints[k][1]][placepoints[k][0]] = '0' + size;
				b->ships[kk].xy[k][0] = placepoints[k][0];
				b->ships[kk].xy[k][1] = placepoints[k][1];
			}
			b->ships[kk].hits = size;
			message = "Ship placed.";
			return true;
		}

		void my_placement(board *b){//controlls manual placement. b - player board
			int size;//size of ship
			for (int k=10; k>=0; k--){
				if (k==10){
					size = 4;
				}
				else if (k>7){
					size = 3;
				}
				else if (k>4){
					size = 2;
				}
				else if (k>0){
					size = 1;
				}
				else{
					b->iship = 10;
					message = "All ships are placed properly.";
					return;
				}
				if (!psplacement(b, size, k-1)) k++;//if input fails, try again
			}
		
		}

		void shipsink(board *b, int id, board *bb){//ship sinking procedure. b - victim board , id - ship to sink, bb - attacker shooter board
			for (int i=0; i<(b->ships[id].hits); i++){
				b->fields[b->ships[id].xy[i][1]][b->ships[id].xy[i][0]] = '\\';
				bb->fields[b->ships[id].xy[i][1]][b->ships[id].xy[i][0]] = '\\';
				for (int x=b->ships[id].xy[i][0]-1; x<=b->ships[id].xy[i][0]+1; x++){
					for (int y=b->ships[id].xy[i][1]-1; y<=b->ships[id].xy[i][1]+1; y++){
						if (x>-1 && x<10 && y>-1 && y<10 && b->fields[y][x] == '='){
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
						}
					}
				}
			}
			b->iship--;
		}

		void shiphit(board *b, board *bb){//b - victim board , bb - attacker shooter board
			//runs every time any ship is hit - checks every ship for remaining hits
			for (int i=0; i<10; i++){
				int by = b->ships[i].xy[0][1];
				int bx = b->ships[i].xy[0][0];
				if (b->fields[by][bx] == '\\') continue; //sunk ship, ignore it; must be to count remaining ships properly
				int temphits = b->ships[i].hits;
				for (int j=0; j<(b->ships[i].hits); j++){ //check ship hit parts
					if (b->fields[b->ships[i].xy[j][1]][b->ships[i].xy[j][0]] == '!') temphits--;
				}
				if (!temphits){ //every part of ship destroyed, ship must sink
					shipsink(b, i, bb);
				}
			}
		}

		bool nextfrepeat(int x, int y){//checks x,y position for comp tactic2 next possible position to cross-attack; prevents writing two same positions to attack
			for (int i=0; i<ai.nextf; i++){
				if (ai.nextfield[i][0] == x && ai.nextfield[i][1] == y) return false;
			}
			return true;
		}

		void compmove(board *b, board *bb){//b - comp shooting board, bb - player board
			//tactics: 0-full random, 1-random+attacking found ships, 2-cross attack in random+attacking found ships
			//"computer AI", choses position to attack, prepares next ones; code is repeated to avoid mistakes from mixing everything
			while (1){
				if (!ai.tactic){//tactic 0, attack empty square
					int x = rand() % 10;
					int y = rand() % 10;
					if (b->fields[y][x] != '=') continue;//not empty square
					else if (bb->fields[y][x] == '='){//miss
						b->fields[y][x] = '.';
						bb->fields[y][x] = '.';
					}
					else{//hit
						b->fields[y][x] = '!';
						bb->fields[y][x] = '!';
						shiphit(bb,b);
					}
					break;
				}//end tactic0
				else if (ai.tactic == 1){//tactic 1
					if (ai.numcandidates == 0){//no following moves, choose random target until hit
						int x = rand() % 10;
						int y = rand() % 10;
						if (b->fields[y][x] != '=') continue;
						else if (bb->fields[y][x] == '='){
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
						}
						else{
							b->fields[y][x] = '!';
							bb->fields[y][x] = '!';
							shiphit(bb,b);
							if (b->fields[y][x] == '\\'); //if it was 1-hit ship, next close positions are pointless
							else{//add plus-shape squares ass possible position for remaining ship parts; ignore if out of map
								if (x>0){
									ai.shootcandidates[ai.numcandidates][0] = x-1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (x<9){
									ai.shootcandidates[ai.numcandidates][0] = x+1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (y>0){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y-1;
									ai.numcandidates++;
								}
								if (y<9){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y+1;
									ai.numcandidates++;
								}
							}
						}
						break;
					}//end random for tactic1
					else{//choose one of candidates
						int r = rand() % ai.numcandidates;
						int x = ai.shootcandidates[r][0];
						int y = ai.shootcandidates[r][1];
						ai.shootcandidates[r][0] = ai.shootcandidates[ai.numcandidates-1][0];//swap chosen candidate with last one
						ai.shootcandidates[r][1] = ai.shootcandidates[ai.numcandidates-1][1];
						if (ai.numcandidates) ai.numcandidates--;
						if (b->fields[y][x] != '=') continue;
						else if (bb->fields[y][x] == '='){
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
						}
						else{
							b->fields[y][x] = '!';
							bb->fields[y][x] = '!';
							shiphit(bb,b);
							if (b->fields[y][x] == '\\'){//ship sunk, the rest of positions are empty
								ai.numcandidates = 0;
							}
							else{//new available positions
								if (x>0){
									ai.shootcandidates[ai.numcandidates][0] = x-1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (x<9){
									ai.shootcandidates[ai.numcandidates][0] = x+1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (y>0){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y-1;
									ai.numcandidates++;
								}
								if (y<9){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y+1;
									ai.numcandidates++;
								}
							}
						}
						break;
					}//end 'accurate' attack for tactic1
				}//end tactic1
				else if (ai.tactic == 2){//tactic 2, attacks in cross until hit
					if (ai.numcandidates == 0 && ai.nextf == 0){//no positions, random target
						int x = rand() % 10;
						int y = rand() % 10;
						if (b->fields[y][x] != '=') continue;
						else if (bb->fields[y][x] == '='){//miss, but choose position in cross to hit larger ships easier
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
							if (x>0 && y>0 && b->fields[y-1][x-1] == '='){
								ai.nextfield[ai.nextf][0] = x-1;
								ai.nextfield[ai.nextf][1] = y-1;
								ai.nextf++;
							}
							if (x<9 && y>0 && b->fields[y-1][x+1] == '='){
								ai.nextfield[ai.nextf][0] = x+1;
								ai.nextfield[ai.nextf][1] = y-1;
								ai.nextf++;
							}
							if (x>0 && y<9 && b->fields[y+1][x-1] == '='){
								ai.nextfield[ai.nextf][0] = x-1;
								ai.nextfield[ai.nextf][1] = y+1;
								ai.nextf++;
							}
							if (x<9 && y<9 && b->fields[y+1][x+1] == '='){
								ai.nextfield[ai.nextf][0] = x+1;
								ai.nextfield[ai.nextf][1] = y+1;
								ai.nextf++;
							}
						}
						else{
							b->fields[y][x] = '!';
							bb->fields[y][x] = '!';
							shiphit(bb,b);
							if (b->fields[y][x] == '\\');
							else{
								if (x>0){
									ai.shootcandidates[ai.numcandidates][0] = x-1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (x<9){
									ai.shootcandidates[ai.numcandidates][0] = x+1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (y>0){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y-1;
									ai.numcandidates++;
								}
								if (y<9){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y+1;
									ai.numcandidates++;
								}
							}
						}
						break;
					}//end random for tactic2
					else if (ai.nextf>0){
						int r = rand() % ai.nextf;
						int x = ai.nextfield[r][0];
						int y = ai.nextfield[r][1];
						ai.nextfield[r][0] = ai.nextfield[ai.nextf-1][0];
						ai.nextfield[r][1] = ai.nextfield[ai.nextf-1][1];
						if (ai.nextf) ai.nextf--;
						if (b->fields[y][x] != '=') continue;
						else if (bb->fields[y][x] == '='){
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
							int itmp = rand() % 4;
							for (int i=0; i<4; i++){
								if (itmp+i==0 && x>0 && y>0 && b->fields[y-1][x-1] == '=' && nextfrepeat(x,y)){
									ai.nextfield[ai.nextf][0] = x-1;
									ai.nextfield[ai.nextf][1] = y-1;
									ai.nextf++;
									continue;
								}
								else if (itmp+i==1 && x<9 && y>0 && b->fields[y-1][x+1] == '=' && nextfrepeat(x,y) && ai.nextf<4){
									ai.nextfield[ai.nextf][0] = x+1;
									ai.nextfield[ai.nextf][1] = y-1;
									ai.nextf++;
									continue;
								}
								else if (itmp+i==2 && x>0 && y<9 && b->fields[y+1][x-1] == '=' && nextfrepeat(x,y) && ai.nextf<4){
									ai.nextfield[ai.nextf][0] = x-1;
									ai.nextfield[ai.nextf][1] = y+1;
									ai.nextf++;
									continue;
								}
								else if (itmp+i==3 && x<9 && y<9 && b->fields[y+1][x+1] == '=' && nextfrepeat(x,y) && ai.nextf<4){
									ai.nextfield[ai.nextf][0] = x+1;
									ai.nextfield[ai.nextf][1] = y+1;
									ai.nextf++;
								}
							}
						}
						else{
							b->fields[y][x] = '!';
							bb->fields[y][x] = '!';
							shiphit(bb,b);
							ai.nextf = 0;
							if (b->fields[y][x] == '\\') ai.numcandidates = 0;
							else{
								if (x>0){
									ai.shootcandidates[ai.numcandidates][0] = x-1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (x<9){
									ai.shootcandidates[ai.numcandidates][0] = x+1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (y>0){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y-1;
									ai.numcandidates++;
								}
								if (y<9){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y+1;
									ai.numcandidates++;
								}
							}
						}
						break;
					}//end cross-attack
					else{
						int r = rand() % ai.numcandidates;
						int x = ai.shootcandidates[r][0];
						int y = ai.shootcandidates[r][1];
						ai.shootcandidates[r][0] = ai.shootcandidates[ai.numcandidates-1][0];
						ai.shootcandidates[r][1] = ai.shootcandidates[ai.numcandidates-1][1];
						if (ai.numcandidates) ai.numcandidates--;
						if (b->fields[y][x] != '=') continue;
						else if (bb->fields[y][x] == '='){
							b->fields[y][x] = '.';
							bb->fields[y][x] = '.';
						}
						else{
							b->fields[y][x] = '!';
							bb->fields[y][x] = '!';
							shiphit(bb,b);
							if (b->fields[y][x] == '\\'){
								ai.numcandidates = 0;
							}
							else{
								if (x>0){
									ai.shootcandidates[ai.numcandidates][0] = x-1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (x<9){
									ai.shootcandidates[ai.numcandidates][0] = x+1;
									ai.shootcandidates[ai.numcandidates][1] = y;
									ai.numcandidates++;
								}
								if (y>0){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y-1;
									ai.numcandidates++;
								}
								if (y<9){
									ai.shootcandidates[ai.numcandidates][0] = x;
									ai.shootcandidates[ai.numcandidates][1] = y+1;
									ai.numcandidates++;
								}
							}
						}
						break;
					}	
				}
				else break; //no comp
			}
		}

		void clearboard(board *b){//clears selected board by changing all positions to = (empty/unknown) 
			fill(b->fields[0], b->fields[0]+100, '=');
		}
									
		void selectopt(board *b){//pre-game options. b - player board, i - build board parameter
			char c;
			screene(0);
			while(1){
				c = getch();
				if (c == '0'){
					ai.tactic = 0;
					break;
				}
				else if (c == '1'){
					ai.tactic = 1;
					break;
				}
				else if (c == '2'){
					ai.tactic = 2;
					break;
				}
			}
			screene(1);
			while(1){
				c = getchar();
				switch (c){
					case 'm':
					case 'M':
						my_placement(b);
						return;
					case 'r':
					case 'R':
						com_placement(b);
						return;
				}
			}
		}
									
		char endgame(int e, board *myb, board *sb, board *cpb, board *csb){//handles endgame sequence - new game or leaving program
			char c;
			screend(myb, cpb, e);
			cursor.x = 0;
			cursor.y = 0;
			while(1){
				c = getchar();
				switch(c){
					case 'y':
					case 'Y':
						clearboard(myb); clearboard(sb); clearboard(cpb); clearboard(csb);
						com_placement(cpb);
						selectopt(myb);
						return 'x';
						break;
					case 'n':
					case 'N':
						return 'q';
						break;
				}
			}
			return 'q';
		}
									
		int run(){//'main' function, handles game and inits variables
			unsigned seed=time(0);
			srand(seed);
			initscr();
			cbreak();
			keypad(stdscr, TRUE);
			noecho();
			nodelay(stdscr, TRUE);
			int ch = 0;
			cursor.x = 0;
			cursor.y = 0;
			board pb, ps, cb, cs; // player board (player ships are here), player shooting board (player uses it to shoot comp ships), computer board (comp ships), computer shooting board
			board *myb = &pb; clearboard(myb); //pointer to player board
			board *sb = &ps; clearboard(sb);
			board *cpb = &cb; clearboard(cpb); //computer placement board
			board *csb = &cs; clearboard(csb);
			com_placement(cpb);
			selectopt(myb);
			screend(myb, sb);
			while(ch!='q' && ch!='Q'){
				ch = 0;
				ch = getch();
				switch(ch){
					case 'w':
					case 'W':
					case KEY_UP:
						if (cursor.y>0) cursor.y--;
						screend(myb, sb);
						break;
					case 'a':
					case 'A':
					case KEY_LEFT:
						if (cursor.x>0) cursor.x--;
						screend(myb, sb);
						break;
					case 'd':
					case 'D':
					case KEY_RIGHT:
						if (cursor.x<9) cursor.x++;
						screend(myb, sb);
						break;
					case 's':
					case 'S':
					case KEY_DOWN:
						if (cursor.y<9) cursor.y++;
						screend(myb, sb);
						break;
					case 'x':
					case 'X':
					case KEY_ENTER:
					case '\n':
						switch(cpb->fields[cursor.y][cursor.x]){
							case '1':
							case '2':
							case '3':
							case '4':
								message = "Hit!";
								cpb->fields[cursor.y][cursor.x] = '!';
								sb->fields[cursor.y][cursor.x] = '!';
								shiphit(cpb, sb);
								if (sb->fields[cursor.y][cursor.x] == '\\') message += " Ship sunk!";
								if (cpb->iship < 1){
									ch = endgame(1, myb, sb, cpb, csb);
									break;
								}
								compmove(csb, myb);
								if (myb->iship < 1){
									ch = endgame(2, myb, sb, cpb, csb);
								}
								break;
							case '=':
								message = "Miss";
								cpb->fields[cursor.y][cursor.x] = '.';
								sb->fields[cursor.y][cursor.x] = '.';
								compmove(csb, myb);
								if (myb->iship < 1){
									ch = endgame(2, myb, sb, cpb, csb);
								}
								break;
						}
						screend(myb, sb);
						break;
					case 'r':
					case 'R':
						ch = endgame(3, myb, sb, cpb, csb);
						screend(myb, sb);
						break;
				}
			}
			endwin();
			return 0;
		}
};


int main(){
	Battleships *game = new Battleships;
	return 0;
}
