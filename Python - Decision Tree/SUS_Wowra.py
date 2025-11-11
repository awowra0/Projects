import math

def main(filename="test.txt", tosplit=",", dec=-1):
    f = open(filename, "r")
    A=[]
    for i in f:
        A.append(i.replace("\n","").split(tosplit))
    f.close()
    start = create_tree(A, dec)
    visual(start, dec, filename)
    #print(superprint(start))

def create_tree(A, dec):
	start = Node(A, dec)
	Q = [start]
	while len(Q) > 0:
		curr = Q[0]
		if curr.val != 0:
			B = findunique(curr.A)#list of lists of attributes' values
			E = [[j for j in curr.A if j[curr.atr] == B[curr.atr][i]] for i in range(len(B[curr.atr]))] #list of lists of objects divided by attribute
			curr.next = [Node(E[i],dec,curr.atr,curr.depth+1) for i in range(len(E))]
		for i in curr.next:
			Q.append(i)
		Q.pop(0)
	return start

def visual(start, dec, filename):
	print("Decision tree\n")
	text = f"Decision tree from {filename}\n"
	A = [start]
	curr = A[0]
	A.pop(0)
	print(f"{'  '*curr.depth}   A{curr.atr+1}")
	text += f"{'  '*curr.depth}   A{curr.atr+1}\n"
	for i in curr.next[::-1]:
		A.append(i)
	while len(A)>0:
		curr = A[0]
		if len(curr.next)>0:
			print(f"{'   '*curr.depth}\--{curr.A[0][curr.decided]}|A{curr.atr+1}")
			text += f"{'   '*curr.depth}\--{curr.A[0][curr.decided]}|A{curr.atr+1}\n"
		else:
			print(f"{'   '*curr.depth}\--{curr.A[0][curr.decided]}->{curr.A[0][dec]}")
			text += f"{'   '*curr.depth}\--{curr.A[0][curr.decided]}->{curr.A[0][dec]}\n"
		for i in curr.next[::-1]:
			A.insert(1,i)
		A.pop(0)
	try:
		f = open("SUS-res.txt","w")
		f.write(text)
		f.close()
	except:
		print("Failed to write results to file.")

def superprint(start):
	A = [start]
	text = ""
	while len(A) > 0:
		text += f"Node containing {A[0].A}, \nbestpick: {A[0].atr} {A[0].val}. Depth: {A[0].depth}."
		if len(A[0].next) > 0:
			text += "\n"
			for i in A[0].next:
				A.append(i)
		else:
			text += " LEAF.\n"
		A.pop(0)
	return text


class Node:
	def __init__(self, A, dec, decided = None, depth = 0):
		self.A = A #list of objects to divide
		self.depth = depth #node depth
		self.decided = decided #attribute dividing parent node
		self.val, self.atr = findbestpick(A, dec) #balanced gain raisement and attribute index
		self.next = []
	
	def __str__(self):
		print(f"Node containing {self.A}, \nbestpick: {self.atr} {self.val}. Depth: {self.depth}.")
		if len(self.next) > 0:
			for i in self.next:
				print(i)
		else:
			print("LEAF")
		return ""


def show(A):
    print(A)

def findunique(A):
    B = [[] for i in range(len(A[0]))]
    for i in A:
        for j in range(len(A[0])):
            if i[j] not in B[j]:
                B[j].append(i[j])
    return B#every possible attribute value

def countvars(A):
    C = [{} for i in range(len(A[0]))]
    for i in A:
        for j in range(len(A[0])):
            if i[j] not in C[j]:
                C[j][i[j]] = 1
            else:
                C[j][i[j]] += 1
    return C#count of occurences of every attribute value

def prob(A):
    C = countvars(A)
    D = [{} for i in range(len(C))]
    for ii, i in enumerate(C):
        temp = 0
        for j in i:
            temp += i[j]
        for k in i:
            D[ii][k] = i[k]/temp
    return D #probability of occuring of chosen attribute value

def findbestpick(A, dec=-1):
    B = findunique(A)
    C = countvars(A)
    INFO = []#information value
    ent = [] #attributes entropy
    ENTROPY = 0#decisive attribute entropy
    temp = 0
    D = prob(A)
    for m in D:#attributes entropy
        temp = 0
        for n in m:
            temp += m[n] * math.log2(m[n])
        ent.append(-temp)
    for n in D[dec]: #decisive attribute entropy
        ENTROPY += D[dec][n] * math.log2(D[dec][n])
    ENTROPY *= -1
    for pp, p in enumerate(D):#pick atr, INFO
        if pp==dec or (dec==-1 and pp==len(D)-1):
            continue
        temp = 0
        INFOT = 0
        for r in p:#pick val from atr
            S = {}
            ss = 0
            for s in A:
                if s[dec] not in S:
                    S[s[dec]] = 0
                if r == s[pp]:
                    S[s[dec]] += 1
                    ss += 1
            temp += p[r] * ENT(S, ss)
        INFO.append(temp)
    GAIN = [] #information raisement
    for i in INFO:
        GAIN.append(ENTROPY-i)
    GAINRATIO = [] #balanced information raisement
    for ii,i in enumerate(GAIN):
    	try:
    		GAINRATIO.append(i/ent[ii])
    	except:
    		GAINRATIO.append(0)
    #print("Gainratio")
    #for i in GAINRATIO:
    #    print(i)
    #print("")
    bestpick = GAINRATIO[0]
    bestindex = 0
    for kk,k in enumerate(GAINRATIO):
    	if k > bestpick:
    		bestpick = k
    		bestindex = kk
    return bestpick, bestindex #balanced information raisement, attribute index

def ENT(S, ss):
    ent = 0
    for i in S:
        try:
            ent += S[i]/ss * math.log2(S[i]/ss)
        except:
            pass
    return -ent #single attribute entropy

