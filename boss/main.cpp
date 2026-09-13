#include "network.hpp"
#include <raylib.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace boss;
static Vector2 rv(Vec p) { return {p.x,p.y}; }
static Color ink{18,20,29,255}, stone{40,45,55,255}, gold{226,183,103,255}, blue{101,197,221,255};
static void bar(int x,int y,int w,float fraction,Color c) {
    DrawRectangle(x,y,w,5,stone); DrawRectangle(x,y,static_cast<int>(w*std::clamp(fraction,0.f,1.f)),5,c);
}
static void actor(Vec p,Color c,bool warden,float glow) {
    int x=static_cast<int>(p.x),y=static_cast<int>(p.y);
    DrawEllipse(x,y+12,15,5,{9,11,17,255});
    DrawRectangle(x-7,y-9,14,20,c); DrawRectangle(x-5,y-17,10,10,{204,195,167,255});
    DrawRectangle(x-8,y+9,5,5,stone); DrawRectangle(x+3,y+9,5,5,stone);
    if(warden) { DrawRectangle(x-15,y-4,7,15,gold); DrawRectangle(x+10,y-13,3,22,LIGHTGRAY); }
    else { DrawRectangle(x+9,y-10,3,20,gold); DrawRectangle(x+12,y-6,3,12,gold); }
    if(glow>0) DrawCircleLines(x,y,23,warden ? gold : blue);
}
static void render(const State& s,const std::array<Vec,2>& positions,bool hosting,bool practice,
                   const std::string& status,unsigned ping,bool debug) {
    ClearBackground(ink);
    DrawText("UNITE / THE ASHEN BELL",35,18,20,gold);
    DrawText(practice ? "PRACTICE  /  YOU ARE THE WARDEN" : hosting ? "HOST  /  WARDEN" : "CLIENT  /  ARCHER",35,45,12,LIGHTGRAY);
    DrawText(TextFormat("%s   RTT %u ms",status.c_str(),ping),550,24,12,LIGHTGRAY);
    DrawRectangle(35,95,890,390,{27,31,39,255});
    for(int y=98;y<480;y+=24) for(int x=38;x<920;x+=32) {
        Color c=((x/32+y/24)%3==0) ? Color{34,38,46,255} : Color{30,34,42,255};
        DrawRectangle(x,y,30,22,c);
    }
    DrawRectangleLinesEx({35,95,890,390},3,{66,64,63,255});
    for(int x:{70,875}) for(int y:{135,430}) {
        DrawRectangle(x-10,y-14,20,32,stone);
        DrawRectangle(x-13,y-17,26,7,{70,70,74,255});
        DrawCircle(x,y-23,5,gold);
    }
    if(s.phase==Phase::Windup) {
        DrawCircleV(rv(s.slam),90,Fade(RED,.12f));
        DrawCircleLinesV(rv(s.slam),90,{194,79,72,255});
        DrawCircleLinesV(rv(s.slam),90*std::clamp(s.timer/1.45f,0.f,1.f),gold);
        DrawText("SLAM",static_cast<int>(s.slam.x)-16,static_cast<int>(s.slam.y)-6,10,gold);
    }
    if(s.phase==Phase::Rain) for(auto p:s.rain) {
        DrawCircleV(rv(p),68,Fade(RED,.18f));
        DrawCircleLinesV(rv(p),68,RED);
        DrawLine(static_cast<int>(p.x)-8,static_cast<int>(p.y),static_cast<int>(p.x)+8,static_cast<int>(p.y),gold);
    }
    int bx=480,by=200;
    DrawEllipse(bx,by+38,55,12,{12,14,21,255});
    DrawRectangle(bx-35,by-35,70,70,{78,79,85,255});
    DrawRectangle(bx-49,by-22,16,52,stone); DrawRectangle(bx+33,by-22,16,52,stone);
    DrawRectangle(bx-20,by-52,40,24,{112,107,99,255});
    DrawRectangle(bx-26,by-57,52,8,gold);
    DrawRectangle(bx-13,by-42,7,4,ink); DrawRectangle(bx+6,by-42,7,4,ink);
    DrawCircle(bx,by,14,s.phase==Phase::Exposed ? blue : ink);
    if(s.phase==Phase::Exposed) DrawCircleLines(bx,by,48,blue);
    for(int i=0;i<2;++i) {
        const auto& p=s.players[i];
        actor(positions[i],p.hurt>0 ? WHITE : i==0 ? gold : blue,i==0,i==0 ? p.parry : p.charge>=ChargeTime);
        if(p.dodge>0) DrawCircleLinesV(rv(positions[i]),20,WHITE);
        for(int h=0;h<3;++h) DrawRectangle(static_cast<int>(positions[i].x)-12+h*9,static_cast<int>(positions[i].y)+21,6,4,h<p.hp ? RED : stone);
        if(debug) DrawCircleLinesV(rv(p.pos),12,GREEN);
    }
    if(s.beamTimer>0) DrawLineEx(rv(s.beamStart),rv(s.beamEnd),3,blue);
    if(s.flash>0) DrawRectangle(35,95,890,390,Fade(gold,s.flash*.4f));
    DrawText("THE BELLKEEPER",345,63,14,LIGHTGRAY); bar(345,83,270,s.hp/100.f,s.hp<=50 ? RED : gold);
    const char* phase=phaseName(s.phase);
    DrawText(phase,480-MeasureText(phase,16)/2,105,16,s.phase==Phase::Exposed ? blue : gold);
    if(s.phase==Phase::Windup || s.phase==Phase::Exposed || s.phase==Phase::Rain)
        DrawText(TextFormat("%.2fs",std::max(0.f,s.timer)),462,125,12,WHITE);
    DrawText("WASD move   SPACE dodge   E / LEFT MOUSE parry or charge + release",35,494,14,LIGHTGRAY);
    DrawText("R host restart   F1 hitboxes   ESC quit",35,515,12,GRAY);
    bar(690,516,100,1-s.players[hosting ? 0 : 1].dodgeCooldown/.85f,WHITE);
    DrawText("DODGE",690,499,10,GRAY);
    if(hosting) {
        bar(805,516,100,1-s.players[0].parryCooldown/.85f,gold); DrawText("PARRY",805,499,10,gold);
    } else {
        bar(805,516,100,s.players[1].charge/ChargeTime,blue); DrawText("CHARGE",805,499,10,blue);
    }
    if(s.phase==Phase::Waiting || s.phase==Phase::Victory || s.phase==Phase::Defeat) {
        DrawRectangle(215,310,530,95,Fade(ink,.95f));
        const char* title=s.phase==Phase::Waiting ? "Awaiting a second soul..." : s.phase==Phase::Victory ? "PACT FULFILLED" : "TRY AGAIN";
        DrawText(title,480-MeasureText(title,24)/2,328,24,gold);
        const char* sub=s.phase==Phase::Waiting ? "Run --join HOST_IP on your partner's machine" : "Host presses R to restart the encounter";
        DrawText(sub,480-MeasureText(sub,14)/2,369,14,LIGHTGRAY);
    }
}
static Input readInput() {
    Input i;
    if(IsWindowFocused()) {
        i.x=float(IsKeyDown(KEY_D))-float(IsKeyDown(KEY_A));
        i.y=float(IsKeyDown(KEY_S))-float(IsKeyDown(KEY_W));
        i.action=IsMouseButtonDown(MOUSE_BUTTON_LEFT)||IsKeyDown(KEY_E);
        i.dodge=IsKeyDown(KEY_SPACE);
    }
    Vector2 mouse=GetMousePosition(); i.aimX=mouse.x; i.aimY=mouse.y;
    return i;
}
int main(int argc,char** argv) {
    try {
        bool hosting=true,practice=false,headless=false,capture=false; std::string address="127.0.0.1";
        unsigned short port=7777; float seconds=0; int modes=0;
        for(int a=1;a<argc;++a) {
            std::string arg=argv[a];
            if(arg=="--host") { hosting=true; ++modes; }
            else if(arg=="--practice") { practice=true; ++modes; }
            else if(arg=="--join" && a+1<argc) { hosting=false; address=argv[++a]; ++modes; }
            else if(arg=="--port" && a+1<argc) {
                std::string value=argv[++a]; std::size_t used;
                int p=std::stoi(value,&used); if(used!=value.size() || p<1 || p>65535) throw std::runtime_error("Port must be 1..65535");
                port=static_cast<unsigned short>(p);
            } else if(arg=="--headless") headless=true;
            else if(arg=="--capture") capture=true;
            else if(arg=="--seconds" && a+1<argc) {
                std::string value=argv[++a]; std::size_t used; seconds=std::stof(value,&used);
                if(used!=value.size() || !std::isfinite(seconds) || seconds<=0) throw std::runtime_error("Seconds must be positive");
            } else {
                std::cout<<"Usage: unite_boss [--host | --join ADDRESS | --practice] [--port 7777]\n"
                           "                  [--headless --seconds N] [--capture]\n";
                return arg=="--help" ? 0 : 1;
            }
        }
        if(modes>1) throw std::runtime_error("Choose one of --host, --join, --practice");
        if(headless && seconds<=0) throw std::runtime_error("Headless mode requires --seconds N");
        std::unique_ptr<Network> net;
        if(!practice) net=std::make_unique<Network>(hosting,address,port);
        State state=initial();
        if(practice) beginWindup(state);
        std::array<Vec,2> positions{state.players[0].pos,state.players[1].pos};
        bool debug=false,everConnected=false,everSnapshot=false;
        float accumulator=0,sendTimer=0,noSnapshot=0;
        Input lastSent{};
        float lastRemoteX=state.players[1].pos.x;
        std::string status=practice ? "BOT ARCHER" : hosting ? "LISTENING UDP "+std::to_string(port) : "CONNECTING";
        if(!headless) {
            InitWindow(960,540,"Unite - The Ashen Bell");
            SetTargetFPS(60);
        }
        auto start=std::chrono::steady_clock::now(),previous=start;
        while(headless || !WindowShouldClose()) {
            auto now=std::chrono::steady_clock::now();
            float elapsed=std::chrono::duration<float>(now-start).count();
            if(seconds>0 && elapsed>=seconds) break;
            float dt=std::min(.1f,std::chrono::duration<float>(now-previous).count()); previous=now;
            Input local=headless ? Input{} : readInput();
            if(headless && !hosting) local.x=.5f; // Smoke-test observable host-owned movement.
            if(net) {
                net->poll();
                if(net->justConnected) {
                    everConnected=true; status="CONNECTED";
                    if(hosting) { state=initial(); beginWindup(state); }
                }
                if(net->justDisconnected) {
                    status=hosting ? "PARTNER LEFT - WAITING" : "DISCONNECTED - RELAUNCH TO JOIN";
                    if(hosting) state=initial();
                }
                if(!hosting && net->freshState) {
                    state=net->received; everSnapshot=true; noSnapshot=0;
                    lastRemoteX=state.players[1].pos.x;
                }
                noSnapshot+=dt;
                if(!hosting && net->connected && noSnapshot>3) status="NO SNAPSHOTS - CONNECTION STALLED";
                else if(net->connected) status="CONNECTED";
                if(!hosting && !everConnected && elapsed>8) status="CONNECTION FAILED - CHECK IP / UDP PORT";
            }
            if(!headless && IsKeyPressed(KEY_F1)) debug=!debug;
            if(!headless && hosting && IsKeyPressed(KEY_R)) {
                state=initial(); if(practice || net->connected) beginWindup(state);
            }
            accumulator+=dt;
            int steps=0;
            while(accumulator>=Step && steps++<6) {
                if(hosting) {
                    Input other=practice ? practiceArcher(state) : net->remoteInput;
                    update(state,{local,other});
                }
                accumulator-=Step;
            }
            sendTimer+=dt;
            if(net && (sendTimer>=1.f/30 || (!hosting && (local.action!=lastSent.action || local.dodge!=lastSent.dodge)))) {
                sendTimer=0;
                if(hosting) net->snapshot(state); else { net->input(local); lastSent=local; }
            }
            for(int i=0;i<2;++i)
                positions[i]=hosting ? state.players[i].pos : positions[i]+(state.players[i].pos-positions[i])*std::min(1.f,dt*22);
            if(!headless) {
                BeginDrawing(); render(state,positions,hosting,practice,status,net ? net->ping() : 0,debug); EndDrawing();
                if(capture && elapsed>.3f) { TakeScreenshot("boss-preview.png"); capture=false; }
            } else std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        if(!headless) CloseWindow();
        std::cout<<"connected="<<everConnected<<" snapshot="<<everSnapshot
                 <<" tick="<<state.tick<<" archer_x="<<(hosting ? state.players[1].pos.x : lastRemoteX)<<"\n";
        if(headless && !practice && (!everConnected || (!hosting && !everSnapshot))) return 2;
        return 0;
    } catch(const std::exception& e) { std::cerr<<e.what()<<"\n"; return 1; }
}
