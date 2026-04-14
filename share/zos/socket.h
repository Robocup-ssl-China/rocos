#ifndef __ZOS_UDP_SOCKET_H__
#define __ZOS_UDP_SOCKET_H__
#include <string>
#include <array>
#include <boost/asio.hpp>
#include <fmt/core.h>
#include <iostream>
#include "zos/type.h"
#include "zos/utils/singleton.h"

namespace zos{
using __io = Singleton<boost::asio::io_context>;
namespace udp{
using __callback_type = zos::type::socket_callback_type;
using endpoint = boost::asio::ip::udp::endpoint;
using address = boost::asio::ip::address;
using address_v4 = boost::asio::ip::address_v4;
class socket{
public:
    socket():_socket(*__io::_(),boost::asio::ip::udp::v4()){}
    ~socket() = default;
    bool bind(const boost::asio::ip::udp::endpoint& _listen_ep,const __callback_type& f = {}){
        if(f){
            _callback = std::bind(f,std::placeholders::_1,std::placeholders::_2);
        }
        boost::system::error_code ec;
        _socket.set_option(boost::asio::ip::udp::socket::reuse_address(true), ec);
        if(ec.value() != 0){
            std::cerr << fmt::format("get error11 {}:{}",ec.value(),ec.message()) << std::endl;
            return false;
        }
        _socket.bind(_listen_ep,ec);
        if(ec.value() != 0){
            std::cerr << fmt::format("get error11 {}:{}",ec.value(),ec.message()) << std::endl;
            return false;
        }
        _socket.async_receive_from(boost::asio::buffer(_data,MAX_LENGTH),_received_ep
            , std::bind(&socket::handle_receive_from, this, std::placeholders::_1, std::placeholders::_2)
        );
        return true;
    }
    void join_multicast(const boost::asio::ip::address& multicast_address,const boost::asio::ip::address& if_address = boost::asio::ip::address_v4::any()){
        _socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        _socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
        _socket.set_option(boost::asio::ip::multicast::join_group(multicast_address.to_v4(),if_address.to_v4()));
    }
    void set_interface(const boost::asio::ip::address& if_address){
        auto _if = boost::asio::ip::multicast::outbound_interface(if_address.to_v4());
        // std::cout << "set interface " << _if.ipv4_value_ << std::endl;
        _socket.set_option(_if);
    }
    void set_interface(const int if_num){
        auto _if = boost::asio::ip::multicast::outbound_interface(if_num);
        // std::cout << "set interface " << _if << std::endl;
        _socket.set_option(_if);
    }
    void set_callback(const __callback_type& f){
        _callback = std::bind(f,std::placeholders::_1,std::placeholders::_2);
    }
    bool try_bind(){
        boost::system::error_code ec;
        _socket.set_option(boost::asio::ip::udp::socket::reuse_address(true), ec);
        if(ec.value() != 0){
            std::cerr << fmt::format("get error {}:{}",ec.value(),ec.message()) << std::endl;
            return false;
        }
        _socket.bind(_listen_ep,ec);
        if(ec.value() != 0){
            std::cerr << fmt::format("get error {}:{}",ec.value(),ec.message()) << std::endl;
            return false;
        }
        _socket.async_receive_from(boost::asio::buffer(_data,MAX_LENGTH),_received_ep
            , std::bind(&socket::handle_receive_from, this, std::placeholders::_1, std::placeholders::_2)
        );
        return true;
    }
    // use for sender
    void send_to(const std::string& str,const boost::asio::ip::udp::endpoint& ep){
        send_to(str.c_str(),str.size(),ep);
    }
    void send_to(const void* p,const size_t size,const boost::asio::ip::udp::endpoint& ep){
        _socket.send_to(boost::asio::buffer(p,size),ep);
    }
private:
    void handle_receive_from(const std::error_code &ec, size_t bytes_recvd){
        // std::cout << "receive ep : " << _received_ep << std::endl;
        if (ec.value() == 0){
            if(_callback) std::invoke(_callback,_data.data(),bytes_recvd);
            _socket.async_receive_from(boost::asio::buffer(_data,MAX_LENGTH),_received_ep
                , std::bind(&socket::handle_receive_from, this, std::placeholders::_1, std::placeholders::_2)
            );
        }else{
            std::cerr << fmt::format("get error222 {}:{}",ec.value(),ec.message()) << std::endl;
        }
    }
private:
    boost::asio::ip::udp::endpoint _listen_ep,_received_ep;
    boost::asio::ip::udp::socket _socket;
    enum{
        MAX_LENGTH = 1024
    };
    std::array<char,MAX_LENGTH> _data;
    __callback_type _callback = {};
};
} // namespace zos::udp
} // namespace zos
#endif // __ZOS_UDP_SOCKET_H__
