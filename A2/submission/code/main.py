from graphviz import Digraph
import subprocess
import argparse

def show(file_name):
    # Create a new Digraph
    dot = Digraph()

    # Read the input file and add nodes and edges to the Digraph
    with open(file_name, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip().split()
            node_id = line[0]
            parent_id = line[1]
            label = f"{node_id}\n{line[2]}"
            color = "green" if int(line[3]) == args.numpeers - 2 else "red" if int(line[3]) == args.numpeers - 1 else "white"
            dot.node(node_id, label=label, shape='box', style='filled', fillcolor=color)
            if parent_id != '-1':
                dot.edge(parent_id, node_id)

    # Render the Digraph to a PDF file
    dot_file = file_name[:-3]
    pdf_file = file_name[:-4]
    dot.render(pdf_file, format='pdf', cleanup=True)

# Parser to parse the command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('--numpeers', type=int)
# parser.add_argument('--slowpercent', type=float)
# parser.add_argument('--lowcpupercent', type=float)
parser.add_argument('--intertxntime', type=float)
parser.add_argument('--interblktime', type=float)
parser.add_argument('--adv1hashpower', type=float)
parser.add_argument('--adv2hashpower', type=float)
args = parser.parse_args()

if args.numpeers is None:
    print("Argument NUMPEERS missing")
    exit(0)
if args.intertxntime is None:
    print("Argument INTERTXNTIME missing")
    exit(0)
if args.interblktime is None:
    print("Argument INTERBLKTIME missing")
    exit(0)
if args.adv1hashpower is None:
    print("Argument AADV1HASHPOWER is missing")
    exit(0)
if args.adv2hashpower is None:
    print("Argument ADV2HASHPOWER is missing")
    exit(0)

# Removing previously existing *.dot, *.txt, *.pdf and *.out files
remove_previous_files = subprocess.Popen("rm -rf *.txt *.dot *.pdf *.out", shell=True)
remove_previous_files.wait()
# Compiling cpp file
compile_process = subprocess.Popen("g++ implementation.cpp -o a.out", shell=True)
compile_process.wait()  
# running executable with given arguments
run_process = subprocess.Popen(f"./a.out {args.numpeers} {50} {args.intertxntime} {args.interblktime} {args.adv1hashpower} {args.adv2hashpower} > log.txt", shell=True)
run_process.wait()

# Visualization
for i in range(args.numpeers):
    show(f"node_{i}.txt")