CS765 Assignment 2

Simulating a double selfish mining attack using the P2P Cryptocurrency Network developed in Assignment 1

usage: main.py [-h] [--numpeers NUMPEERS] [--intertxntime INTERTXNTIME] [--interblktime INTERBLKTIME] [--adv1hashpower ADV1HASHPOWER] [--adv2hashpower ADV2HASHPOWER]

Example usage: "python3 main.py --numpeers 10 --intertxntime 400 --interblktime 600 --adv1hashpower 40 --adv2hashpower 40" 

NUMPEERS : Number of peers
INTERTXNTIME : Mean interarrival time between two txns (seconds)
INTERBLKTIME : Mean interarrival time between two blocks (seconds)
ADV1HASHPOWER : Hashing power of adversary 1 (in %)
ADV2HASHPOWER : Hashing power of adversary 2 (in %)

// Log can be found in log.txt which includes graph also.
// For each node(miner) blockchain will be printed in node_{node_number}.pdf
// In visualization blocks of Attacker 1 are coloured by green, blocks of Attacker 2 are coloured by red and remaining white coloured blocks are created by honest nodes.
// Each  block in blockchain contains BlkId and Time of arrival
// In results folder plot.py is present which is used to plot the results. Result images which are used in report are also present in folder result.

// Requirements:
// 1. python treelib library (pip install treelib)
// 2. graphviz (sudo apt install graphviz)
