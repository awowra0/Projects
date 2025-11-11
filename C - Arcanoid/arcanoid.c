#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

//compiled with command "gcc final.c -w -lSDL2 -lm -o final"
//textures are stored in "gfx" directory

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 320;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* render = NULL;
SDL_Texture* stexture = NULL;
SDL_Event e;

int mode=0;
int ilevel=3;
int level=0;
int chances;
int score;

SDL_Texture* bricktext[12];
int ibricks[3]={4,30,16};
int idbricks;
int ctbrick[30];
int cdbrick[30];
int bricksx[3][30]={{0,9,4,5},{0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9},{0,1,2,3,4,5,6,6,6, 0,1,2,3,4,5,5}};
int bricksy[3][30]={{0,0,3,3},{0,0,0,0,0,0,0,0,0,0, 1,1,2,2,3,3,2,2,1,1, 2,3,3,4,5,5,4,3,3,2},{5,5,5,5,5,5,5,4,3, 4,4,4,4,4,4,3}};
int bricktype[3][30]={{4,4,5,6},{8,5,1,6,4,4,6,1,5,8, 2,6,7,8,0,0,7,8,6,2, 2,0,1,2,2,2,2,1,0,2},{0,0,0,0,0,0,0,0,0, 1,2,2,1,2,2,1}};
SDL_Texture* paddletext[4];
int paddlelength[4]={60,90,120,150};
int xmouse,ymouse;
int ballblocked=1;
SDL_Texture* bonustext[9];
SDL_Texture* balltext[3];
SDL_Texture* backtext[3];

typedef struct Ball {
	double x;
	double y;
	int type;
	double angle;
	double speed;
	SDL_Texture *texture;
} Ball;

typedef struct Paddle {
	double x;
	double y;
	int type;
	int magnet;
	SDL_Texture *texture;
} Paddle;

typedef struct Bonus {
	double x;
	double y;
	int type;
	int show;
	SDL_Texture *texture;
} Bonus;

Ball ball;
Paddle paddle;
Bonus bonus;

int initgame(){
	if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    else{
    	window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf( "Failed to create window: %s\n", SDL_GetError());
            return 1;
        }
        else{
        	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        	if(render == NULL){
            	printf( "Failed to create renderer: %s\n", SDL_GetError());
            	return 1;
        	}
        }
    }
    surface = SDL_LoadBMP("gfx/hard.bmp");
    stexture = SDL_CreateTextureFromSurface(render, surface);
    bricktext[0]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/flameable.bmp");
    bricktext[1]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/gray3.bmp");
    bricktext[2]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/gray2.bmp");
    bricktext[3]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/gray1.bmp");
    bricktext[4]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/blue.bmp");
    bricktext[5]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/greencyan.bmp");
    bricktext[6]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/orangeyellow.bmp");
    bricktext[7]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bluepurple.bmp");
    bricktext[8]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/flameb1.bmp");
    bricktext[9]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/flameb2.bmp");
    bricktext[10]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/flameb3.bmp");
    bricktext[11]=SDL_CreateTextureFromSurface(render, surface);
    
    surface = SDL_LoadBMP("gfx/paddle60.bmp");
    paddletext[0]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/paddle90.bmp");
    paddletext[1]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/paddle120.bmp");
    paddletext[2]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/paddle150.bmp");
    paddletext[3]=SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/ball11.bmp");
    balltext[0] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/cball11.bmp");
    balltext[1] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/fball11.bmp");
    balltext[2] = SDL_CreateTextureFromSurface(render, surface);
    
    surface = SDL_LoadBMP("gfx/bonus1.bmp");
    bonustext[0] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus2.bmp");
    bonustext[1] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus3.bmp");
    bonustext[2] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus4.bmp");
    bonustext[3] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus5.bmp");
    bonustext[4] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus6.bmp");
    bonustext[5] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus7.bmp");
    bonustext[6] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus8.bmp");
    bonustext[7] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/bonus9.bmp");
    bonustext[8] = SDL_CreateTextureFromSurface(render, surface);
    
    surface = SDL_LoadBMP("gfx/back0.bmp");
    backtext[0] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/back1.bmp");
    backtext[1] = SDL_CreateTextureFromSurface(render, surface);
    surface = SDL_LoadBMP("gfx/back2.bmp");
    backtext[2] = SDL_CreateTextureFromSurface(render, surface);
    
    for(int i=0;i<ilevel;i++){
    	for (int j=0;j<ibricks[i];j++){
    		bricksx[i][j] *= 60;
    		bricksy[i][j] *= 20;
    	}
    }
    return 0;
}

int closewindow(){
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(stexture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
    SDL_Quit();
}

void rvideo(){
	SDL_Rect rectbrick;
	SDL_RenderClear(render);
	for(int i=0; i<ibricks[level]; i++){
    	if (cdbrick[i]==1) continue;
    	rectbrick.w=60;
		rectbrick.h=20;
    	rectbrick.x=bricksx[level][i];
    	rectbrick.y=bricksy[level][i];
    	if (cdbrick[i]==0){
    		SDL_RenderCopy(render, bricktext[ctbrick[i]], NULL, &rectbrick);
    	}
    	else if (cdbrick[i]>33){
    		SDL_RenderCopy(render, bricktext[9], NULL, &rectbrick);
    	}
  		else if (cdbrick[i]>17){
  			SDL_RenderCopy(render, bricktext[10], NULL, &rectbrick);
  		}
  		else if (cdbrick[i]>1){
  			SDL_RenderCopy(render, bricktext[11], NULL, &rectbrick);
  		}
    }
    rectbrick.x=5;
    rectbrick.y=5;
    rectbrick.w=30;
    rectbrick.h=5;
    for (int i=0;i<chances && i<7;i++){
    	SDL_RenderCopy(render, paddletext[0], NULL, &rectbrick);
    	rectbrick.x+=35;
    }
    if (bonus.show==1){
    	rectbrick.x=bonus.x;
    	rectbrick.y=bonus.y;
    	rectbrick.w=25;
    	rectbrick.h=25;
    	SDL_RenderCopy(render, bonus.texture, NULL, &rectbrick);
    }
    rectbrick.w=10;
    rectbrick.h=10;
    rectbrick.x=ball.x-5;
    rectbrick.y=ball.y-5;
    if (ball.type==0) SDL_RenderCopy(render, balltext[0], NULL, &rectbrick);
    else if (ball.type==1) SDL_RenderCopy(render, balltext[1], NULL, &rectbrick);
    else SDL_RenderCopy(render, balltext[2], NULL, &rectbrick);
    rectbrick.w=paddlelength[paddle.type];
    rectbrick.h=10;
    rectbrick.x=paddle.x-paddlelength[paddle.type]/2;
    rectbrick.y=paddle.y;
    SDL_RenderCopy(render, paddletext[paddle.type], NULL, &rectbrick);
    SDL_RenderPresent(render);
}

void bounce(int ibounce){
	if (ibounce==0){ //paddle
		ball.angle = 345-150*(paddle.x + paddlelength[paddle.type]/2 - ball.x) / paddlelength[paddle.type];
		if (paddle.magnet==1){
			ballblocked=1;
			}
	}
	else if (ibounce==1){ //upper/lower obstacle 
		ball.angle = 360 - ball.angle;
	}
	else if (ibounce==2){ //left/right obstacle
		ball.angle = 180 - ball.angle;
		if (ball.angle < 0) ball.angle += 360;
	}
	if (ball.speed<1) ball.speed+=0.00390625;
}

void movebonus(){
	bonus.y+=0.5;
	if (bonus.y>=paddle.y-25 && bonus.y<=paddle.y+25){
		if (bonus.x>=paddle.x-paddlelength[paddle.type]/2-25 && bonus.x<=paddle.x+paddlelength[paddle.type]/2) usebonus();
	}
	else if (bonus.y>=SCREEN_HEIGHT) bonus.show=0;
}

void moveball(){
	ball.x += ball.speed * cos(ball.angle/180*M_PI);
	ball.y += ball.speed * sin(ball.angle/180*M_PI);
}
void collision(){
	double xh,yh;
	if ((ball.x<=5 || ball.x >=SCREEN_WIDTH-5)) bounce(2);
	else if (ball.y<=5) bounce(1);
	else if (ball.y >=SCREEN_HEIGHT){
		SDL_Delay(200);
		ballblocked=1;
		chances--;
		ball.angle=300;
		ball.speed=0.59765625;
		ball.type=0;
		bonus.show=0;
	}
	else if (ball.y>=paddle.y-5){
		if (ball.x>=paddle.x-paddlelength[paddle.type]/2-5 && ball.x<=paddle.x+paddlelength[paddle.type]/2+5) bounce(0);
	}
	else{
		for (int i=0; i<ibricks[level]; i++){
			if (cdbrick[i]>0) continue;
			else {
				xh=abs(ball.x-bricksx[level][i]-30)-30;
				yh=abs(ball.y-bricksy[level][i]-10)-10;
				if (xh<=10 && yh<=10){
					if (ball.type==1 || ball.type==3){
						bounce(4);
						destroybrick(i);
					}
					else if (xh<yh){
						bounce(1);
						destroybrick(i);
					}
					else{
						bounce(2);
						destroybrick(i);
					}
					break;
				}
			}
		}
	}
}

void destroybrick(int ibrickd){
	if (ctbrick[ibrickd]==0 && ball.type==0){}
	else if (ctbrick[ibrickd]==0 && ball.type==1){//hard brick
		cdbrick[ibrickd]=1;
		givepoints(ibrickd,0);
	}
	else if (ball.type>1){
		givepoints(ibrickd,1);
		if (ctbrick[ibrickd]==0) idbricks++;
		ctbrick[ibrickd]=1;
		cdbrick[ibrickd]=2;
	}
	else if (ctbrick[ibrickd]==1){//flameable brick
		cdbrick[ibrickd]=50;
	}
	else if ((ctbrick[ibrickd]==2 || ctbrick[ibrickd]==3) && ball.type==1){
		cdbrick[ibrickd]=1;
		idbricks--;
		givepoints(ibrickd,0);
	}
	else if (ctbrick[ibrickd]==2 || ctbrick[ibrickd]==3){
		ctbrick[ibrickd]++;
		givepoints(ibrickd,2);
	}
	else {
		cdbrick[ibrickd]=1;
		idbricks--;
		givepoints(ibrickd,0);
	}
}

void givepoints(int ibrickd, int type){
	if (type==0){ // destroyed brick
		switch(ctbrick[ibrickd]){
			case 0:
				score+=65;
				break;
			case 1:
				score+=15;
				break;
			case 2:
				score+=40;
				break;
			case 3:
				score+=30;
				break;
			default:
				score+=20;
				break;
			}
		genbonus();
		}
	else if (type==1){ //burned brick
		switch(ctbrick[ibrickd]){
			case 0:
				score+=39;
				break;
			case 1:
				score+=9;
				break;
			case 2:
				score+=24;
				break;
			case 3:
				score+=18;
				break;
			default:
				score+=12;
				break;
		}
		genbonus();
	}
	else if (type==2){ //weaken graybrick
		score+=10;
	}
	else if (type==3){ //bonus
		score+=50;
	}
}

void usebonus(){
	switch(bonus.type){
		case 0:
			if (paddle.type<3){
				paddle.type++;
			}
			break;
		case 1:
			if (paddle.type>0){
				paddle.type--;
			}
			break;
		case 2:
			if (ball.type==0) ball.type=2;
			else if (ball.type==1) ball.type=3;
			break;
		case 3:
			if (ball.type==0) ball.type=1;
			else if (ball.type==2) ball.type=3;
			break;
		case 4:
			ball.speed=1;
			break;
		case 5:
			ball.speed=0.5;
			break;
		case 6:
			chances++;
			break;
		case 7:
			paddle.magnet=1;
			break;
		case 8:
			ball.type=0;
			paddle.magnet=0;
	}
	givepoints(0,3);
	bonus.show=0;
}

void genbonus(int i){
	if (bonus.show==0){
		int num=rand()%100;
		if (num<25){
			bonus.x=bricksx[level][i]+30;
			bonus.y=bricksy[level][i]+10;
			num=rand()%9;
			bonus.type=num;
			bonus.texture=bonustext[num];
			bonus.show=1;
		}
	}
}

void flameable(){
	for (int i=0; i<ibricks[level]; i++){
		if (cdbrick[i]==2){
			if (ctbrick[i]==1){
				for(int j=0; j<ibricks[level]; j++){
					if (cdbrick[j]!=0) continue;
					else if (abs(bricksx[level][j]-bricksx[level][i])<=65 && abs(bricksy[level][j]-bricksy[level][i])<=25){
						cdbrick[j]=50;
					}
				}
			}
			cdbrick[i]=1;
			if (ctbrick[i]!=0) idbricks--;
			givepoints(i,1);
		}
		else if(cdbrick[i]>2) cdbrick[i]--;
	}
}

void switchmenu(){
	SDL_RenderClear(render);
	SDL_SetRenderDrawColor(render, 0, 255, 255, 255);
	SDL_RenderClear(render);
	SDL_Rect textrect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	SDL_RenderCopy(render, backtext[0], NULL, &textrect);
	SDL_RenderPresent(render);
    mode=0;
    level=0;
    score=0;
}

void switchlevel(){
	ball.angle=300;
	ball.speed=0.59765625;
	ball.type=0;
	paddle.type=1;
	paddle.magnet=0;
	bonus.show=0;
	idbricks=0;
	for (int i=0;i<ibricks[level];i++){
		cdbrick[i]=0;
		ctbrick[i]=bricktype[level][i];
		if (ctbrick[i]!=0) idbricks++;
	}
	ballblocked=1;
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	SDL_RenderClear(render);
	SDL_RenderPresent(render);
    mode=1;
    SDL_Delay(500);
}

void endgame(int win){
	SDL_RenderClear(render);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);
	SDL_Rect textrect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	struct tm * at;
	time_t cutime=time(0);
	at=localtime(&cutime);
	FILE* file;
	file=fopen("Arcanoid-scores.dat","a");
	if (file){
		if (win==1){
			fprintf(file,"Game won with score %d on %d.%d.%d %d:%d.\n",score,at->tm_mday,at->tm_mon,at->tm_year+1900,at->tm_hour,at->tm_min);
		}
		else{
			fprintf(file,"Game lost on level %d with score %d on %d.%d.%d %d:%d.\n",level+1,score,at->tm_mday,at->tm_mon+1,at->tm_year+1900,at->tm_hour,at->tm_min);
		}
		printf("Score saved.\n");
	}
	else{
		printf("Failed to save score.\n");
	}
	fclose(file);
	if (win==1) SDL_RenderCopy(render, backtext[2], NULL, &textrect);
	else SDL_RenderCopy(render, backtext[1], NULL, &textrect);
	SDL_RenderPresent(render);
}

int main(int argc, char* args[])
{
    if(initgame()) return 1;
    
    SDL_Delay(200);
    int bexit=0;
    unsigned seed=time(0);
    srand(seed);
    ball.angle=300;
    ball.speed=0.59765625;
    paddle.x=300;
    paddle.y=300;
    paddle.type=1;
    bonus.show=0;
    
    switchmenu();
    while(!bexit){
    	while(SDL_PollEvent(&e) != 0){
    		if(e.type == SDL_QUIT){
            	bexit=1;
            }
            else if(e.type == SDL_KEYDOWN){
            	if (e.key.keysym.sym==SDLK_ESCAPE){
            		if (mode==0) bexit=1;
            		else if (mode==1){
            			SDL_Delay(200);
            			mode=0;
            			level=0;
            			switchmenu();
            		}
            		else if (mode==3){
            			switchmenu();
            		}
            	}
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN){
            	if (e.button.button == SDL_BUTTON_LEFT){
            		if (mode==0){
            			chances=3;
            			switchlevel();
            		}
            		else if (mode==1){
            			if (ballblocked==1) ballblocked=0;
            		}
            		else if (mode==3){
            			switchmenu();
            		}
            	} 
            }
            else if (e.type == SDL_MOUSEMOTION && mode==1){
            	SDL_GetMouseState(&xmouse, &ymouse);
            	if (xmouse-paddlelength[paddle.type]/2<0) paddle.x=paddlelength[paddle.type]/2;
            	else if (xmouse>SCREEN_WIDTH-paddlelength[paddle.type]/2) paddle.x=SCREEN_WIDTH-paddlelength[paddle.type]/2;
            	else paddle.x = xmouse;
            }
    	}
    	if (mode==1){
    		flameable();
    		if (bonus.show==1) movebonus();
        	SDL_Delay(2);
    		if (!ballblocked){
    			collision();
    			moveball();
    		}
    		else{
    			ball.x=(-345.0+ball.angle)/150.0*paddlelength[paddle.type]+paddle.x+paddlelength[paddle.type]/2;
    			ball.y=paddle.y-7;
    		}
    		rvideo();
    		if (idbricks==0){
    			if (level<ilevel-1){
    				mode=2;
    				SDL_Delay(200);
    				level++;
    				switchlevel();
    			}
    			else if (level==ilevel-1){
    				printf("You have won, your score: %d.\n", score);
    				SDL_Delay(400);
    				mode=3;
    				endgame(1);
    			}
    		}
    		if (chances==0){
    			SDL_Delay(200);
    			printf("Lost all chances, game over.\nYour score: %d.\n",score);
    			mode=3;
    			endgame(0);
    		}
    	}
    }
    closewindow();
    return 0;
}

