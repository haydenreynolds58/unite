#pragma once
#include "protocol.hpp"
#include <enet/enet.h>
#include <stdexcept>

namespace boss {
class Network {
    ENetHost* host{};
    ENetPeer* peer{};
    bool server{};
public:
    bool connected{}, freshState{}, justConnected{}, justDisconnected{};
    Input remoteInput{};
    State received=initial();
    explicit Network(bool hosting, const std::string& address, unsigned short port):server(hosting) {
        if(enet_initialize()!=0) throw std::runtime_error("ENet initialization failed");
        ENetAddress bind{}; bind.host=ENET_HOST_ANY; bind.port=port;
        host=enet_host_create(server ? &bind : nullptr,1,2,0,0);
        if(!host) { enet_deinitialize(); throw std::runtime_error("Cannot create network host. Is the UDP port already in use?"); }
        if(!server) {
            ENetAddress destination{}; destination.port=port;
            if(enet_address_set_host(&destination,address.c_str())!=0) {
                enet_host_destroy(host); enet_deinitialize(); throw std::runtime_error("Cannot resolve host address");
            }
            peer=enet_host_connect(host,&destination,2,wire::Magic);
            if(!peer) { enet_host_destroy(host); enet_deinitialize(); throw std::runtime_error("Cannot create connection"); }
            enet_peer_timeout(peer,8,2000,5000);
        }
    }
    ~Network() {
        if(peer) enet_peer_disconnect_now(peer,0);
        enet_host_destroy(host); enet_deinitialize();
    }
    Network(const Network&)=delete;
    Network& operator=(const Network&)=delete;
    void poll() {
        freshState=justConnected=justDisconnected=false;
        ENetEvent e;
        // Bound work per frame even when a peer sends too many packets.
        for(int budget=0;budget<256 && enet_host_service(host,&e,0)>0;++budget) {
            if(e.type==ENET_EVENT_TYPE_CONNECT) {
                if(server && e.data!=wire::Magic) { enet_peer_disconnect_now(e.peer,1); continue; }
                peer=e.peer; connected=true; justConnected=true;
                enet_peer_timeout(peer,8,2000,5000);
            } else if(e.type==ENET_EVENT_TYPE_DISCONNECT) {
                connected=false; peer=nullptr; remoteInput={}; justDisconnected=true;
            } else if(e.type==ENET_EVENT_TYPE_RECEIVE) {
                if(e.packet->dataLength<=2048 && e.peer==peer) {
                    std::string data(reinterpret_cast<char*>(e.packet->data),e.packet->dataLength);
                    if(server && e.channelID==0) wire::decodeInput(data,remoteInput);
                    else if(!server && e.channelID==1) {
                        State candidate;
                        if(wire::decodeState(data,candidate)) { received=candidate; freshState=true; }
                    }
                }
                enet_packet_destroy(e.packet);
            }
        }
    }
    void send(const std::string& bytes, enet_uint8 channel, bool reliable) {
        if(!connected || !peer) return;
        auto* p=enet_packet_create(bytes.data(),bytes.size(),reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        if(p && enet_peer_send(peer,channel,p)!=0) enet_packet_destroy(p);
        enet_host_flush(host);
    }
    void input(const Input& i) { send(wire::encodeInput(i),0,true); }
    void snapshot(const State& s) { send(wire::encodeState(s),1,false); }
    unsigned ping() const { return peer && connected ? peer->roundTripTime : 0; }
};
}
