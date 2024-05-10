CS765 Assignment 1

Simulation of a P2P Cryptocurrency Network

usage: python3 main.py [-h] [--numpeers NUMPEERS] [--slowpercent SLOWPERCENT] [--lowcpupercent LOWCPUPERCENT] [--intertxntime INTERTXNTIME] [--interblktime INTERBLKTIME]

Example usage: "python3 main.py --numpeers 20 --slowpercent 10 --lowcpupercent 90 --intertxntime 600 --interblktime 30" 

NUMPEERS : Number of peers
SLOWPERCENT : % of peers which are slow
LOWCPUPERCENT : % of peers which have low CPU
INTERTXNTIME : Mean interarrival time between two txns (seconds)
INTERBLKTIME : Mean interarrival time between two blocks (seconds)

// For each node(miner) blockchain will be printed in node_{node_number}.pdf
// Each  block in blockchain contains BlkId and Time of arrival

// Requirements:
// 1. python treelib library (pip install treelib)
// 2. graphviz (sudo apt install graphviz)
