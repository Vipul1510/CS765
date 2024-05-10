from treelib import Tree
import subprocess
import argparse

# Standard functions from https://treelib.readthedocs.io/en/latest/ are used
def show(file_name):
    # Create the tree from the input file
    tree = Tree()
    with open(file_name, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip().split()
            node_id = line[0]
            parent_id = line[1]
            label = f"{node_id}\n{line[2]}"
            if parent_id == '-1':
                tree.create_node(label, node_id, data={"shape": "box", "style": "filled", "fillcolor": "#e6e6e6"})
            else:
                tree.create_node(label, node_id, parent=parent_id, data={"shape": "box", "style": "filled", "fillcolor": "#e6e6e6"})
    
    # Export the tree to DOT format
    dot_file = file_name[:-3] + "dot"
    tree.to_graphviz(dot_file,shape="box")
    
    # Convert DOT to PDF using Graphviz
    output = subprocess.check_output(['dot', '-Tpdf', dot_file])

    # Write the PDF output
    pdf_file = file_name[:-3] + "pdf"
    with open(pdf_file, 'wb') as f:
        f.write(output)

# Parser to parse the command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('--numpeers', type=int)
parser.add_argument('--slowpercent', type=float)
parser.add_argument('--lowcpupercent', type=float)
parser.add_argument('--intertxntime', type=float)
parser.add_argument('--interblktime', type=float)
args = parser.parse_args()

if args.numpeers is None:
    print("Argument NUMPEERS missing")
    exit(0)
if args.slowpercent is None:
    print("Argument SLOWPERCENT missing")
    exit(0)
if args.lowcpupercent is None:
    print("Argument LOWCPUPERCENT missing")
    exit(0)
if args.intertxntime is None:
    print("Argument INTERTXNTIME missing")
    exit(0)
if args.interblktime is None:
    print("Argument INTERBLKTIME missing")
    exit(0)

# Removing previously existing *.dot, *.txt, *.pdf and *.out files
remove_previous_files = subprocess.Popen("rm -rf *.txt *.dot *.pdf *.out", shell=True)
remove_previous_files.wait()
# Compiling cpp file
compile_process = subprocess.Popen("g++ implementation.cpp -o a.out", shell=True)
compile_process.wait()  
# running executable with given arguments
run_process = subprocess.Popen(f"./a.out {args.numpeers} {args.slowpercent} {args.lowcpupercent} {args.intertxntime} {args.interblktime} ", shell=True)
run_process.wait()

# Visualization
for i in range(args.numpeers):
    show(f"node_{i}.txt")