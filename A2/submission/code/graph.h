#include <bits/stdc++.h>
#include <random>
using namespace std;

struct graph
{
    int no_of_vertices;// Number of peers
    vector<set<int>> edges;// Connection between peers
    
    graph(){}
    graph(int vertices){
        no_of_vertices = vertices;
        while(true){//Creating graph with randomised conenctions of 3-6 for each peer until its fully connected
            create_graph();   
            if(check_connected()){break;}
        }
        print_graph();//Printing all Connections
    }

    void create_graph(){
        random_device rd;
        mt19937 gen(rd()); //seeding for random number generation 
        edges = vector<set<int>> (no_of_vertices);// intialising the Connections of each peer
        set<int> valid; // all valid connections
        for(int i=0; i<no_of_vertices; i++){valid.insert(i);}// inserting all vertices as all require connections
        uniform_int_distribution<int> distribution(3,6);// random number generator for number of edges
        for(int i=0; i<no_of_vertices; i++){
            if(valid.count(i)==0){continue;}// its already found its connections in 3-6
            int x = distribution(gen); // getting number of connections from distribution
            if(x <= edges[i].size()){continue;}//it already has more than that number of connections
            vector<int> validset;// Connecting peers in this to i
            for(auto node:valid){
                if(edges[i].count(node)==0 && node != i){ // if not already connected or itself can connect to it
                    validset.push_back(node);
                }
            }
            shuffle(validset.begin(), validset.end(),gen);// shufling connection possibilities for randomisation
            int num_to_be_filled = x-edges[i].size();//number of connections to be added
            if(num_to_be_filled <=0 ){continue;}
            for(int j=0;j<min(num_to_be_filled,(int)validset.size());j++){// connecting the peers as per the randomisation
                edges[i].insert(validset[j]);
                edges[validset[j]].insert(i);
            }
            for(int j=0;j<no_of_vertices;j++){
                if(edges[j].size()>=6){valid.erase(j);}// if already breached the upper limit remove from valid
            }
            if(valid.size()==0){return;}// if all connected its done
        }
    }

    void dfs(int curr_vertex,vector<int> &visited,int &count){// Regular dfs to check and traverse all possible peers from current_vertex
        count++;// number of peers connected
        visited[curr_vertex]=1;// updating visited vertices
        for(auto adjacent_vertex : edges[curr_vertex]){// traversing its edges
            if(!visited[adjacent_vertex]){
                dfs(adjacent_vertex,visited,count);//recursive dfs
            }
        }
    }

    bool check_connected(){
        vector<int> visited(no_of_vertices,0);// visited or not vector
        int count = 0; // to store number of peers connected
        dfs(0,visited,count);
        return (count == no_of_vertices);// if all peers are connected 
    }

    void print_graph(){// printing all edges of each vertex
        for(int i=0;i<no_of_vertices;i++){
            cout<<"Vertex "<<i<<" : ";
            for(auto adj_to_i:edges[i]){
                cout<<adj_to_i<<" ";
            }
            cout<<endl;
        }
    }
};
