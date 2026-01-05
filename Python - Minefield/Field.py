import tkinter as tk
import random


class Board:
    def __init__(self):
        self.mines = 40
        self.field = [[" " for i in range(21)] for j in range(21)]
        self.gamefield = [[" " for i in range(21)] for j in range(21)]
        self.started = False
        self.playable = True
        self.chances = 3
        self.X = 10
        self.Y = 0
        self.level = 1
        self.steps = 0
        self.diff = "n"
        self.growth = 5
        self.maxmines = 100
        self.__buildgui()
        
    def __buildgui(self):
        self.tk = tk.Tk(screenName=None, baseName=None, className='Tk', useTk=1)
        self.tk.title("Field")
        self.lablev = tk.Label(self.tk, text=f"{self.level}")
        self.lablev.grid(row=0, column=5)
        self.labl = tk.Label(self.tk, text=f"L")
        self.labl.grid(row=0, column=4)
        self.labcha = tk.Label(self.tk, text=f"{self.chances}")
        self.labcha.grid(row=0, column=16)
        self.labp = tk.Label(self.tk, text=f"+")
        self.labp.grid(row=0, column=15)
        self.labdiff = tk.Label(self.tk, text=f"{self.diff.upper()}")
        self.labdiff.grid(row=0, column=10)
        self.laba = tk.Label(self.tk, text=f"*")
        self.laba.grid(row=0, column=9)
        self.labaa = tk.Label(self.tk, text=f"*")
        self.labaa.grid(row=0, column=11)
        self.frames = [[tk.Label(self.tk, text=" ", bg="snow", width=3, height=2, relief="raised") for i in range(21)] for j in range(21)]
        for i, ii in enumerate(self.frames):
            for j, jj in enumerate(ii):
                jj.grid(row=21-i, column=j)
        self.frames[0][10]["relief"] = "sunken"
        self.frames[20][10]["text"] = "*"
        self.frames[0][10]["text"] = "*"
        self.frames[0][10]["underline"] = 0
        self.gamefield[0][10] = "*"
        self.tk.option_add('*tearOff', False)
        self.menu = tk.Menu(self.tk)
        self.tk.config(menu=self.menu)
        self.gamemenu = tk.Menu(self.menu)
        self.menu.add_cascade(label='Game', menu=self.gamemenu)
        self.gamemenu.add_command(label='New', command=lambda: self.__newgame())
        self.gamemenu.add_command(label='Exit', command=lambda: self.tk.destroy())
        self.diffmenu = tk.Menu(self.menu)
        self.menu.add_cascade(label='Difficulty', menu=self.diffmenu)
        self.diffmenu.add_command(label="Easy", command=lambda: self.__diffchange("e"))
        self.diffmenu.add_command(label="Normal", command=lambda: self.__diffchange("n"))
        self.diffmenu.add_command(label="Hard", command=lambda: self.__diffchange("h"))
        self.tk.bind("<Up>", lambda x: self.__move("u"))
        self.tk.bind("<Down>", lambda x: self.__move("d"))
        self.tk.bind("<Left>", lambda x: self.__move("l"))
        self.tk.bind("<Right>", lambda x: self.__move("r"))
        self.__fill()
        self.tk.mainloop()

    def __diffchange(self, diff):
        if self.diff != diff:
            self.diff = diff
            self.labdiff["text"] = f"{self.diff.upper()}"
            self.__newgame()

    def __newgame(self):
        self.field = [[" " for i in range(21)] for j in range(21)]
        self.gamefield = [[" " for i in range(21)] for j in range(21)]
        match self.diff:
            case "n":        
                self.mines = 40
                self.chances = 3
                self.growth = 5
                self.maxmines = 100 #13
            case "e":
                self.mines = 30
                self.chances = 5
                self.growth = 3
                self.maxmines = 75 #16
            case "h":
                self.mines = 55
                self.chances = 3
                self.growth = 7
                self.maxmines = 125 #11
        self.labcha["text"] = self.chances
        self.level = 1
        self.lablev["text"] = self.level
        self.started = False
        self.playable = True
        self.X = 10
        self.Y = 0
        self.steps = 0
        for i in self.frames:
            for j in i:
                j.configure(text=" ")
                j["relief"] = "raised"
                j["bg"] = "snow"
                j["underline"] = -1
        self.frames[0][10]["relief"] = "sunken"
        self.frames[0][10]["text"] = "*"
        self.frames[0][10]["underline"] = 0
        self.frames[20][10]["text"] = "*"
        self.gamefield[0][10] = "*"
        self.__fill()
    
    def __fill(self):
        broute = False
        while not broute:
            self.field = [[" " for i in range(21)] for j in range(21)]
            indexes = [i for i in range(21 * 21)]
            indexes.remove(9)
            indexes.remove(10)
            indexes.remove(11)
            indexes.remove(31)
            indexes.remove(20 * 21 + 10)
            indexes.remove(20 * 21 + 9)
            indexes.remove(20 * 21 + 11)
            indexes.remove(19 * 21 + 10)

            indexes.remove(8)
            indexes.remove(12)
            indexes.remove(30)
            indexes.remove(32)
            indexes.remove(52)
            placedMines = 0
            while placedMines < self.mines:
                chosen = random.choice(indexes)
                indexes.remove(chosen)
                self.field[chosen // 21][chosen % 21] = "M"
                placedMines += 1
            self.__countNeighbors()
            broute = self.__levelcheck()
    
    def __levelcheck(self):
        queue = [(10, 0, 0)]
        graph = [[" " for i in range(21)] for j in range(21)]
        while len(queue) > 0:
            coord = queue.pop(0)
            if 0 <= coord[0] <= 20 and 0 <= coord[1] <= 20 and graph[coord[1]][coord[0]] == " ":
                graph[coord[1]][coord[0]] = coord[2]
                if coord[0] > 0:
                    queue.append((coord[0]-1,coord[1],coord[2]+1))
                if coord[0] < 20:
                    queue.append((coord[0]+1,coord[1],coord[2]+1))
                if coord[1] > 0:
                    queue.append((coord[0],coord[1]-1,coord[2]+1))
                if coord[1] < 20:
                    queue.append((coord[0],coord[1]+1,coord[2]+1))
        if graph[20][10] == " ":
            print("Level generation failed ... Trying again")
            return False
        return True
    
    def __countNeighbors(self):
         for j, jj in enumerate(self.field):
             for i, ii in enumerate(jj):
                 if ii == "M":
                     continue
                 number = 0
                 for k in [(i-1, j), (i, j-1), (i, j+1), (i+1, j)]:
                     number += self.__countHELPER(k[0], k[1])
                 self.field[j][i] = str(number)
    
    def __countHELPER(self, X, Y):
        if X < 0 or X >= 21 or Y < 0 or Y >= 21:
            return 0
        elif self.field[Y][X] == "M":
            return 1
        return 0
    
    def __move(self, direction):
        if not self.playable:
            return
        self.frames[self.Y][self.X]["relief"] = "flat"
        self.frames[self.Y][self.X]["underline"] = -1
        match direction:
            case "u":
                if self.Y < 20:
                    self.Y += 1
            case "d":
                if self.Y > 0:
                    self.Y -= 1
            case "l":
                if self.X > 0:
                    self.X -= 1
            case "r":
                if self.X < 20:
                    self.X += 1
        self.frames[self.Y][self.X]["relief"] = "sunken"
        self.frames[self.Y][self.X]["underline"] = 0
        if self.X == 10 and self.Y == 20:
            self.__switchlevel()
        elif self.gamefield[self.Y][self.X] == " ":
            self.steps += 1
            if self.field[self.Y][self.X] == "M":
                self.chances -= 1
                self.labcha["text"] = self.chances
                self.gamefield[self.Y][self.X] = "X"
                self.frames[self.Y][self.X]["text"] = "X"
            else:
                self.gamefield[self.Y][self.X] = self.field[self.Y][self.X]
                self.frames[self.Y][self.X]["text"] = self.field[self.Y][self.X]
            match self.field[self.Y][self.X]:
                case "0":
                    self.frames[self.Y][self.X]["bg"] = "green"
                case "1":
                    self.frames[self.Y][self.X]["bg"] = "yellow"
                case "2":
                    self.frames[self.Y][self.X]["bg"] = "orange"
                case "3":
                    self.frames[self.Y][self.X]["bg"] = "red"
                case "4":
                    self.frames[self.Y][self.X]["bg"] = "purple"
                case "M":
                    self.frames[self.Y][self.X]["bg"] = "gray50"
        if self.chances < 1:
            self.playable = False
            self.__score()
            self.__newgame()
    
    def __score(self):
        scorewindow = tk.Toplevel(self.tk)
        labdiff = f"Difficulty: "
        score = 0
        match self.diff:
            case "e":
                labdiff += "Easy"
                score = self.steps + (self.level - 1) * 50
            case "n": 
                labdiff += "Normal"
                score = self.steps * 3 + (self.level - 1) * 200
            case "h":
                labdiff += "Hard"
                score = self.steps * 5 + (self.level - 1) * 500
        lablev = tk.Label(scorewindow, text=f"Level: {self.level}")
        labscore = tk.Label(scorewindow, text=f"Score: {score}")
        butend = tk.Button(scorewindow, text="Exit", command=lambda: self.tk.destroy())
        butnew = tk.Button(scorewindow, text="New Game", command=lambda: scorewindow.quit())
        lablev.pack()
        labscore.pack()
        butnew.pack()
        butend.pack()
        #scorewindow.transient(self.tk)
        #scorewindow.attributes("-topmost", True)
        scorewindow.geometry(f"{self.tk.winfo_x() + 200}x{self.tk.winfo_x() + 200}")
        scorewindow.grab_set()
        scorewindow.mainloop()
        scorewindow.destroy()
    
    def __switchlevel(self):
        self.X = 10
        self.Y = 0
        self.mines = min(self.maxmines, self.mines + self.growth)
        if self.level < 99:
            self.level += 1
        else:
            self.level = 0
        if self.level % 5 == 0 and self.chances < 99:
            self.chances += 1
            self.labcha["text"] = self.chances
        self.lablev["text"] = self.level
        self.field = [[" " for i in range(21)] for j in range(21)]
        self.gamefield = [[" " for i in range(21)] for j in range(21)]
        for i in self.frames:
            for j in i:
                j.configure(text=" ")
                j["relief"] = "raised"
                j["bg"] = "snow"
                j["underline"] = -1
        self.frames[0][10]["relief"] = "sunken"
        self.frames[0][10]["text"] = "*"
        self.frames[0][10]["underline"] = 0
        self.frames[20][10]["text"] = "*"
        self.gamefield[0][10] = "*"
        self.__fill()


try:
    playground = Board()
    # playground.tk.mainloop()
except Exception as e:
    print(e)
# input("Press Enter")