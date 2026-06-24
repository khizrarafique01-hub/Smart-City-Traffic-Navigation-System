#ifndef graph_H
#define graph_H
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set> 
using namespace std;

class graph {
private:
    int nodecount;
    int edgecount;

public:
    vector<vector<pair<string,int>>> adjlist;
    unordered_map<string,int> nodeindex;

    graph() {
        nodecount = 0;
        edgecount = 0;
    }

    void addnode(string name);
    void addedge(string node1, string node2, int weight);
    void loadfromfile(string filename);
    void displaymap();
    vector<string> dijkstra(string startnode, string endnode,int &totaldistance);
    void savefile(string filename);
    int getnodecount() { 
        return nodecount;
     }
};
#endif