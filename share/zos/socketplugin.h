#ifndef UDPPLUGIN_H
#define UDPPLUGIN_H

#include "zos/data.h"
#include "zos/socket.h"

namespace zos{
namespace udp{
template<zos::concepts::Serializable T>
class Plugin{
public:
    Plugin(){};
    Plugin(const char* multicast_address){
        _socket.join_multicast(zos::udp::address::from_string(multicast_address));
    }
    Plugin(int port,const zos::udp::__callback_type& f={}):Plugin(zos::udp::endpoint(zos::udp::address::from_string("0.0.0.0"),port),nullptr,f){}
    Plugin(const zos::udp::endpoint& ep,const zos::udp::__callback_type& f = {}):Plugin(ep,nullptr,f){}
    Plugin(const zos::udp::endpoint& ep,const char* multicast_address,const zos::udp::__callback_type& f = {}){
        _socket.bind(ep,f);
        _socket.join_multicast(zos::udp::address::from_string(multicast_address));
    }
    void sendData(const T& t,const zos::udp::endpoint& ep){
        static zos::Data data;
        size_t size = t.ByteSize();
        data.resize(size);
        t.SerializeToArray(data.ptr(),size);
        _socket.send_to(data.data(),size,ep);
    }
    void sendData(const void* p,size_t size,int port){
        _socket.send_to(p,size,zos::udp::endpoint(zos::udp::address::from_string("0.0.0.0"),port));
    }
private:
    zos::udp::socket _socket;
};
} // namespace zos::udp;
} // namespace zos;
#endif // UDPPLUGIN_H
