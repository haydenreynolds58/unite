#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace boss {
constexpr float Step = 1.0f / 60.0f;
constexpr float ChargeTime = .65f, ParryWindow = .28f, ExposeTime = 1.6f;
constexpr float Width = 960, Height = 540;
struct Vec { float x{}, y{}; };
inline Vec operator+(Vec a, Vec b) { return {a.x+b.x,a.y+b.y}; }
inline Vec operator-(Vec a, Vec b) { return {a.x-b.x,a.y-b.y}; }
inline Vec operator*(Vec a, float b) { return {a.x*b,a.y*b}; }
inline float dot(Vec a, Vec b) { return a.x*b.x+a.y*b.y; }
inline float length(Vec a) { return std::sqrt(dot(a,a)); }
inline Vec normal(Vec a) { float n=length(a); return n>.001f ? a*(1/n) : Vec{1,0}; }
struct Input { float x{}, y{}, aimX{480}, aimY{200}; bool action{}, dodge{}; };
struct Player {
    Vec pos{}; int hp{3};
    float parry{}, parryCooldown{}, dodge{}, dodgeCooldown{}, charge{}, hurt{};
    Vec dash{1,0}; bool wasAction{}, wasDodge{};
};
enum class Phase { Waiting, Windup, Exposed, Recovery, Rain, Victory, Defeat };
struct State {
    std::array<Player,2> players{};
    Vec bossPos{480,200}, slam{};
    std::array<Vec,2> rain{};
    Phase phase{Phase::Waiting}; float timer{}, flash{}, beamTimer{};
    Vec beamStart{}, beamEnd{};
    int hp{100}, cycle{}, combo{};
    std::uint32_t tick{};
};
inline State initial() {
    State s; s.players[0].pos={430,315}; s.players[1].pos={610,380}; return s;
}
inline void beginWindup(State& s) {
    s.phase=Phase::Windup; s.timer=s.hp<=50 ? 1.05f : 1.45f;
    s.slam=s.players[0].pos;
}
inline void damage(Player& p) {
    if (p.dodge<=0 && p.hurt<=0) { --p.hp; p.hurt=.8f; }
}
inline bool aimsAtBoss(Vec start, Vec aim, Vec center) {
    Vec d=normal(aim-start), offset=center-start;
    float along=dot(offset,d);
    return along>=0 && along<900 && length(offset-d*along)<43;
}
// Pure fixed-step gameplay. Only the host calls this in online mode.
inline void update(State& s, const std::array<Input,2>& input, float dt=Step) {
    ++s.tick;
    s.flash=std::max(0.f,s.flash-dt); s.beamTimer=std::max(0.f,s.beamTimer-dt);
    if (s.phase==Phase::Waiting || s.phase==Phase::Victory || s.phase==Phase::Defeat) return;
    for (int i=0;i<2;++i) {
        auto& p=s.players[i]; const auto& in=input[i];
        auto decay=[dt](float& v) { v=std::max(0.f,v-dt); };
        decay(p.parry); decay(p.parryCooldown); decay(p.dodge);
        decay(p.dodgeCooldown); decay(p.hurt);
        Vec move{in.x,in.y}; if(length(move)>1) move=normal(move);
        if(in.dodge && !p.wasDodge && p.dodgeCooldown<=0) {
            p.dodge=.22f; p.dodgeCooldown=.85f;
            p.dash=length(move)>.01f ? normal(move) : normal(Vec{in.aimX,in.aimY}-p.pos);
        }
        p.pos=p.pos+(p.dodge>0 ? p.dash*480.f : move*180.f)*dt;
        p.pos.x=std::clamp(p.pos.x,55.f,905.f); p.pos.y=std::clamp(p.pos.y,115.f,465.f);
        if(i==0 && in.action && !p.wasAction && p.parryCooldown<=0) {
            p.parry=ParryWindow; p.parryCooldown=.85f;
        }
        if(i==1) {
            if(in.action) p.charge=std::min(1.f,p.charge+dt);
            if(!in.action && p.wasAction) {
                s.beamStart=p.pos; s.beamEnd=p.pos+normal(Vec{in.aimX,in.aimY}-p.pos)*900;
                s.beamTimer=.16f;
                if(p.charge>=ChargeTime && s.phase==Phase::Exposed &&
                   aimsAtBoss(p.pos,{in.aimX,in.aimY},s.bossPos)) {
                    s.hp=std::max(0,s.hp-25); ++s.combo; s.flash=.25f;
                    s.phase=s.hp==0 ? Phase::Victory : Phase::Recovery; s.timer=.9f;
                }
                p.charge=0;
            }
        }
        p.wasAction=in.action; p.wasDodge=in.dodge;
    }
    if(s.phase==Phase::Victory) return;
    s.timer-=dt;
    if(s.timer<=0) {
        switch(s.phase) {
        case Phase::Windup: {
            bool parried=length(s.players[0].pos-s.slam)<90 && s.players[0].parry>0;
            if(parried) { s.phase=Phase::Exposed; s.timer=ExposeTime; s.flash=.3f; }
            else {
                for(auto& p:s.players) if(length(p.pos-s.slam)<90) damage(p);
                s.phase=Phase::Recovery; s.timer=.8f;
            }
            break;
        }
        case Phase::Exposed: s.phase=Phase::Recovery; s.timer=.6f; break;
        case Phase::Recovery:
            s.phase=Phase::Rain; s.timer=s.hp<=50 ? .7f : 1.f;
            for(int i=0;i<2;++i) s.rain[i]=s.players[i].pos;
            break;
        case Phase::Rain:
            for(auto& p:s.players)
                for(auto target:s.rain) if(length(p.pos-target)<68) damage(p);
            ++s.cycle; beginWindup(s); break;
        default: break;
        }
    }
    for(const auto& p:s.players) if(p.hp<=0) s.phase=Phase::Defeat;
}
inline Input practiceArcher(const State& s) {
    Input i; i.aimX=s.bossPos.x; i.aimY=s.bossPos.y;
    i.action=!(s.phase==Phase::Exposed && s.players[1].charge>=ChargeTime);
    // Step away from a rain marker while charging.
    if(s.phase==Phase::Rain) i.x=s.rain[1].x>650 ? -1.f : 1.f;
    return i;
}
inline const char* phaseName(Phase p) {
    switch(p) {
    case Phase::Waiting:return "WAITING FOR ARCHER";
    case Phase::Windup:return "WARDEN: PARRY THE SLAM";
    case Phase::Exposed:return "CORE OPEN - ARCHER, RELEASE!";
    case Phase::Recovery:return "RECOVER / PREPARE";
    case Phase::Rain:return "ASHFALL - MOVE OUT OF THE CIRCLES";
    case Phase::Victory:return "THE BELLKEEPER HAS FALLEN";
    case Phase::Defeat:return "THE PACT IS BROKEN";
    }
    return "";
}
}
