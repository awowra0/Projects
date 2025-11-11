#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,4,5,6,7);
int state = 0;
int mine; //liczba min
int mode = 0; //tryb dla stanu, etap konfiguracji
int x;
int y;
int k;
char mapa [16][16];
char maph [16][16];
int sizex=10;
int sizey=10;
int points; //pola do odkrycia
int fmines=0; //znalezione miny
int volt;
String sdata; //port szeregowy
bool ext;
unsigned long stime;
 
void setup(){
  lcd.begin(16,2);
  Serial.begin(9600);
}


void loop(){
  if (Serial.available()>0){ // wprowadzono napis
    sdata=Serial.readStringUntil('#'); // odczytuj do znaku komendy #
    if (sdata=="MAPA" and state>1){
      for (int c=sizey-1; c>=0; c--){
        for (int d=0; d<sizex; d++){
          if (mapa[d][c]==0) Serial.print("Q");
          else Serial.print(mapa[d][c]);
        }
        Serial.println();
      }
    }
    else if (sdata=="MAPH" and state>1){
      for (int c=sizey-1; c>=0; c--){
        for (int d=0; d<sizex; d++){
          Serial.print(maph[d][c]);
        }
        Serial.println();
      }
    }
    else if (sdata.startsWith("CONF") and state<2){ // CONF08120350204# => sizex=8, sizey=12, mine=35, x=2, y=4
      int csx = sdata.substring(4,6).toInt();
      int csy = sdata.substring(6,8).toInt();
      int cm = sdata.substring(8,11).toInt();
      int cx = sdata.substring(11,13).toInt();
      int cy = sdata.substring(13,15).toInt();
      if (csx>=5 and csx<=16){
        if (csy>=5 and csy<=16){
          if (cm>=5 and cm<=csx*csy-5){
            if (cx>0 and cx<= csx){
              if (cy>0 and cy<= csy){
                sizex = csx;
                sizey = csy;
                mine = cm;
                x = cx-1;
                y = cy-1;
                points=sizex*sizey-mine;
                generic();
                check(x,y);
                state=2;
                mode=0;
                stime = millis();
                printer();
              }
            }
          }
        }
      }
    }
    else if (sdata=="MINY" and state>1){
      Serial.println("Wszystkie miny: "+String(mine));
      Serial.println("Oznaczone miny: "+String(fmines));
      Serial.println("Ukryte miny: "+String(mine-fmines));
    }
    else if (sdata=="POLA" and state>1){
      Serial.println("Wszytkie pola: "+String(sizex*sizey));
      Serial.println("Nieodkryte pola: "+String(points));
    }
    else if (sdata.startsWith("XY")){ //przykład: XY1002# => x=10, y=2
      if (sdata.substring(2,4).toInt()<=sizex and sdata.substring(2,4).toInt()>0 and sdata.substring(4,6).toInt()<=sizey and sdata.substring(4,6).toInt()>0){
          x=sdata.substring(2,4).toInt()-1;
          y=sdata.substring(4,6).toInt()-1;
          if (state==3) state=2;
          printer();
      }
    }
    else if (sdata=="ODKRYJ" and (state==2 or state==3)){
      state=2;
      if (mapa[x][y]!='P'){
        if (maph[x][y]=='M') Serial.println("Mina!");
        else Serial.println(maph[x][y]);
        check(x,y);
      }
      printer();
    }
    else if (sdata=="OZNACZ" and (state==2 or state==3)){
      state=2;
      if (mapa[x][y]==0){
        mapa[x][y]='P';
        Serial.println("Oznaczono: "+String(x+1)+", "+String(y+1));
        fmines++;
        printer();
      }
      else if (mapa[x][y]=='P') {
        mapa[x][y]=0;
        Serial.println("Odznaczono: "+String(x+1)+", "+String(y+1));
        fmines--;
        printer();
      }
    }
    else if (sdata=="AUTO" and (state==2 or state==3)){
      state=2;
      if (fmines==mine) finish();
      else Serial.println("Nie oznaczono wszystkich min.");
      printer();
    }
    else if (sdata.startsWith("ODXY") and (state==2 or state==3)){ // ODXY0204# => x=2, y=4, check(x,y)
      if (sdata.substring(4,6).toInt()>0 and sdata.substring(4,6).toInt()<=sizex and sdata.substring(6,8).toInt()>0 and sdata.substring(6,8).toInt()<=sizey){
        x = sdata.substring(4,6).toInt()-1;
        y = sdata.substring(6,8).toInt()-1;
        state = 2;
        if (mapa[x][y]!='P'){
          if (maph[x][y]=='M') Serial.println("Mina!");
          else Serial.println(maph[x][y]);
          check(x,y);
        }
        printer();
      }
    }
    else if (sdata.startsWith("OZXY") and (state==2 or state==3)){
      if (sdata.substring(4,6).toInt()>0 and sdata.substring(4,6).toInt()<=sizex and sdata.substring(6,8).toInt()>0 and sdata.substring(6,8).toInt()<=sizey){
        x = sdata.substring(4,6).toInt()-1;
        y = sdata.substring(6,8).toInt()-1;
        state = 2;
        if (mapa[x][y]==0){
          mapa[x][y]='P';
          Serial.println("Oznaczono: "+String(x+1)+", "+String(y+1));
          fmines++;
          printer();
        }
        else if (mapa[x][y]=='P') {
          mapa[x][y]=0;
          Serial.println("Odznaczono: "+String(x+1)+", "+String(y+1));
          fmines--;
          printer();
        }
      }
    }
    else if (sdata=="NOWA" and state>0){
      Serial.println("Nowa gra");
      reset();
    }
    else if (sdata=="RESTART" and state>1){
      Serial.println("Restart");
      restart();
    }
  }
  
  volt = analogRead(A0);
  if (state==0){ //start
    lcd.setCursor(0,0);
    if (mode==0){
      RGB_mode(192,255,192);
      lcd.clear();
      lcd.print("  Saper  ");
      mode=1;
    }
    else {
      RGB_mode(255,255,192);
      lcd.clear();
      lcd.print("* Saper *");
      mode=0;
    }
      volt = analogRead(A0);
    if (volt > 700 and volt < 800) {
      state=1;
      mode=0;
      RGB_mode(64,255,64);
    }
  }
  else if (state==1){ //generacja
    if (mode == 0){
      printer();
      delay(1000);
      mode = 1;
      x = 10;
      y = 10;
      printer();
    }
    else if (mode == 1){
      if (volt > 100 and volt < 200 and y<16){
        y++;
        printer();
      }
      else if (volt > 300 and volt < 400 and y>5){
        y--;
        printer();
      }
      else if (volt > 500 and volt < 600 and x>5){
        x--;
        printer();
      }
      else if (volt < 100 and x<16){
        x++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        sizex = x;
        sizey = y;
        mode = 2;
        mine = 10;
        printer();
      }
    }
    else if (mode == 2){
      if (volt > 100 and volt < 200 and mine<sizex*sizey-14){
        mine=mine+10;
        printer();
      }
      else if (volt > 300 and volt < 400 and mine>14){
        mine=mine-10;
        printer();
      }
      else if (volt > 500 and volt < 600 and mine>5){
        mine--;
        printer();
      }
      else if (volt < 100 and mine<sizex*sizey-5){
        mine++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        x=0;
        y=0;
        points=sizex*sizey-mine; //pola do odkrycia;
        mode=3;
        printer();
      }
      
    }
    else if (mode==3){
      if (volt > 100 and volt < 200 and y<sizey-1){
        y++;
        printer();
      }
      else if (volt > 300 and volt < 400 and y>0){
        y--;
        printer();
      }
      else if (volt > 500 and volt < 600 and x>0){
        x--;
        printer();
      }
      else if (volt < 100 and x<sizex-1){
        x++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        generic();
        state=2;
        mode=0;
        volt=1023;
        stime = millis();
        check(x, y);
        printer();
      }
    }
  }
  else if (state==2){
    if (volt > 100 and volt < 200 and y<sizey-1){
      y++;
      printer();
    }
    else if (volt > 300 and volt < 400 and y>0){
      y--;
      printer();
    }
    else if (volt > 500 and volt < 600 and x>0){
      x--;
      printer();
    }
    else if (volt < 100 and x<sizex-1){
      x++;
      printer();
    }
    else if (volt > 700 and volt < 800){
      state=3;
      k=7;
      delay(200);
      printer();
    }
  }
  else if (state==3){
    if (volt > 500 and volt < 600 or (volt > 300 and volt < 400)){
      if (k==0 and fmines!=mine) k=7;
      else if (k==0 and fmines==mine) k=8;
      else if (k>0) k--;
      printer();
    }
    else if (volt < 100 or (volt > 100 and volt < 200)){
      if (k==7 and fmines!=mine) k=0;
      else if (k==7 and fmines==mine) k=8;
      else if (k==8) k=0;
      else if (k<8) k++;
      printer();
    }
    else if (volt > 700 and volt < 800){
      if (k==0){
        check(x, y);
      }
      else if (k==1 and mapa[x][y]==0) {
        mapa[x][y]='P';
        fmines++;
      }
      else if (k==1 and mapa[x][y]=='P') {
        mapa[x][y]=0;
        fmines--;
      }
      else if (k==2 and mode==0) mode=1;
      else if (k==2 and mode==1) mode=0;
      else if (k==3){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(mine-fmines);
        delay(2000);
      }
      else if (k==8) finish();
      else if (k==4){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(points);
        delay(2000);
      }
      else if (k==5) reset();
      else if (k==6) restart();
      if (state==3) state=2;
      k=7;
      printer();
    }
  }
  else if (state==4){ //koniec
    if (mode==2){
      if (volt > 100 and volt < 200 and y<sizey-1){
        y++;
        printer();
      }
      else if (volt > 300 and volt < 400 and y>0){
        y--;
        printer();
      }
      else if (volt > 500 and volt < 600 and x>0){
        x--;
        printer();
      }
      else if (volt < 100 and x<sizex-1){
        x++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        mode=3;
        k=0;
        printer();
      }
    }
    else if (mode==3){
      if (volt > 100 and volt < 200){
        if (k==2) k=0;
        else k++;
        printer();
      }
      else if (volt > 300 and volt < 400){
        if (k==0) k=2;
        else k--;
        printer();
      }
      else if (volt > 500 and volt < 600){
        if (k==0) k=2;
        else k--;
        printer();
      }
      else if (volt < 100){
        if (k==2) k=0;
        else k++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        mode=2;
        if (k==1) restart();
        else if (k==2) reset();
        k=0;
        printer();
      }
    }
  }
  else if (state==5){
    if (mode==2) {
      if (volt > 100 and volt < 200 and y<sizey-1){
        y++;
        printer();
      }
      else if (volt > 300 and volt < 400 and y>0){
        y--;
        printer();
      }
      else if (volt > 500 and volt < 600 and x>0){
        x--;
        printer();
      }
      else if (volt < 100 and x<sizex-1){
        x++;
        printer();
      }
   else if (volt > 700 and volt < 800){
      mode=3;
      k=0;
      printer();
      }
    }
    else if (mode==3){
      if (volt > 100 and volt < 200){
        if (k==2) k=0;
        else k++;
        printer();
      }
      else if (volt > 300 and volt < 400){
        if (k==0) k=2;
        else k--;
        printer();
      }
      else if (volt > 500 and volt < 600){
        if (k==0) k=2;
        else k--;
        printer();
      }
      else if (volt < 100){
        if (k==2) k=0;
        else k++;
        printer();
      }
      else if (volt > 700 and volt < 800){
        mode=2;
        if (k==1) restart();
        else if (k==2) reset();
        k=0;
        printer();
      }
    }
  }
  delay(200);
}

void printer(){
  if (state==1){
    if (mode==0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Konfiguracja gry");
    }
    else if (mode==1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wymiary planszy");
      lcd.setCursor(0,1);
      lcd.print(String(x)+ ", "+ String(y));
    }
    if (mode==2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Liczba min");
      lcd.setCursor(0,1);
      lcd.print(mine);
    }
    else if (mode==3){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Punkt startowy: ");
      lcd.setCursor(0,1);
      lcd.print(String(x+1)+ ", "+ String(y+1));
    }
  }
  else if (state==2){
    if (mode==0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(String(x+1)+", "+String(y+1));
      lcd.setCursor(0,1);
      if (mapa[x][y]==0) lcd.print("Nieodkryte");
      else if (mapa[x][y]=='P') lcd.print("Oznaczone");
      else  lcd.print("Odkryte "+String(mapa[x][y]));
    }
    else if (mode==1){
      lcd.clear();
      lcd.setCursor(6,0);
      if (y>=1){
        for (int l=x; l<x+5; l++){
          if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y-1]);
          else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(0,1);
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y]);
        else lcd.print("|");
      }
      lcd.setCursor(0,0);
      if (y+1<sizey){
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y+1]);
        else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(6,1);
      lcd.print("x "+String(x+1));
      lcd.setCursor(11,1);
      lcd.print("y "+String(y+1));
      lcd.setCursor(15,0);
      lcd.print(mapa[x][y]);
    }
  }
  else if (state==3){
    lcd.clear();
    lcd.setCursor(0,0);
    if (k==0) lcd.print("Odkryj to pole");
    else if (k==1) lcd.print("Oznacz pole");
    else if (k==2) lcd.print("Inny widok");
    else if (k==3) lcd.print("Ile min ukrytych");
    else if (k==4) lcd.print("Pola do odkrycia");
    else if (k==7) lcd.print("Anuluj");
    else if (k==8) lcd.print("Automatyczna gra");
    else if (k==5) lcd.print("Nowa gra");
    else if (k==6) lcd.print("Restart gry");
  }
  else if (state==4){ //koniec
    if (mode==1){
    unsigned long etime = millis();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mina!!!");
    tone(12,250,600);
    RGB_mode(64,255,255);
    delay(200);
    RGB_mode(255,255,255);
    delay(200);
    RGB_mode(64,255,255);
    delay(200);
    noTone(12);
    RGB_mode(255,255,255);
    delay(200);
    RGB_mode(64,255,255);
    delay(200);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Koniec gry!");
    Serial.println("Koniec gry!");
    lcd.setCursor(0,1);
    lcd.print("Czas " +String((etime-stime)/1000));
    RGB_mode(255,255,255);
    delay(200);
    RGB_mode(64,255,255);
    delay(200);
    RGB_mode(255,255,255);
    delay(200);
    RGB_mode(64,255,255);
    delay(200);
    RGB_mode(255,255,255);
    delay(200);
    RGB_mode(64,255,255);
    delay(300);
    mode=2;
    }
    else if (mode==2){
      lcd.clear();
      lcd.setCursor(6,0);
      if (y>=1){
        for (int l=x; l<x+5; l++){
          if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y-1]);
          else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(0,1);
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y]);
        else lcd.print("|");
      }
      lcd.setCursor(0,0);
      if (y+1<sizey){
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y+1]);
        else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(6,1);
      lcd.print("x "+String(x+1));
      lcd.setCursor(11,1);
      lcd.print("y "+String(y+1));
      lcd.setCursor(15,0);
      lcd.print(mapa[x][y]);
    }
    else if (mode==3){
      lcd.clear();
      lcd.setCursor(0,0);
      if (k==0) lcd.print("Anuluj");
      else if (k==1) lcd.print("Restart gry");
      else  if (k==2) lcd.print("Nowa gra");
    }
  }
  else if (state==5){ //wygrana
    if (mode==1){
    unsigned long etime = millis();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wygrana!");
    Serial.println("Wygrana!");
    lcd.setCursor(0,1);
    lcd.print("Czas " +String((etime-stime)/1000));
    RGB_mode(255,64,255);
    tone(12,2000,125);
    delay(125);
    tone(12,2200,125);
    delay(125);
    tone(12,2500,125);
    RGB_mode(255,160,255);
    delay(125);
    tone(12,2300,125);
    delay(125);
    RGB_mode(255,255,255);
    noTone(12);
    delay(250);
    RGB_mode(255,160,255);
    delay(250);
    RGB_mode(255,64,255);
    delay(250);
    RGB_mode(255,160,255);
    delay(250);
    RGB_mode(255,255,255);
    delay(250);
    RGB_mode(255,160,255);
    delay(250);
    RGB_mode(255,64,255);
    mode=2;
    }
    else if (mode==2){
      lcd.clear();
      lcd.setCursor(6,0);
      if (y>=1){
        for (int l=x; l<x+5; l++){
          if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y-l]);
          else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(0,1);
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y]);
        else lcd.print("|");
      }
      lcd.setCursor(0,0);
      if (y+1<sizey){
      for (int l=x; l<x+5; l++){
        if (l<sizex+2 and l>=2) lcd.print(mapa[l-2][y+1]);
        else lcd.print("|");
        }
      }
      else lcd.print("_____");
      lcd.setCursor(6,1);
      lcd.print("x "+String(x+1));
      lcd.setCursor(11,1);
      lcd.print("y "+String(y+1));
      lcd.setCursor(15,0);
      lcd.print(mapa[x][y]);
    }
    else if (mode==3){
      lcd.clear();
      lcd.setCursor(0,0);
      if (k==0) lcd.print("Anuluj");
      else if (k==1) lcd.print("Restart gry");
      else if (k==2) lcd.print("Nowa gra");
    }
  }
  delay(200);
}




void generic() {
  int t = mine;
  while (t > 0){
    int a = rand() % sizex;
    int b = rand() % sizey;
    if ((a!=x or b!=y) and maph[a][b] != 'M'){
      maph[a][b] = 'M';
      t--;
    }
  }
  for (int c=sizey-1; c>=0; c--){
    for (int d=0; d<sizex; d++){
      if(maph[d][c]!='M'){
        maph[d][c] = countm(d, c);
      }
      //Serial.print(maph[d][c]);
    }
    //Serial.println();
  }
}

void check(int xx, int yy){
  if (mapa[xx][yy]==0 and maph[xx][yy]!='M' and maph[xx][yy]!='-'){
    mapa[xx][yy]=maph[xx][yy];
    points--;
    tone(12,2500,125);
    delay(125);
    noTone(12);
    //tone
  }
  else if (maph[xx][yy]=='-'){
      mapa[xx][yy]='R';
      ext=true;
      tone(12,2200,125);
      delay(125);
      noTone(12);
      tone(12,2500,125);
      delay(125);
      noTone(12);
      extended();
      //tone
    }
  else if (maph[x][y]=='M' and mapa[x][y]!='P'){
    mapa[x][y]='!';
    state=4;
    mode=1;
    finish();
  }
  if (points==0){
    state=5;
    mode=1;
    finish();
  }
  else if (2*points>sizex*sizey-mine) RGB_mode(128,128,128);
  else if (2*points<=sizex*sizey-mine and 4*points>=sizex*sizey-mine) RGB_mode(255,255,64);
  else if (4*points<sizex*sizey-mine) RGB_mode(255,64,64);
}

void extended(){
  while (ext){
    ext=false;
    for (int c=0; c<sizey; c++){
      for (int d=0; d<sizex; d++){
        if (mapa[d][c]=='R'){
          mapa[d][c]='-';
          points--;
          ext = true;
          checknu(d,c);
        }
      }
    }
  }
}

void checknu(int xx, int yy){
  for (int a=yy+1; a>=0 and a>yy-2; a--){
    for (int b=xx+1; b>=0 and b>xx-2; b--){
      if (a<sizey and b<sizex and mapa[b][a]==0 and maph[b][a]!='-'){
        mapa[b][a]=maph[b][a];
        points--;
      }
      else if (a<sizey and b<sizex and mapa[b][a]==0 and maph[b][a]=='-'){
        mapa[b][a]='R';
      }
    }
  }
}


char countm(int cc, int dd){
  int coun = 0;
  for (int e=cc+1; e>=0 and e>cc-2; e--){
    for (int f=dd+1; f>=0 and f>dd-2; f--){
      if (e<sizex and f<sizey){
        if (maph[e][f]=='M') coun++;
      }
    }
  }
  if (coun>0) return ('0' + coun);
  return '-';
}

void finish(){
  for (int g=0; g<sizex; g++){
    for (int h=0; h<sizey; h++){
      if (maph[g][h]=='M' and mapa[g][h]!='P' and fmines==mine){ //autofinish przy źle oznaczonych minach - przegrana
        mapa[g][h]='!';
        state=4;
        mode=1;
      }
      else if (maph[g][h]=='M' and fmines!=mine and mapa[g][h]!='!') mapa[g][h]='M'; //nieoznaczona mina bez autofinish
      else if (maph[g][h]!='M' and mapa[g][h]=='P') mapa[g][h]='X'; //tu nie było miny, błąd przy oznaczaniu
      else if (mapa[g][h]==0) mapa[g][h]=maph[g][h]; //odsłoń nieodkryte pola
    }
  }
  if (state!=4){
  state=5;
  mode=1;
  }
  printer();
}

void reset(){
  memset(maph,0,sizeof(maph));
  memset(mapa,0,sizeof(mapa));
  state=0;
  mode=0;
  fmines=0;
  printer();
}

void restart(){
  memset(mapa,0,sizeof(mapa));
  state=2;
  mode=0;
  fmines=0;
  points=sizex*sizey-mine;
  RGB_mode(128,128,128);
  stime=millis();
  printer();
}

void RGB_mode(int redc, int greenc, int bluec){
  analogWrite(11,redc);
  analogWrite(10,greenc);
  analogWrite(3,bluec);
}
