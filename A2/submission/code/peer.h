#include <iostream>
#include <vector>
#include <random>  
#include <algorithm>
using namespace std; 
std::random_device rd;
std::mt19937 gen(rd());
double miningfee = 50;
struct Transaction{
    int sender_id;
    int receiver_id;
    double amount;
    int txn_id;// Transaction id
    Transaction(){}
    Transaction(int txn_id,int sender_id,int receiver_id,double amount){//Constructor for storing all the transaction information
        this->sender_id=sender_id;
        this->receiver_id=receiver_id;
        this->amount = amount;
        this->txn_id = txn_id;
    }
};

struct Block{
    int miner_id; // miner_id
    int block_id;
    int parent_id;
    double time; //arrival time of the block
    vector<Transaction> transactions;// list if transactions in the block
    vector<double> balances; // stroing all the peers balances
    // max 999 transactions
    int depth;// depth in the blockchain tree
    Block(){
        this->parent_id = -1;// the genesis block
    }
    Block(int miner_id,int block_id,int parent_id,double time,int depth,int noofpeers){// Constructor to store all the information in the block
        this->miner_id=miner_id;
        this->block_id=block_id;
        this->parent_id=parent_id;
        this->time=time;
        this->depth = depth;
        balances = vector<double> (noofpeers,0.0);// initial all peers 0 coins
    }
    void set_balances_to_mining_fee(){
        for(int i=0;i<balances.size();i++){
            balances[i] = (double)50;//intialising the coins with everyone to 50
        }
    }
};

struct TreeNode {// Struct required to store all the forks of the blockchain
    Block block;
    vector<TreeNode*> children;// all forked children
    
    TreeNode(Block block){
        this->block=block;// the parent block intialisation
    }
};

struct Tree {
    TreeNode* root;// the parent tree node 

    Tree() : root(nullptr) {}
    Tree(Block genesis){
        root = new TreeNode(genesis);// the genesis constructor
    }
    bool addBlock(Block block, int parent_id) {// adding a block to the parent block by traversing the tree
        TreeNode* newNode = new TreeNode(block);// creating a new node for the new block
        TreeNode* parentNode = findNode(root, parent_id);// finding where the parent id of the block recieved is
        if (parentNode) {// if found adding this block to that tree node as a child
            newNode->block.depth = parentNode->block.depth + 1;
            parentNode->children.push_back(newNode);
            return true;
        } else {
            return false;
        }
    }
    pair<int,vector<double>> balances_in_parent(int parent_id){// finding balances give its parent id
        TreeNode* parentNode = findNode(root, parent_id);
        // assert(parentNode != nullptr);
        if(parentNode == nullptr) return {0,{}};
        return {1,parentNode->block.balances};
    }

    vector<Block> longestChainHelper(TreeNode* node) {
        vector<Block> longestChain;
        if (node == nullptr) return longestChain; // Base case: empty chain
        longestChain.push_back(node->block); // Add current block to the chain
        vector<Block> maxChain;
        for (TreeNode* child : node->children) {
            vector<Block> childChain = longestChainHelper(child);
            if (childChain.size() > maxChain.size()) {
                maxChain = childChain;
            }
            else if(childChain.size() == maxChain.size() && childChain.size() > 0 && childChain.back().time < maxChain.back().time){
                maxChain = childChain;
            }
        }
        longestChain.insert(longestChain.end(), maxChain.begin(), maxChain.end());
        return longestChain;
    }

    vector<Block> longest_chain(){
        if(root==nullptr){return vector<Block>();}
        return longestChainHelper(root);
    }

    TreeNode* giveNode(int parent_id){
        TreeNode* parentNode = findNode(root,parent_id);
        assert(parentNode != nullptr);
        return parentNode; 
    }

    void printAllNodes(TreeNode* currentNode){
        if (!currentNode) {
            return;
        }
        
        // Print the current node
         cout << "Block ID: " << currentNode->block.block_id <<", Parent ID: "<<currentNode->block.parent_id<< ", Depth: " << currentNode->block.depth << endl;
        
        // Recursively print all child nodes
        for (TreeNode* child : currentNode->children) {
            printAllNodes(child);
        }
    }
    void printAllNodes_file(TreeNode* currentNode, fstream& file){
        if (!currentNode) {
            return;
        }

        // Print the current node
        file << currentNode->block.block_id <<" "<<currentNode->block.parent_id<< " " << currentNode->block.time<<" "<<currentNode->block.miner_id<< endl;
        // Recursively print all child nodes
        for (TreeNode* child : currentNode->children) {
            printAllNodes_file(child, file);
        }
    }
    void printAllNodes(int node_num){
        // Start printing from the root
        printAllNodes(root);
        fstream file;
        string name = "node_"+to_string(node_num)+".txt";
        file.open(name, ios::out);
        printAllNodes_file(root, file);
        file.close();
    }

private:
    TreeNode* findNode(TreeNode* currentNode, int block_id) {// finding the block corresponding to block_id in currentnode
        if (currentNode->block.block_id == block_id) {
            return currentNode;//base case
        }
        for (TreeNode* child : currentNode->children) {
            TreeNode* result = findNode(child, block_id);// recursion
            if (result) {
                return result;
            }
        }
        return nullptr;
    }
};

struct peer
{//storing all the details of the peers
    int ID;
    int noofpeers;
    bool slow;
    bool lowcpu;
    bool is_selfish;
    int mined = 0; 
    double I_value; // denotes I/value 
    vector<Transaction> all_transactions;
    vector<Transaction> transaction_pool;
    vector<Block> selfish_chain;
    vector<Block> all_blocks;
    vector<Block> cached_blocks;// if unfound parents then keep in this
    vector<Block> unadded;
    // assuming < 1e9 unique blocks are created
    Tree BlockChain;// the peers perception of the blockchain
    vector<Block> longest_chain;// main chain currently
    peer(){}
    peer(int ID,bool lowcpu,bool fast,bool selfish)//constructor to store all information of the peer
    {
        this->ID=ID;
        this->lowcpu=lowcpu;
        this->slow = !fast;
        this->is_selfish = selfish;
        cout<<ID<<" "<<" "<<slow<<" "<<lowcpu<<endl;
    }
    void initiate_tree(Block gen){// intially all have only genesis block
        BlockChain = Tree(gen);
    }
    bool find_in_all_transactions(Transaction t){//checking if a transaction t is of this peer 
        for(auto x : all_transactions){
            if(x.txn_id == t.txn_id){return true;}
        }
        return false;
    }
    vector<Block> get_longest_chain(){
        return BlockChain.longest_chain();// Current longest chain
    }
    bool find_in_tree(int block_id){//finding the block in the blockchain tree
        for(int i=0;i<all_blocks.size();i++){
            if(all_blocks[i].block_id == block_id){
                return true;
            }
        }
        return false;
    }
    int add_block(Block b){//Adding block to the tree and updating the longest chain and updating transaction pool
        longest_chain = get_longest_chain();
        int increased_chain_length = 1;
        vector<Block> prev_longest_chain = longest_chain;// current longest chain before adding block
        all_blocks.push_back(b);// add this block to all blocks the peer recieved
        if(b.parent_id == -1){
            b.parent_id = longest_chain.back().block_id;// adding this to genesis block
        }
        if(BlockChain.addBlock(b,b.parent_id)){ //checking if blocks parent is in blockchain
            vector<Block> temp_blocks;// adding all cached blocks whose parent_id is the id of new block
            for(int i=0;i<cached_blocks.size();i++){
                if(cached_blocks[i].parent_id == b.block_id){
                    increased_chain_length = 2;
                    BlockChain.addBlock(cached_blocks[i],b.block_id);// adding the children of new block
                }
                else{
                    temp_blocks.push_back(cached_blocks[i]);// if not child and no parent saving to temp_cache
                }
            }
            cached_blocks = temp_blocks;// updating cached_blocks
        }
        else{
            cout<<"Cached so returning"<<endl;
            cached_blocks.push_back(b);// if block parent isn't found cached the block
            return 0;
        }
        longest_chain = get_longest_chain();// Post the recursive adding of new childrens of recieved block longest chain 
        if(!same_chain(prev_longest_chain)){// checing if the longest chain is same else updating transaction
            // update transaction pool 
            vector<Transaction> updated_transaction_pool;// new pool
            vector<Transaction> transaction_in_new_chain;// new long chain transaction
            for(auto b : longest_chain){
                for(auto t : b.transactions){
                    transaction_in_new_chain.push_back(t);// adding all transaction in the longest chain
                }
            }
            for(auto t : all_transactions){
                bool found = false;
                for(auto x : transaction_in_new_chain){// checking the long chain's pool if its available
                    if(x.txn_id == t.txn_id){
                        found = true;break;
                    }
                }
                if(!found){updated_transaction_pool.push_back(t);}// if not found in the new chain add it to new pool 
            }
            transaction_pool = updated_transaction_pool;// updating the transaction pool
            return increased_chain_length;
        }
        else{
            return 0;// same longest chain
        }
        // need to add block and return true
        // get the block by prev block id and add it 
        // if block creates a new longest chain
        //      also update transaction pool
    }
    bool same_chain(vector<Block>& chain){// checking if both the chains are same or different
        longest_chain = get_longest_chain();
        if(chain.size() != longest_chain.size()){return false;}
        for(int i=0; i<chain.size(); i++){
            if(longest_chain[i].block_id != chain[i].block_id){
                return false;
            }
        }
        return true;
    }
    Block create_block(int block_id,int txn_id,double time){
        // complete this function
        // prev blk id should be that of last block in the longest chain
        // add the created block to the block list
        // increase block depth
        longest_chain = get_longest_chain();
        Block b(ID,block_id,longest_chain.back().block_id,time,longest_chain.back().depth+1,noofpeers);
        // insert only valid transactions
        b.balances = longest_chain.back().balances;// getting the balances from the prev longest chain
        Transaction coin_base(txn_id,-1,ID,miningfee);// adding its own coin_base transaction in this block
        b.transactions.push_back(coin_base);
        b.balances[ID] += miningfee;// increasing if this block gets added
        all_transactions.push_back(coin_base);// adding to the pool
        int size = min(999,(int)transaction_pool.size());// limiting the transaction pool
        vector<Transaction> new_transaction_pool;// all remaining transactions not added
        vector<int> failed;// which give negative balances if added
        for(int i=0;i<size;i++){
            Transaction t = transaction_pool[i];
            if(t.sender_id==-1){// coinbase transaction
                b.balances[t.receiver_id] += miningfee;
                b.transactions.push_back(t);
            }
            else{
                if(b.balances[t.sender_id] >= t.amount){//checking if transaction is possible or not
                    b.balances[t.sender_id] -= t.amount;
                    b.balances[t.receiver_id] += t.amount;
                    b.transactions.push_back(t);// updating balances and transactions
                }
                else{
                    failed.push_back(i);// couldn't add now due to low balance
                }
            }
        }
        for(auto i:failed){
            new_transaction_pool.push_back(transaction_pool[i]);
        }
        for(int i=size;i<(int)transaction_pool.size();i++){
            new_transaction_pool.push_back(transaction_pool[i]);
        }
        transaction_pool = new_transaction_pool;//all failed or size limit failed transactions added
        // add transactions into block
        return b;
    }

    Block create_block(int block_id,int txn_id,double time,Block parent){
        longest_chain = get_longest_chain();
        // TreeNode* p = BlockChain.giveNode(parent_id);
        // Block back = p->block;
        Block b(ID,block_id,parent.block_id,time,parent.depth+1,noofpeers);
        // insert only valid transactions
        b.balances = parent.balances;// getting the balances from the prev longest chain
        Transaction coin_base(txn_id,-1,ID,miningfee);// adding its own coin_base transaction in this block
        b.transactions.push_back(coin_base);
        b.balances[ID] += miningfee;// increasing if this block gets added
        all_transactions.push_back(coin_base);// adding to the pool
        int size = min(999,(int)transaction_pool.size());// limiting the transaction pool
        vector<Transaction> new_transaction_pool;// all remaining transactions not added
        vector<int> failed;// which give negative balances if added
        for(int i=0;i<size;i++){
            Transaction t = transaction_pool[i];
            if(t.sender_id==-1){// coinbase transaction
                b.balances[t.receiver_id] += miningfee;
                b.transactions.push_back(t);
            }
            else{
                if(b.balances[t.sender_id] >= t.amount){//checking if transaction is possible or not
                    b.balances[t.sender_id] -= t.amount;
                    b.balances[t.receiver_id] += t.amount;
                    b.transactions.push_back(t);// updating balances and transactions
                }
                else{
                    failed.push_back(i);// couldn't add now due to low balance
                }
            }
        }
        for(auto i:failed){
            new_transaction_pool.push_back(transaction_pool[i]);
        }
        for(int i=size;i<(int)transaction_pool.size();i++){
            new_transaction_pool.push_back(transaction_pool[i]);
        }
        transaction_pool = new_transaction_pool;//all failed or size limit failed transactions added
        // add transactions into block
        return b;
    }

    bool validate_block(Block b){
        vector<double> prev_balances;// checking from prev balances if a block is valid
        int a;
        a = BlockChain.balances_in_parent(b.parent_id).first;// if block parent available or not
        if(a==0){return false;}
        prev_balances = BlockChain.balances_in_parent(b.parent_id).second;// its balances list is sent
        for(auto t:b.transactions){// checking all peers transaction amounts positive or not 
            if(t.sender_id == -1){prev_balances[t.receiver_id] += miningfee;continue;}
            if(prev_balances[t.sender_id] < t.amount){    
            return false;}
            prev_balances[t.sender_id] -= t.amount;
            prev_balances[t.receiver_id] += t.amount;// updating peer balances
        }
        return true;
    }
};
