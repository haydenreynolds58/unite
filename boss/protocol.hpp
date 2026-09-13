#pragma once
#include "simulation.hpp"
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

// Versioned, bounded textual packets avoid ABI/padding/endianness assumptions.
// No user-provided strings or filesystem operations are accepted from peers.
namespace boss::wire {
constexpr unsigned Magic = 0x554e4901;
inline bool finite(float v, float limit=10000) { return std::isfinite(v) && std::abs(v)<=limit; }
inline std::ostringstream output() {
    std::ostringstream o; o.imbue(std::locale::classic()); o<<std::setprecision(7); return o;
}
inline bool end(std::istringstream& in) { in>>std::ws; return in.eof() && !in.bad(); }
inline std::string encodeInput(const Input& i) {
    auto o=output(); o<<"I "<<Magic<<' '<<i.x<<' '<<i.y<<' '<<i.aimX<<' '<<i.aimY<<' '<<i.action<<' '<<i.dodge; return o.str();
}
inline bool decodeInput(const std::string& data, Input& result) {
    if(data.size()>160) return false;
    std::istringstream in(data); in.imbue(std::locale::classic());
    char type; unsigned version; Input i; int action,dodge;
    if(!(in>>type>>version>>i.x>>i.y>>i.aimX>>i.aimY>>action>>dodge) ||
       type!='I' || version!=Magic || !end(in) ||
       !finite(i.x,1) || !finite(i.y,1) || !finite(i.aimX) || !finite(i.aimY) ||
       action<0 || action>1 || dodge<0 || dodge>1) return false;
    i.action=action; i.dodge=dodge; result=i; return true;
}
inline std::string encodeState(const State& s) {
    auto o=output();
    o<<"S "<<Magic<<' '<<s.tick<<' '<<static_cast<int>(s.phase)<<' '<<s.timer<<' '<<s.hp<<' '<<s.cycle<<' '<<s.combo
     <<' '<<s.slam.x<<' '<<s.slam.y<<' '<<s.flash<<' '<<s.beamTimer
     <<' '<<s.beamStart.x<<' '<<s.beamStart.y<<' '<<s.beamEnd.x<<' '<<s.beamEnd.y;
    for(auto r:s.rain) o<<' '<<r.x<<' '<<r.y;
    for(const auto& p:s.players)
        o<<' '<<p.pos.x<<' '<<p.pos.y<<' '<<p.hp<<' '<<p.parry<<' '<<p.parryCooldown
         <<' '<<p.dodge<<' '<<p.dodgeCooldown<<' '<<p.charge<<' '<<p.hurt;
    return o.str();
}
inline bool decodeState(const std::string& data, State& result) {
    if(data.size()>2048) return false;
    std::istringstream in(data); in.imbue(std::locale::classic());
    char type; unsigned version; int phase; State s=initial();
    if(!(in>>type>>version>>s.tick>>phase>>s.timer>>s.hp>>s.cycle>>s.combo
         >>s.slam.x>>s.slam.y>>s.flash>>s.beamTimer
         >>s.beamStart.x>>s.beamStart.y>>s.beamEnd.x>>s.beamEnd.y) ||
         type!='S' || version!=Magic || phase<0 || phase>6 || s.hp<0 || s.hp>100 ||
         s.cycle<0 || s.combo<0 || s.combo>4) return false;
    for(auto& r:s.rain) if(!(in>>r.x>>r.y) || !finite(r.x) || !finite(r.y)) return false;
    for(auto& p:s.players) {
        if(!(in>>p.pos.x>>p.pos.y>>p.hp>>p.parry>>p.parryCooldown
               >>p.dodge>>p.dodgeCooldown>>p.charge>>p.hurt) ||
           !finite(p.pos.x) || !finite(p.pos.y) || p.hp<0 || p.hp>3) return false;
        for(float f:{p.parry,p.parryCooldown,p.dodge,p.dodgeCooldown,p.charge,p.hurt})
            if(!finite(f,2) || f<0) return false;
    }
    for(float f:{s.timer,s.slam.x,s.slam.y,s.flash,s.beamTimer,
                 s.beamStart.x,s.beamStart.y,s.beamEnd.x,s.beamEnd.y})
        if(!finite(f)) return false;
    if(!end(in)) return false;
    s.phase=static_cast<Phase>(phase); result=s; return true;
}
}
