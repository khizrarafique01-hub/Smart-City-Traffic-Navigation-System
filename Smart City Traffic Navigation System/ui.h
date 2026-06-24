#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "graph.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

struct nodepos { float x, y; };

struct inputbox {
    Rectangle rect;
    string text;
    bool active;
    string placeholder;
};

extern graph g;
extern Font fontbold, fontreg;
extern vector<string> pathresult;
extern int pathdist;
extern string patherror;
extern bool showpopup;
extern string popupmsg;
extern int cityscrolloffset;
extern inputbox inpsource, inpdest;
extern inputbox inpaddcity1, inpaddcity2, inpaddweight;
extern unordered_map<string, nodepos> nodepositions;
extern vector<pair<string, nodepos>> defaultpositions;
extern float panoffsetx;
extern float panoffsety;
extern bool ispanning;
extern Vector2 panstart;

string normalizename(string s);
nodepos getpos(const string &name);
nodepos autoplace(int index);
Vector2 labelpos(nodepos p, const string &name, Font font, float fontsize);
void drawroundrect(Rectangle r, float rnd, Color c);
void drawroundrectborder(Rectangle r, float rnd, float thick, Color c);
void drawtextcentered(Font f, const char* txt, Rectangle r, float size, Color c);
bool mouseover(Rectangle r);
bool drawinputbox(inputbox &box, Font font, float fontsize);
void handleinput(inputbox &box);
void deactivateall();
void drawgrid();
set<pair<string,string>> buildhighlightedges();
void drawgraph();
void drawpanel();
void drawpopup();

#endif