#include "protocol.hpp"
#include <iostream>
#include <stdexcept>
using namespace boss;
static void check(bool condition,const char* message) { if(!condition) throw std::runtime_error(message); }
int main() {
    try {
        State s=initial(); beginWindup(s); s.timer=Step/2;
        Input w; w.action=true; update(s,{w,{}});
        check(s.phase==Phase::Exposed,"Timed parry must expose core");
        Input a; a.action=true; a.aimX=480; a.aimY=200;
        for(int n=0;n<42;++n) update(s,{Input{},a});
        a.action=false; update(s,{Input{},a});
        check(s.hp==75 && s.combo==1,"Charged partner shot must damage exposed boss");

        s=initial(); beginWindup(s);
        s.players[1].charge=1; s.players[1].wasAction=true;
        update(s,{Input{},a}); check(s.hp==100,"Armor must reject solo charged shot");
        s.phase=Phase::Exposed; s.timer=1; s.players[1].charge=.2f; s.players[1].wasAction=true;
        update(s,{Input{},a}); check(s.hp==100,"Undercharged shot must fail");
        s.players[1].charge=1; s.players[1].wasAction=true; a.aimX=900; a.aimY=450;
        update(s,{Input{},a}); check(s.hp==100,"Misaimed shot must fail");

        s=initial(); beginWindup(s); s.timer=Step/2; update(s,{Input{},Input{}});
        check(s.players[0].hp==2,"Missed parry must damage Warden");
        s=initial(); beginWindup(s); s.timer=Step/2; w={}; w.dodge=true;
        update(s,{w,{}}); check(s.players[0].hp==3,"Dodge must prevent damage");

        s=initial(); beginWindup(s); s.timer=Step/2; w={}; w.action=true;
        s.players[0].pos={70,450}; update(s,{w,{}});
        check(s.phase!=Phase::Exposed,"Parry outside slam must fail");

        s=initial(); beginWindup(s);
        for(int hit=0;hit<4;++hit) {
            s.phase=Phase::Exposed; s.timer=1;
            s.players[1].charge=1; s.players[1].wasAction=true;
            a={}; a.aimX=480; a.aimY=200; update(s,{Input{},a});
        }
        check(s.phase==Phase::Victory && s.hp==0,"Four combos must win");

        s=initial(); beginWindup(s); s.players[0].hp=1; s.timer=Step/2;
        update(s,{Input{},Input{}}); check(s.phase==Phase::Defeat,"Death must end encounter");
        s=initial(); beginWindup(s); s.phase=Phase::Exposed; s.timer=Step/2;
        update(s,{Input{},Input{}}); check(s.phase==Phase::Recovery,"Unused opening must expire");

        Input decoded; Input source{.5f,-1,480,200,true,false};
        check(wire::decodeInput(wire::encodeInput(source),decoded) && decoded.action,"Input roundtrip");
        check(!wire::decodeInput("I 1 0 0 0 0 0 0",decoded),"Version mismatch");
        check(!wire::decodeInput("I 1431193857 nan 0 0 0 0 0",decoded),"Nonfinite input");
        check(!wire::decodeInput(wire::encodeInput(source)+" garbage",decoded),"Trailing input");
        source.x=2; check(!wire::decodeInput(wire::encodeInput(source),decoded),"Invalid movement");
        State copy;
        check(wire::decodeState(wire::encodeState(s),copy) && copy.phase==s.phase,"State roundtrip");
        check(!wire::decodeState("S",copy),"Truncated snapshot");
        check(!wire::decodeState(std::string(4096,'x'),copy),"Oversized snapshot");
        std::cout<<"All simulation and protocol checks passed\n";
    } catch(const std::exception& e) { std::cerr<<e.what()<<"\n"; return 1; }
}
