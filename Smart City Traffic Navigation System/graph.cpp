#include "graph.h"
#include <fstream>
#include <climits>
#include <queue>
#include <algorithm>

void graph::addnode(string name) {
    for(auto &c : name) c = toupper(c);
    if(nodeindex.find(name) == nodeindex.end()) {
        nodeindex[name] = nodecount;
        adjlist.push_back({});
        nodecount++;
    }
}

void graph::addedge(string n1, string n2, int weight) {
    for(auto &c : n1) c = toupper(c);
    for(auto &c : n2) c = toupper(c);

    // duplicate edge check
    int i = nodeindex[n1];
    for(auto &nb : adjlist[i]) {
        if(nb.first == n2) return;
    }

    int j = nodeindex[n2];
    adjlist[i].push_back({n2, weight});
    adjlist[j].push_back({n1, weight});
    edgecount++;
}

void graph::loadfromfile(string file) {
    ifstream fin(file);
    if(!fin) {
        cout << "File not found!" << endl;
        return;
    }
    int n;
    fin >> n;
    fin.ignore();
    for(int i = 0; i < n; i++) {
        string line;
        getline(fin, line);
        if(line.empty()) { i--; continue; }

        // parse from right: last token is weight
        int lastspace = line.rfind(' ');
        int weight = stoi(line.substr(lastspace + 1));
        string rest = line.substr(0, lastspace);

        // second last token is n2
        int secondlastspace = rest.rfind(' ');
        string n2 = rest.substr(secondlastspace + 1);
        string n1 = rest.substr(0, secondlastspace);

        for(auto &c : n1) c = toupper(c);
        for(auto &c : n2) c = toupper(c);

        addnode(n1);
        addnode(n2);
        addedge(n1, n2, weight);
    }
    fin.close();
}

void graph::displaymap() {
    cout << "\n---------City Map--------" << endl;
    for(auto node : nodeindex) {
        cout << node.first << " -> ";
        for(auto neighbour : adjlist[node.second]) {
            cout << neighbour.first << "(" << neighbour.second << ") ";
        }
        cout << endl;
    }
}

vector<string> graph::dijkstra(string startnode, string endnode, int &totaldistance) {
    int totalnodes = nodecount;
    vector<int> dist(totalnodes, INT_MAX);
    vector<int> parent(totalnodes, -1);
    vector<bool> visited(totalnodes, false);

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    if(nodeindex.find(startnode) == nodeindex.end() ||
       nodeindex.find(endnode)   == nodeindex.end()) {
        totaldistance = -1;
        return {};
    }

    int src = nodeindex[startnode];
    int dst = nodeindex[endnode];

    dist[src] = 0;
    pq.push({0, src});

    while(!pq.empty()) {
        int currentdist = pq.top().first;
        int currentnode = pq.top().second;
        pq.pop();

        if(visited[currentnode]) continue;
        visited[currentnode] = true;

        if(currentnode == dst) break;

        for(auto neighbour : adjlist[currentnode]) {
            int nextnode   = nodeindex[neighbour.first];
            int edgeweight = neighbour.second;
            int newdist    = currentdist + edgeweight;

            if(newdist < dist[nextnode]) {
                dist[nextnode]   = newdist;
                parent[nextnode] = currentnode;
                pq.push({newdist, nextnode});
            }
        }
    }

    if(dist[dst] == INT_MAX) {
        totaldistance = -1;
        return {};
    }

    totaldistance = dist[dst];

    unordered_map<int,string> indextoname;
    for(auto node : nodeindex) {
        indextoname[node.second] = node.first;
    }

    vector<string> path;
    int curr = dst;
    while(curr != -1) {
        path.push_back(indextoname[curr]);
        curr = parent[curr];
    }

    reverse(path.begin(), path.end());
    return path;
}

void graph::savefile(string filename) {
    ofstream fout(filename);
    set<pair<string,string>> written;
    int count = 0;

    for(auto node : nodeindex) {
        for(auto neighbour : adjlist[node.second]) {
            if(written.find({neighbour.first, node.first}) == written.end()) {
                written.insert({node.first, neighbour.first});
                count++;
            }
        }
    }

    fout << count << "\n";
    written.clear();

    for(auto node : nodeindex) {
        for(auto neighbour : adjlist[node.second]) {
            if(written.find({neighbour.first, node.first}) == written.end()) {
                written.insert({node.first, neighbour.first});
                fout << node.first << " "
                     << neighbour.first << " "
                     << neighbour.second << "\n";
            }
        }
    }

    fout.close();
}