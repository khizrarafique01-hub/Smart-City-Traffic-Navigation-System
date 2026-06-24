#include "ui.h"
 
Color BG_DARK        = {10,  15,  35,  255};
Color PANEL_BG       = {15,  22,  50,  255};
Color PANEL_BORDER   = {40,  80, 160,  255};
Color NODE_CLR       = {50, 120, 220,  255};
Color NODE_BORDER    = {100,180, 255,  255};
Color EDGE_CLR       = {80, 120, 180,  120};
Color EDGE_LABEL     = {180,210, 255,  255};
Color HIGHLIGHT_EDGE = {255,210,  50,  255};
Color SRC_CLR        = {50,  220, 120, 255};
Color DST_CLR        = {220,  80,  80, 255};
Color PATH_NODE_CLR  = {255, 180,  30, 255};
Color TITLE_CLR      = {120, 190, 255, 255};
Color SUBTITLE_CLR   = {80,  140, 220, 255};
Color TEXT_CLR       = {200, 220, 255, 255};
Color DIM_CLR        = {100, 130, 180, 255};
Color INPUT_BG       = {20,  30,  65,  255};
Color INPUT_ACTIVE   = {30,  50, 100,  255};
Color INPUT_BORDER   = {60, 100, 180,  255};
Color BTN_FIND       = {30, 100, 220,  255};
Color BTN_ADD        = {30, 140,  80,  255};
Color BTN_HOVER      = {50, 140, 255,  255};
Color BTN_DELETE     = {180, 40,  40,  255};
Color DIVIDER        = {40,  70, 130,  255};
Color POPUP_BG       = {10,  20,  50,  230};
Color POPUP_OK       = {30, 120, 200,  255};
Color GRID_CLR       = {20,  30,  60,  255};
 
const int SCREEN_W    = 1400;
const int SCREEN_H    = 820;
const int PANEL_X     = 980;
const int PANEL_W     = 420;
const int NODE_RADIUS = 18;
 
float panoffsetx = 0.0f;
float panoffsety = 0.0f;
bool ispanning = false;
Vector2 panstart = {0, 0};
 
vector<pair<string, nodepos>> defaultpositions = {
    {"LAHORE",      {490, 90}},
    {"GUJRANWALA",  {650, 145}},
    {"SIALKOT",     {280, 150}},
    {"RAWALPINDI",  {130, 255}},
    {"ISLAMABAD",   {750, 255}},
    {"FAISALABAD",  {820, 385}},
    {"MULTAN",      {95,  390}},
    {"PESHAWAR",    {155, 535}},
    {"ABBOTTABAD",  {740, 535}},
    {"QUETTA",      {615, 650}},
    {"HYDERABAD",   {250, 665}},
    {"KARACHI",     {435, 690}},
};
 
unordered_map<string, nodepos> nodepositions;
graph g;
Font fontbold, fontreg;
vector<string> pathresult;
int pathdist = 0;
string patherror = "";
bool showpopup = false;
string popupmsg = "";
int cityscrolloffset = 0;
inputbox inpsource, inpdest;
inputbox inpaddcity1, inpaddcity2, inpaddweight;
 
string normalizename(string s) {
    if(s.empty()) return s;
    for(auto &c : s) c = toupper(c);
    return s;
}
 
nodepos getpos(const string &name) {
    auto it = nodepositions.find(name);
    if(it != nodepositions.end())
        return {it->second.x + panoffsetx, it->second.y + panoffsety};
    return {490 + panoffsetx, 400 + panoffsety};
}
 
nodepos autoplace(int index) {
    float cx = 490, cy = 390;
    float angle = index * 2.8f;
    float radius = 200 + index * 45.0f;
    if(radius > 420) radius = 420;
    return { cx + radius * cosf(angle), cy + radius * sinf(angle) };
}
 
Vector2 labelpos(nodepos p, const string &name, Font font, float fontsize) {
    float cx = 490 + panoffsetx, cy = 390 + panoffsety;
    float tw = (float)MeasureTextEx(font, name.c_str(), fontsize, 1).x;
    float th = fontsize;
    float offset = (float)NODE_RADIUS + 6;
    bool rightside = p.x >= cx;
    bool topside   = p.y <= cy;
    float dx = fabsf(p.x - cx);
    float dy = fabsf(p.y - cy);
    if(dy > dx * 1.5f) {
        if(topside) return { p.x - tw/2, p.y - NODE_RADIUS - th - 4 };
        else        return { p.x - tw/2, p.y + NODE_RADIUS + 4 };
    } else {
        if(rightside) return { p.x + offset, p.y - th/2 };
        else          return { p.x - offset - tw, p.y - th/2 };
    }
}
 
void drawroundrect(Rectangle r, float rnd, Color c) {
    DrawRectangleRounded(r, rnd, 8, c);
}
 
void drawroundrectborder(Rectangle r, float rnd, float thick, Color c) {
    DrawRectangleRoundedLinesEx(r, rnd, 8, thick, c);
}
 
void drawtextcentered(Font f, const char* txt, Rectangle r, float size, Color c) {
    Vector2 tv = MeasureTextEx(f, txt, size, 0);
    float tx = r.x + (r.width  - tv.x) / 2.0f;
    float ty = r.y + (r.height - tv.y) / 2.0f;
    DrawTextEx(f, txt, {tx, ty}, size, 0, c);
}
 
bool mouseover(Rectangle r) {
    Vector2 mp = GetMousePosition();
    return CheckCollisionPointRec(mp, r);
}
 
bool drawinputbox(inputbox &box, Font font, float fontsize) {
    bool clicked = false;
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseover(box.rect))
        clicked = true;
    Color bg  = box.active ? INPUT_ACTIVE : INPUT_BG;
    Color brd = box.active ? TITLE_CLR    : INPUT_BORDER;
    drawroundrect(box.rect, 0.3f, bg);
    drawroundrectborder(box.rect, 0.3f, box.active ? 2.0f : 1.0f, brd);
    const string &display = box.text.empty() ? box.placeholder : box.text;
    Color tc = box.text.empty() ? DIM_CLR : TEXT_CLR;
    DrawTextEx(font, display.c_str(),
        {box.rect.x + 10, box.rect.y + (box.rect.height - fontsize)/2},
        fontsize, 1, tc);
    if(box.active && ((int)(GetTime()*2) % 2 == 0) && !box.text.empty()) {
        float cw = MeasureTextEx(font, box.text.c_str(), fontsize, 1).x;
        DrawRectangle((int)(box.rect.x + 10 + cw + 2),
                      (int)(box.rect.y + 6), 2,
                      (int)(box.rect.height - 12), TEXT_CLR);
    }
    return clicked;
}
 
void handleinput(inputbox &box) {
    if(!box.active) return;
    int key = GetCharPressed();
    while(key > 0) {
        if(key >= 32 && key <= 125 && box.text.size() < 30)
            box.text += (char)key;
        key = GetCharPressed();
    }
    if(IsKeyPressed(KEY_BACKSPACE) && !box.text.empty())
        box.text.pop_back();
}
 
void deactivateall() {
    inpsource.active = inpdest.active = false;
    inpaddcity1.active = inpaddcity2.active = inpaddweight.active = false;
}
 
void drawgrid() {
    float ox = fmod(panoffsetx, 40.0f);
    float oy = fmod(panoffsety, 40.0f);
    for(float x = ox; x < PANEL_X; x += 40)
        DrawLine((int)x, 0, (int)x, SCREEN_H, GRID_CLR);
    for(float y = oy; y < SCREEN_H; y += 40)
        DrawLine(0, (int)y, PANEL_X, (int)y, GRID_CLR);
}
 
set<pair<string,string>> buildhighlightedges() {
    set<pair<string,string>> s;
    for(int i = 0; i + 1 < (int)pathresult.size(); i++) {
        s.insert({pathresult[i], pathresult[i+1]});
        s.insert({pathresult[i+1], pathresult[i]});
    }
    return s;
}
 
void drawgraph() {
    set<pair<string,string>> hledges = buildhighlightedges();
    set<pair<string,string>> drawn;
 
    for(auto &node : g.nodeindex) {
        string n1 = node.first;
        nodepos p1 = getpos(n1);
        for(auto &nb : g.adjlist[node.second]) {
            string n2 = nb.first;
            if(drawn.count({n1,n2}) || drawn.count({n2,n1})) continue;
            drawn.insert({n1,n2});
            nodepos p2 = getpos(n2);
            bool hl = hledges.count({n1,n2}) > 0;
            Color ec = hl ? HIGHLIGHT_EDGE : EDGE_CLR;
            float thick = hl ? 3.5f : 1.5f;
            DrawLineEx({p1.x, p1.y}, {p2.x, p2.y}, thick, ec);
            float mx = (p1.x + p2.x) / 2;
            float my = (p1.y + p2.y) / 2;
            string diststr = to_string(nb.second) + " km";
            Vector2 tv = MeasureTextEx(fontreg, diststr.c_str(), 10, 1);
            drawroundrect({mx - tv.x/2 - 2, my - tv.y/2 - 1, tv.x + 4, tv.y + 2}, 0.4f, BG_DARK);
            DrawTextEx(fontreg, diststr.c_str(), {mx - tv.x/2, my - tv.y/2}, 10, 1, EDGE_LABEL);
        }
    }
 
    for(auto &node : g.nodeindex) {
        string name = node.first;
        nodepos p = getpos(name);
        Color nc = NODE_CLR;
        if(!pathresult.empty()) {
            if(name == pathresult.front())     nc = SRC_CLR;
            else if(name == pathresult.back()) nc = DST_CLR;
            else {
                for(auto &pn : pathresult)
                    if(pn == name) { nc = PATH_NODE_CLR; break; }
            }
        }
        if(nc.r != NODE_CLR.r || nc.g != NODE_CLR.g || nc.b != NODE_CLR.b)
            DrawCircle((int)p.x, (int)p.y, NODE_RADIUS + 6, {nc.r, nc.g, nc.b, 60});
        DrawCircle((int)p.x, (int)p.y, NODE_RADIUS, nc);
        DrawCircleLines((int)p.x, (int)p.y, NODE_RADIUS, NODE_BORDER);
        Vector2 lp = labelpos(p, name, fontreg, 11);
        lp.x = max(2.0f, min(lp.x, (float)PANEL_X - 80));
        lp.y = max(2.0f, min(lp.y, (float)SCREEN_H - 20));
        DrawTextEx(fontreg, name.c_str(), lp, 11, 1, TEXT_CLR);
    }
}
 
void drawpanel() {
    DrawRectangle(PANEL_X, 0, PANEL_W, SCREEN_H, PANEL_BG);
    DrawLine(PANEL_X, 0, PANEL_X, SCREEN_H, PANEL_BORDER);
 
    float px = PANEL_X + 14;
    float py = 12;
    float bw = PANEL_W - 28;
 
    DrawTextEx(fontbold, "Smart Traffic Navigation System", {px, py}, 20, 1, TITLE_CLR);
    py += 26;
    DrawTextEx(fontreg, "Pakistan City Graph", {px, py}, 15, 1, SUBTITLE_CLR);
    py += 22;
 
    DrawLineEx({px, py}, {px + bw, py}, 1, DIVIDER); py += 10;
 
    DrawTextEx(fontbold, "Cities in Graph:", {px, py}, 15, 1, TITLE_CLR);
    py += 20;
 
    vector<string> cities;
    for(auto &n : g.nodeindex) cities.push_back(n.first);
    sort(cities.begin(), cities.end());
 
    float listh = 100;
    Rectangle listbox = {px, py, bw, listh};
    drawroundrect(listbox, 0.2f, INPUT_BG);
    drawroundrectborder(listbox, 0.2f, 1, INPUT_BORDER);
 
    if(mouseover(listbox)) {
        float wheel = GetMouseWheelMove();
        cityscrolloffset -= (int)wheel * 2;
        cityscrolloffset = max(0, cityscrolloffset);
    }
 
    float colw = bw / 2;
    float rowh = 20;
    int visrows = (int)(listh / rowh);
    int maxoffset = max(0, (int)((cities.size() + 1) / 2) - visrows + 1);
    cityscrolloffset = min(cityscrolloffset, maxoffset);
 
    BeginScissorMode((int)listbox.x, (int)listbox.y, (int)listbox.width, (int)listbox.height);
    for(int i = 0; i < (int)cities.size(); i++) {
        int row = i / 2 - cityscrolloffset;
        int col = i % 2;
        if(row < 0 || row >= visrows) continue;
        float cx2 = px + 6 + col * colw;
        float cy2 = py + 4 + row * rowh;
        DrawTextEx(fontreg, cities[i].c_str(), {cx2, cy2}, 14, 1, TEXT_CLR);
    }
    EndScissorMode();
    py += listh + 10;
 
    DrawLineEx({px, py}, {px + bw, py}, 1, DIVIDER); py += 10;
 
    DrawTextEx(fontreg, "Source City", {px, py}, 15, 1, DIM_CLR); py += 19;
    inpsource.rect = {px, py, bw, 36};
    drawinputbox(inpsource, fontreg, 15); py += 44;
 
    DrawTextEx(fontreg, "Destination City", {px, py}, 15, 1, DIM_CLR); py += 19;
    inpdest.rect = {px, py, bw, 36};
    drawinputbox(inpdest, fontreg, 15); py += 44;
 
    Rectangle btnfind = {px, py, bw, 38};
    drawroundrect(btnfind, 0.4f, mouseover(btnfind) ? BTN_HOVER : BTN_FIND);
    drawtextcentered(fontbold, "Find Shortest Path", btnfind, 15, WHITE);
    py += 46;
 
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseover(btnfind)) {
        string src = normalizename(inpsource.text);
        string dst = normalizename(inpdest.text);
        if(src.empty() || dst.empty()) {
            patherror = "Enter both cities!"; pathresult.clear();
        } else if(g.nodeindex.find(src) == g.nodeindex.end()) {
            patherror = "Source city not found!"; pathresult.clear();
        } else if(g.nodeindex.find(dst) == g.nodeindex.end()) {
            patherror = "Destination not found!"; pathresult.clear();
        } else {
            int dist = 0;
            pathresult = g.dijkstra(src, dst, dist);
            pathdist = dist;
            patherror = pathresult.empty() ? "No path found!" : "";
        }
    }
 
    DrawTextEx(fontbold, "Result:", {px, py}, 15, 1, TITLE_CLR); py += 20;
 
    if(!patherror.empty()) {
        DrawTextEx(fontreg, patherror.c_str(), {px, py}, 14, 1, DST_CLR);
        py += 18;
    } else if(!pathresult.empty()) {
        string line = "";
        float liney = py;
        for(int i = 0; i < (int)pathresult.size(); i++) {
            string word = pathresult[i];
            if(i + 1 < (int)pathresult.size()) word += " >";
            string test = line.empty() ? word : line + " " + word;
            float tw = MeasureTextEx(fontreg, test.c_str(), 14, 1).x;
            if(tw > bw && !line.empty()) {
                DrawTextEx(fontreg, line.c_str(), {px, liney}, 14, 1, HIGHLIGHT_EDGE);
                liney += 18; line = word;
            } else { line = test; }
        }
        if(!line.empty()) {
            DrawTextEx(fontreg, line.c_str(), {px, liney}, 14, 1, HIGHLIGHT_EDGE);
            liney += 18;
        }
        py = liney + 4;
        string diststr = "Shortest Distance: " + to_string(pathdist) + " km";
        DrawTextEx(fontbold, diststr.c_str(), {px, py}, 15, 1, SRC_CLR);
        py += 24;
    } else {
        DrawTextEx(fontreg, "Enter cities and click Find", {px, py}, 14, 1, DIM_CLR);
        py += 18;
    }
 
    DrawLineEx({px, py}, {px + bw, py}, 1, DIVIDER); py += 10;
 
    DrawTextEx(fontbold, "Add New Edge:", {px, py}, 15, 1, TITLE_CLR); py += 22;
 
    DrawTextEx(fontreg, "City 1", {px, py}, 14, 1, DIM_CLR); py += 17;
    inpaddcity1.rect = {px, py, bw, 34};
    drawinputbox(inpaddcity1, fontreg, 14); py += 40;
 
    DrawTextEx(fontreg, "City 2", {px, py}, 14, 1, DIM_CLR); py += 17;
    inpaddcity2.rect = {px, py, bw, 34};
    drawinputbox(inpaddcity2, fontreg, 14); py += 40;
 
    DrawTextEx(fontreg, "Distance (km)", {px, py}, 14, 1, DIM_CLR); py += 17;
    inpaddweight.rect = {px, py, bw, 34};
    drawinputbox(inpaddweight, fontreg, 14); py += 40;
 
    Rectangle btnadd = {px, py, bw, 36};
    drawroundrect(btnadd, 0.4f, mouseover(btnadd) ? BTN_HOVER : BTN_ADD);
    drawtextcentered(fontbold, "Add Edge", btnadd, 15, WHITE);
 
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseover(btnadd)) {
        string c1 = normalizename(inpaddcity1.text);
        string c2 = normalizename(inpaddcity2.text);
        string wstr = inpaddweight.text;
 
        if(c1.empty() || c2.empty() || wstr.empty()) {
            showpopup = true;
            popupmsg = "Fill all three fields!";
        } else if(c1 == c2) {
            showpopup = true;
            popupmsg = "City 1 and City 2 cannot be same!";
        } else {
            bool isnumber = true;
            for(char ch : wstr) if(!isdigit(ch)) { isnumber = false; break; }
            if(!isnumber || stoi(wstr) <= 0) {
                showpopup = true;
                popupmsg = "Enter a valid distance!";
            } else {
                int w = stoi(wstr);
                g.addnode(c1);
                g.addnode(c2);
 
                if(nodepositions.find(c1) == nodepositions.end()) {
                    int idx = (int)nodepositions.size();
                    nodepositions[c1] = autoplace(idx);
                }
                if(nodepositions.find(c2) == nodepositions.end()) {
                    int idx = (int)nodepositions.size();
                    nodepositions[c2] = autoplace(idx);
                }
 
                g.addedge(c1, c2, w);
                g.savefile("city_map.txt");
 
                inpaddcity1.text = "";
                inpaddcity2.text = "";
                inpaddweight.text = "";
 
                showpopup = true;
                popupmsg = "Edge added and saved!";
            }
        }
    }
}
 
void drawpopup() {
    if(!showpopup) return;
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,120});
    float pw = 380, ph = 160;
    float ppx = (SCREEN_W - pw) / 2;
    float ppy = (SCREEN_H - ph) / 2;
    Rectangle poprect = {ppx, ppy, pw, ph};
    drawroundrect(poprect, 0.15f, POPUP_BG);
    drawroundrectborder(poprect, 0.15f, 2, PANEL_BORDER);
    Vector2 tv = MeasureTextEx(fontreg, popupmsg.c_str(), 16, 1);
    DrawTextEx(fontreg, popupmsg.c_str(), {ppx + (pw - tv.x)/2, ppy + 40}, 16, 1, TEXT_CLR);
    Rectangle okbtn = {ppx + (pw - 110)/2, ppy + 100, 110, 38};
    drawroundrect(okbtn, 0.4f, mouseover(okbtn) ? BTN_HOVER : POPUP_OK);
    drawtextcentered(fontbold, "OK", okbtn, 16, WHITE);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseover(okbtn)) {
        showpopup = false; popupmsg = "";
    }
}