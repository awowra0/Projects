import random
import tkinter as tk


class Board:
    def __init__(self, sizeX=5, sizeY=5, mines=5):
        self.sizeX = sizeX
        self.sizeY = sizeY
        self.mines = min(mines, sizeX * sizeY - 5)
        self.minesleft = self.mines
        self.left = sizeX * sizeY - mines
        self.field = [[" " for i in range(sizeX)] for j in range(sizeY)]
        self.gamefield = [[" " for i in range(sizeX)] for j in range(sizeY)]
        self.mistakes = []
        self.started = False
        self.playable = True
        self.__buildgui()
        
    def __buildgui(self):
        self.tk = tk.Tk(screenName="Mine", baseName="Mines", className='Tk', useTk=1)
        self.tk.title("Mine")
        self.btnew = tk.Button(self.tk, text="NEW", command=lambda: self.__newgame())
        self.btnew.grid(row=0, column=1)
        self.labmin = tk.Label(self.tk, text=f"{self.minesleft}")
        self.labmin.grid(row=0, column=self.sizeX - 2)
        self.buttons = [[tk.Label(self.tk, text=" ", width=5, height=3, relief="raised", fg="black", bg="snow") for i in range(self.sizeX)] for j in range(self.sizeY)]
        for i, ii in enumerate(self.buttons):
            for j, jj in enumerate(ii):
                jj.grid(row=i+1, column=j)
                jj.bind("<Button-1>", self.__click)
                jj.bind("<Button-2>", self.__rclick)
                jj.bind("<Button-3>", self.__rclick)
        self.tk.option_add('*tearOff', False)
        self.menu = tk.Menu(self.tk)
        self.tk.config(menu=self.menu)
        self.gamemenu = tk.Menu(self.menu)
        self.menu.add_cascade(label='Game', menu=self.gamemenu)
        self.gamemenu.add_command(label='New', command=lambda: self.__newgame())
        self.gamemenu.add_command(label='Options', command=lambda: self.__options())
        self.gamemenu.add_command(label='Exit', command=lambda: self.tk.destroy())
        self.tk.mainloop()

    def __options(self):
        tempx = self.sizeX
        tempy = self.sizeY
        tempmin = self.mines
        opwindow = tk.Toplevel(self.tk)
        opwindow.title = "Mine - Options"
        xlab = tk.Label(opwindow, text="Width")
        xbox = tk.Spinbox(opwindow, from_=5, to=30)
        ylab = tk.Label(opwindow, text="Height")
        ybox = tk.Spinbox(opwindow, from_=5, to=30)
        minlab = tk.Label(opwindow, text="Mines")
        minbox = tk.Spinbox(opwindow, from_=5, to=895)
        butexit = tk.Button(opwindow, text="OK",command=lambda: opwindow.quit())
        xlab.pack()
        xbox.pack()
        ylab.pack()
        ybox.pack()
        minlab.pack()
        minbox.pack()
        butexit.pack()
        xbox.delete(0, "end")
        ybox.delete(0, "end")
        minbox.delete(0, "end")
        xbox.insert(0, self.sizeX)
        ybox.insert(0, self.sizeY)
        minbox.insert(0, self.mines)
        opwindow.mainloop()
        try:
            newx, newy, newmin = int(xbox.get()), int(ybox.get()), int(minbox.get())
            if  newx != tempx or newy != tempy:
                self.mines = min(newmin, newx * newy - 5)
                self.__resize(newx, newy)
                self.__newgame()
            elif newmin != tempmin:
                self.mines = min(newmin, self.sizeX * self.sizeY - 5)
                self.__newgame()
        except:
            pass
        opwindow.destroy()
     
    def __resize(self, sizeX=0, sizeY=0):
        changed = False
        if (isinstance(sizeX, int) and 5 <= sizeX <= 30):
            self.sizeX = sizeX
            changed = True
        if (isinstance(sizeY, int) and 5 <= sizeY <= 30):
            self.sizeY = sizeY
            changed = True
        if changed:
            self.field = [[" " for i in range(self.sizeX)] for j in range(self.sizeY)]
            self.gamefield = [[" " for i in range(self.sizeX)] for j in range(self.sizeY)]
            self.labmin.grid(row=0, column=self.sizeX - 2)
            self.buttons = [[tk.Label(self.tk, text=" ", width=5, height=3, relief="raised", fg="black", bg="snow") for i in range(sizeX)] for j in range(sizeY)]
            for i, ii in enumerate(self.buttons):
                for j, jj in enumerate(ii):
                    jj.grid(row=i+1, column=j)
                    jj.bind("<Button-1>", self.__click)
                    jj.bind("<Button-2>", self.__rclick)
                    jj.bind("<Button-3>", self.__rclick)
            
    def __fill(self, X=0, Y=0):
        indexes = [i for i in range(self.sizeX * self.sizeY)]
        indexes.remove(Y * self.sizeY + X)
        placedMines = 0
        while placedMines < self.mines:
            chosen = random.choice(indexes)
            indexes.remove(chosen)
            self.field[chosen // self.sizeX][chosen % self.sizeX] = "*"
            placedMines += 1
        self.__countNeighbors()
    
    def __countNeighbors(self):
         for j, jj in enumerate(self.field):
             for i, ii in enumerate(jj):
                 if ii == "*":
                     continue
                 number = 0
                 for k in [(i-1, j-1), (i-1, j), (i-1, j+1), (i, j-1), (i, j+1), (i+1, j-1), (i+1, j), (i+1, j+1)]:
                     number += self.__countHELPER(k[0], k[1])
                 self.field[j][i] = str(number)
    
    def __countHELPER(self, X, Y):
        if X < 0 or X >= self.sizeX or Y < 0 or Y >= self.sizeY:
            return 0
        elif self.field[Y][X] == "*":
            return 1
        return 0
    
    def __color(self, X, Y):
        self.buttons[Y][X]["bg"] = "gray95"
        match self.buttons[Y][X]["text"]:
            case "0":
                self.buttons[Y][X]["fg"] = "white"
            case "1":
                self.buttons[Y][X]["fg"] = "blue"
            case "2":
                self.buttons[Y][X]["fg"] = "green"
            case "3":
                self.buttons[Y][X]["fg"] = "red"
            case "4":
                self.buttons[Y][X]["fg"] = "purple"
            case "5":
                self.buttons[Y][X]["fg"] = "brown"
            case "6":
                self.buttons[Y][X]["fg"] = "cyan"
            case "7":
                self.buttons[Y][X]["fg"] = "black"
            case "8":
                self.buttons[Y][X]["fg"] = "gray50"
            case "*":
                self.buttons[Y][X]["fg"] = "black"
                self.buttons[Y][X]["bg"] = "red"
                self.mistakes.append((X, Y))
            case "X":
                self.buttons[Y][X]["fg"] = "red"


    def __click(self, event):
        x = event.x_root - self.tk.winfo_rootx()
        y = event.y_root - self.tk.winfo_rooty()
        X, Y = self.tk.grid_location(x, y)
        Y -= 1
        if not self.started:
            self.__fill(X, Y)
            self.started = True
        if self.playable:
            if self.buttons[Y][X]["relief"] == "raised" and self.buttons[Y][X]["text"] != "M":
                if self.field[Y][X] == "*":
                    self.gamefield[Y][X] == "*"
                    self.buttons[Y][X]["text"] = "*"
                    self.buttons[Y][X]["relief"] = "sunken"
                    self.playable = False
                elif self.field[Y][X] == "0":
                    self.__showSpace(X, Y)
                else:
                    self.gamefield[Y][X] = self.field[Y][X]
                    self.buttons[Y][X]["text"] = self.field[Y][X]
                    self.buttons[Y][X]["relief"] = "sunken"
                    self.left -= 1
                self.__color(X, Y)
            else:
                if "1" <= self.buttons[Y][X]["text"] <= "7":
                    self.__multiclick(X, Y, self.buttons[Y][X]["text"])
        if self.left == 0 and self.playable:
            self.playable = False
            self.labmin["text"] = f"0"
            for i in range(self.sizeX):
                for j in range(self.sizeY):
                    if self.field[j][i] == "*":
                        self.buttons[j][i]["text"] = "M"
        elif not self.playable:
            for i in range(self.sizeX):
                for j in range(self.sizeY):
                    if self.buttons[j][i]["text"] == " " and self.field[j][i] == "*":
                        self.buttons[j][i]["text"] = self.field[j][i]
                    elif self.buttons[j][i]["text"] == "M" and self.field[j][i] != "*":
                        self.buttons[j][i]["text"] = "X"
                        self.__color(i, j)

    def __multiclick(self, x, y, text):
        num = 0
        sparelist = []
        for i in [(x-1, y-1), (x-1, y), (x-1, y+1), (x, y-1), (x, y+1), (x+1, y-1), (x+1, y), (x+1, y+1)]:
            if i[0] < 0 or i[0] >= self.sizeX or i[1] < 0 or i[1] >= self.sizeY or self.buttons[i[1]][i[0]]["relief"] == "sunken":
                continue
            elif self.buttons[i[1]][i[0]]["text"] == "M":
                num += 1
            else:
                sparelist.append(i)
        if num == int(text):
            for k in sparelist:
                i, j = k[0], k[1]
                if self.field[j][i] == "*":
                    self.gamefield[j][i] == "*"
                    self.buttons[j][i]["text"] = "*"
                    self.buttons[j][i]["relief"] = "sunken"
                    self.playable = False
                elif self.field[j][i] == "0":
                    self.__showSpace(i, j)
                elif self.gamefield[j][i] == " ":
                    self.gamefield[j][i] = self.field[j][i]
                    self.buttons[j][i]["text"] = self.field[j][i]
                    self.buttons[j][i]["relief"] = "sunken"
                    self.left -= 1
                self.__color(i, j)

    def __showSpace(self, XX, YY):
        queue = [(XX, YY)]
        while len(queue) > 0:
            point = queue.pop(0)
            x, y = point[0], point[1]
            if x < 0 or x >= self.sizeX or y < 0 or y >= self.sizeY:
                continue
            elif self.gamefield[y][x] != " ":
                continue
            else:
                self.gamefield[y][x] = self.field[y][x]
                self.buttons[y][x]["text"] = self.field[y][x]
                self.buttons[y][x]["relief"] = "sunken"
                self.left -= 1
                self.__color(x, y)
                if self.gamefield[y][x] == "0":
                    for i in [(x-1, y-1), (x-1, y), (x-1, y+1), (x, y-1), (x, y+1), (x+1, y-1), (x+1, y), (x+1, y+1)]:
                        queue.append(i)

    def __rclick(self, event):
        button = event.widget
        if self.playable and self.started and button["state"] == "normal":
            if button["text"] == " ":
                button["text"] = "M"
                self.minesleft -= 1
            elif button["text"] == "M":
                button["text"] = " "
                self.minesleft += 1
            self.labmin["text"] = f"{self.minesleft}"

    def __newgame(self):
        self.field = [[" " for i in range(self.sizeX)] for j in range(self.sizeY)]
        self.gamefield = [[" " for i in range(self.sizeX)] for j in range(self.sizeY)]
        self.left = self.sizeX * self.sizeY - self.mines
        self.minesleft = self.mines
        self.started = False
        self.playable = True
        self.labmin["text"] = f"{self.minesleft}"
        for i in self.buttons:
            for j in i:
                j["text"] = " "
                j["relief"] = "raised"
                j["fg"] = "black"
        for j in self.mistakes:
            self.buttons[j[1]][j[0]]["bg"] = "snow"
        self.mistakes = [] 


try:
    playground = Board()
    # playground.tk.mainloop()
except Exception as e:
    print(e)
# input("Press Enter")
