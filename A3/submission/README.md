CS765 HW-3

DApp for fact-checking a news article or item

usage: simulation.py [--numvoters NUMVOTERS] [--fractionlesshonest FRACTIONLESSHONEST] [--fractionmalicious FRACTIONMALICIOUS]

Example usage: "python3 simulation.py --numvoters 200 --fractionlesshonest 0.7 --fractionmalicious 0.1" 

NUMVOTERS : Number of voters
FRACTIONLESSHONEST : fraction of voters who are very trustworthy and give the correct vote with probability 0.9 
FRACTIONMALICIOUS : fraction of voters who are malicious and deliberately choose the exact wrong answer

- Values of p and q belongs to (0,1) 
- N is an positive integer
- Generated plot will be saved as 'plot_{N}_{p}_{q}.png'
