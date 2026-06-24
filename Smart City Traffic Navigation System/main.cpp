#include "raylib.h"
#include "ui.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1400, 820, "Smart Traffic Navigation System");
    SetTargetFPS(60);

    fontbold = LoadFontEx("arial.ttf", 56, nullptr, 256);
    fontreg  = LoadFontEx("arial.ttf", 48, nullptr, 256);

    g.loadfromfile("city_map.txt");

    for(auto &dp : defaultpositions) {
        string name = dp.first;
        name[0] = toupper(name[0]);
        if(g.nodeindex.count(name) > 0)
            nodepositions[name] = dp.second;
    }

    int autoidx = 0;
    for(auto &node : g.nodeindex) {
        if(nodepositions.find(node.first) == nodepositions.end())
            nodepositions[node.first] = autoplace(autoidx++);
    }

    inpsource    = {{0,0,0,0}, "", false, "e.g. Lahore"};
    inpdest      = {{0,0,0,0}, "", false, "e.g. Karachi"};
    inpaddcity1  = {{0,0,0,0}, "", false, "e.g. Multan"};
    inpaddcity2  = {{0,0,0,0}, "", false, "e.g. Quetta"};
    inpaddweight = {{0,0,0,0}, "", false, "e.g. 5"};

    while(!WindowShouldClose()) {
        Vector2 mp = GetMousePosition();
        bool ingrapharea =  mp.x < 980;

        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && ingrapharea && !showpopup) {
            ispanning = true;
            panstart = mp;
        }
        if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            ispanning = false;
        }
        if(ispanning && IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            panoffsetx += mp.x - panstart.x;
            panoffsety += mp.y - panstart.y;
            panstart = mp;
        }

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !showpopup) {
            deactivateall();
            if(mouseover(inpsource.rect))    inpsource.active    = true;
            if(mouseover(inpdest.rect))      inpdest.active      = true;
            if(mouseover(inpaddcity1.rect))  inpaddcity1.active  = true;
            if(mouseover(inpaddcity2.rect))  inpaddcity2.active  = true;
            if(mouseover(inpaddweight.rect)) inpaddweight.active = true;
        }

        if(!showpopup) {
            handleinput(inpsource);
            handleinput(inpdest);
            handleinput(inpaddcity1);
            handleinput(inpaddcity2);
            handleinput(inpaddweight);
        }

        BeginDrawing();
        ClearBackground({10, 15, 35, 255});
        drawgrid();
        drawgraph();
        drawpanel();
        drawpopup();
        EndDrawing();
    }

    UnloadFont(fontbold);
    UnloadFont(fontreg);
    CloseWindow();
    return 0;
}