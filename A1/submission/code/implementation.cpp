#include <bits/stdc++.h>
#include <random>
using namespace std; 
#include "peer.h"
#include "graph.h"
#include <chrono>

int txn_id = 0;// intialising the ids
int blk_id = 0;// intialising the ids
vector<peer> peers;// list of peers
graph g;// new graph
double I; // average interarrival time between blocks
auto start_time = chrono::high_resolution_clock::now(); 
double gettime(){// time from start
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    return duration.count()/1000000.0;
}

struct EventObject{ //block or transaction event stored 
    int peer_id;
    int senders_id;
    double time;
    string type;
    Transaction txn;
    Block blk;
    vector<Block> longest_chain;
    EventObject(){}
    EventObject(double tim,int id,string event_type,int sender_id){// intialisation based on peer
        peer_id = id;
        type = event_type;
        time = tim;
        senders_id = sender_id;
    };
    void fill_txn(Transaction t){// if event is a transaction
        txn = t;
    }
    void fill_blk(Block b){// if event is a block
        blk = b;
    }
    void fill_list_of_blocks(vector<Block> v){//sending list of blocks
        longest_chain = v;
    }
};

class MyPriorityQueue {
private:
    // Define a lambda function to compare pairs based on the first element
    struct Comparator {
        bool operator()(const std::pair<double, EventObject>& left, const std::pair<double, EventObject>& right) const {
            return left.first > right.first;  // Change the comparison based on your requirements
        }
    };

    // Priority queue with the custom comparator
    std::priority_queue<std::pair<double, EventObject>, std::vector<std::pair<double, EventObject>>, Comparator> pq;

public:
    void push(const std::pair<double, EventObject>& element) {
        pq.push(element);
    }

    void pop() {
        pq.pop();
    }

    bool empty() const {
        return pq.empty();
    }

    pair<double, EventObject> top() const {
        return pq.top();
    }
};

MyPriorityQueue event_queue;//Initialisation a event queue common to network

void handle_event(double rho,int noofpeers,double lambda,int epochs){
    uniform_int_distribution<> peer_dist(0,noofpeers-2);// getting the peer at random to do a event
    random_device rd;
    mt19937 gen(rd());// seeding using hardware

    while((!event_queue.empty()) && (epochs--)){//handling until eventlist empty or epochs run out
        // handle an event
        pair<double, EventObject> element = event_queue.top(); // top to do the event
        if(element.second.type == "send_txn"){//if its to send a transaction
            int sender_id = element.second.peer_id;// get sender id
            int recv_id = peer_dist(gen);// generate reciever at random
            if(recv_id >= sender_id){recv_id++;}// making recv_id range everything except sender_id
            txn_id++;// new transactions new id
            uniform_real_distribution<> txn_amount_dist(0,10); // between 0 to 10
            double txn_amount = txn_amount_dist(gen);// getting money to be sent from a txn-amount_dist
            Transaction t(txn_id,sender_id,recv_id,txn_amount);// creating a transaction
            for(auto adj : g.edges[sender_id]){// sending to all its peers
                double Cij = (peers[sender_id].slow||peers[adj].slow)?5.0:100.0;// if its slow or fast speed found
                exponential_distribution<> dij_dist(Cij/0.096);// queing delay distribution
                double Dij = dij_dist(gen);// generating delay time
                double receive_time = element.first+(1.0/(1000.0*Cij))+ Dij+rho;// time taken to recieve it
                EventObject a(receive_time,adj,"recv_txn",sender_id);// once sent it recieves post recieve time
                a.fill_txn(t);// add txn to event
                event_queue.push({receive_time,a});// add to queue
            }
            peers[sender_id].all_transactions.push_back(t);// adding peers transaction
            peers[sender_id].transaction_pool.push_back(t);// adding into pool of peer transaction
            exponential_distribution<> next_txn_dist(1/lambda);// next event time delay distribution
            double next_txn_time = element.first+next_txn_dist(gen);// next transaction event time
            EventObject a(next_txn_time,sender_id,"send_txn",sender_id);
            cout<<"sent_txn from sender_id "<< sender_id<<" with id "<<txn_id<<endl;
            event_queue.push({next_txn_time,a});// creating and adding new transaction as event
            event_queue.pop();
        }
        else if(element.second.type == "recv_txn"){
            // need to validate the transaction
            // if already in txn pool discard the txn
            if(!peers[element.second.peer_id].find_in_all_transactions(element.second.txn)){// if already in pool dont send it again
                cout<<"recv_txn from sender_id "<< element.second.senders_id<<" at recv_id "<<element.second.peer_id<<" with id "<<element.second.txn.txn_id<<endl;
                peers[element.second.peer_id].all_transactions.push_back(element.second.txn);// add transaction to peers list
                peers[element.second.peer_id].transaction_pool.push_back(element.second.txn);// add transaction to peers list
                for(auto adj : g.edges[element.second.peer_id]){// sending to all its peers
                    if(adj != element.second.senders_id){
                        double Cij;
                        Cij = (peers[element.second.peer_id].slow||peers[adj].slow)?5.0:100.0;// if slow or fast
                        exponential_distribution<> dij_dist(Cij/0.096);// queue delay
                        double Dij = dij_dist(gen);
                        double receive_time = element.first +(1.0/(1000.0*Cij))+ Dij+rho;// time for this event to be done
                        EventObject a(receive_time,adj,"recv_txn",element.second.peer_id);
                        a.fill_txn(element.second.txn);
                        event_queue.push({receive_time,a});// created and added event to queue
                    }
                }
            }
            event_queue.pop();// remove the top as its done
        }
        else if(element.second.type == "check_blk"){
            // if longest_chain now is same as the longest chain in the event object,
            // need to mine a new block and send it to peers
            // almost same as below one but be careful with miner_id
            if(peers[element.second.peer_id].same_chain(element.second.longest_chain)){//if same long chain then peer can mine a new block and send to peers
                blk_id++;
                txn_id++;
                Block b = peers[element.second.peer_id].create_block(blk_id,txn_id,element.first);
                if(peers[element.second.peer_id].add_block(b)){
                    exponential_distribution<> Tk_dist(peers[element.second.peer_id].I_value);
                    double Tk = Tk_dist(gen);
                    EventObject a(element.first+Tk,element.second.peer_id,"check_blk",element.second.peer_id);
                    vector<Block> curr_longest_chain = peers[element.second.peer_id].get_longest_chain(); 
                    a.fill_list_of_blocks(curr_longest_chain);
                    event_queue.push({element.first+Tk,a});
                }
                for(auto adj : g.edges[element.second.peer_id]){// sending this block to  others with all network delay and queue delay randomisation
                    double Cij = (peers[element.second.peer_id].slow||peers[adj].slow)?5.0:100.0;
                    exponential_distribution<> dij_dist(Cij/0.096);
                    double Dij = dij_dist(gen);
                    double receive_time = element.first+((double)b.transactions.size()/(1000.0*Cij))+ Dij+rho;
                    EventObject a(receive_time,adj,"recv_blk",element.second.peer_id);
                    Block dum_block = b;
                    dum_block.time = receive_time;
                    a.fill_blk(dum_block);
                    event_queue.push({receive_time,a});
                }
                cout<<endl;
                cout<<"sent block from sender_id "<<element.second.peer_id<<" block_id "<<element.second.blk.block_id<<endl;
                cout<<endl;
            }
            event_queue.pop();
        }
        else if(element.second.type == "send_blk"){
            blk_id++;
            txn_id++;
            Block b = peers[element.second.peer_id].create_block(blk_id,txn_id,element.first);// create a new block on longest chain
            cout<<"Created block"<<endl;
            if(peers[element.second.peer_id].add_block(b)){//Added the newly created block to the longest chain using appropriate delays
                cout<<"Added block increased length of chain"<<endl;
                exponential_distribution<> Tk_dist(peers[element.second.peer_id].I_value);
                double Tk = Tk_dist(gen);
                EventObject a(element.first+Tk,element.second.peer_id,"check_blk",element.second.peer_id);//Creating a check block event
                vector<Block> curr_longest_chain = peers[element.second.peer_id].get_longest_chain(); 
                a.fill_list_of_blocks(curr_longest_chain);
                event_queue.push({element.first+Tk,a});
            }
            for(auto adj : g.edges[element.second.peer_id]){// sending the block to all its peers using appropriate delays
                double Cij = (peers[element.second.peer_id].slow||peers[adj].slow)?5.0:100.0;
                exponential_distribution<> dij_dist(Cij/0.096);
                double Dij = dij_dist(gen);
                double receive_time = element.first+((double)b.transactions.size()/(1000.0*Cij))+ Dij+rho;
                EventObject a(receive_time,adj,"recv_blk",element.second.peer_id);// all its connected peers recieve block
                Block dum_block = b;
                dum_block.time = receive_time;
                a.fill_blk(dum_block);// fill the event with this block
                event_queue.push({receive_time,a});// event queue updated
            }
            cout<<endl;
            cout<<"sent block from sender_id "<<element.second.peer_id<<" block_id "<<blk_id<<endl;
            cout<<endl;
            event_queue.pop();
        }
        else if(element.second.type == "recv_blk"){
            // store the current longest chain in the event object
            // get longest chain in the tree
            // add an event check_blk at tk + Tk 
            // need to validate all the transactions present in the block received
            if(!peers[element.second.peer_id].find_in_tree(element.second.blk.block_id)){//already this block received so not adding into tree
                if(peers[element.second.peer_id].find_in_tree(element.second.blk.parent_id) && !(peers[element.second.peer_id].validate_block(element.second.blk))){
                    cout<<"Invalid block,not sending it to neighbouring peers"<<endl;
                }
                else{
                    for(auto adj : g.edges[element.second.peer_id]){// sending block to all its peers according to its connections and recpective delays
                        if(adj != element.second.senders_id){
                            double Cij;
                            Cij = (peers[element.second.peer_id].slow||peers[adj].slow)?5.0:100.0;
                            exponential_distribution<> dij_dist(Cij/0.096);
                            double Dij = dij_dist(gen);
                            double receive_time = element.first +((double)element.second.blk.transactions.size()/(1000.0*Cij))+ Dij+rho;
                            EventObject a(receive_time,adj,"recv_blk",element.second.peer_id);// forwarding block by adding network and other delays
                            Block dum_block = element.second.blk;
                            dum_block.time = receive_time;
                            a.fill_blk(dum_block);// adding block to event
                            event_queue.push({receive_time,a});// event added
                        }
                    }
                    if(peers[element.second.peer_id].add_block(element.second.blk)){//Added the newly received block to the longest chain using appropriate delays
                        exponential_distribution<> Tk_dist(peers[element.second.peer_id].I_value);
                        double Tk = Tk_dist(gen);
                        EventObject a(element.first+Tk,element.second.peer_id,"check_blk",element.second.peer_id);
                        vector<Block> curr_longest_chain = peers[element.second.peer_id].get_longest_chain(); 
                        a.fill_list_of_blocks(curr_longest_chain);
                        event_queue.push({element.first+Tk,a});
                    }
                    cout<<endl;
                    cout<<"recv block at receiver_id "<< element.second.peer_id << " block id "<<element.second.blk.block_id<<endl;
                    cout<<endl;
                }
            }
            event_queue.pop();
        }
    }
}

int main(int argc,char *argv[]) //  runnning is ./a.out noofpeers slowpercent lowcpupercent (< 100%) lambda
{
    random_device rd;
    mt19937 gen(rd());
    int noofpeers=atoi(argv[1]);// Number of peers
    double z0=atof(argv[2]); // Percentage of slow link speed
    double z1=atof(argv[3]);// Percentage of low cpu hash power
    int slow=noofpeers*z0/100;  
    int fast=noofpeers-slow;
    int lowcpu=noofpeers*z1/100;
    int highcpu=noofpeers-lowcpu;
    double lambda=atof(argv[4]);//intertransaction time
    I = atof(argv[5]);// Interblock arrival time
    int no_of_events = 10000;
    exponential_distribution<> txn_distribution(1/lambda);// Transaction time distribution

    peers.resize(noofpeers);//Peers initialised
    vector<bool> cpupower(noofpeers,0);
    vector<bool> fast_link(noofpeers,0);
    for(int i=0;i<fast;i++){fast_link[i]=true;}
    for(int i=0;i<highcpu;i++){cpupower[i]=true;}
    random_shuffle(cpupower.begin(), cpupower.end());//randomising which peer has high cpu power 
    random_shuffle(fast_link.begin(), fast_link.end());//randomising which peer has fast link sped
    
    for(int i=0;i<noofpeers;i++){// updating peer attributes
        peers[i]=peer(i,cpupower[i],fast_link[i]);
        peers[i].noofpeers = noofpeers;
    }

    uniform_real_distribution<double> uni_dist(10.0,500.0);
    double rho = uni_dist(gen); // parameter P ij
    Block genesis(-1,0,-1,0,0,noofpeers);
    genesis.set_balances_to_mining_fee();

    for(int i=0;i<noofpeers;i++){// generate send_transaction and send_block event for each peer
        double txn_time = txn_distribution(gen);// transaction time
        EventObject a(txn_time,i,"send_txn",i);// event corresponding to txn
        event_queue.push({txn_time,a});// queue updated
        double value = 1.0/(double)(9*highcpu+noofpeers);
        if(cpupower[i]){value *= 10.0;}
        exponential_distribution<> blk_distribution(value/I);// block generation based on hash power
        peers[i].I_value = value/I;
        double blk_time = blk_distribution(gen);// time to create new block
        peers[i].initiate_tree(genesis);// add genesis block to the tree
        EventObject b(blk_time,i,"send_blk",i);//block gen event added
        event_queue.push({blk_time,b});    
    }
    g = graph(noofpeers);// connected graph generation
    handle_event(rho/1000.0,noofpeers,lambda,no_of_events);//handling all events added and generating accordingly
    for(int i=0;i<noofpeers;i++){// printing Trees of all peers
        cout<<"Node   "<<i<<endl;
        peers[i].BlockChain.printAllNodes(i);
        cout<<endl;
        cout<<"-----------------\n"<<endl;
    }
    int x = 0;
    cout<< "Final Balances :"<<endl;
    for(auto i: peers[0].longest_chain.back().balances){//final balances of all peers
        cout<<"Peer "<<x<<" has "<<i<<endl;
        x++;
    }
}
