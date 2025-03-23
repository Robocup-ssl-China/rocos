#ifndef __ZOS_POSIX_LINUX_H__
#define __ZOS_POSIX_LINUX_H__
#include <unistd.h>
#include <string>
#include <vector>
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <fmt/core.h>
#include <zos/posix.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

namespace zos{
// write a function that execute a shell command and return the result
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
std::string exec(const std::string& cmd){
    return exec(cmd.c_str());
}
// get the current process id
int getProcessId(){
    return getpid();
}
// get the current process name
std::string getProcessName(){
    auto res = exec(fmt::format("ps -p {} -o comm=",getProcessId()));
    res.pop_back();
    return res;
}
// write a function that return all available network interfaces address
std::vector<std::string> getNetworkInterfacesIP(const int ipType = IPV4){
    struct ifaddrs* ptr_ifaddrs = nullptr;
    auto result = getifaddrs(&ptr_ifaddrs);
    if(result != 0){
        throw std::runtime_error("getifaddrs() failed!");
    }
    std::vector<std::string> res;
    for(auto ptr = ptr_ifaddrs;ptr != nullptr;ptr = ptr->ifa_next){
        if(ipType&IPV4 && ptr->ifa_addr->sa_family == AF_INET){
            char buffer[INET_ADDRSTRLEN] = {0, };
            auto ptr_in_addr = reinterpret_cast<struct sockaddr_in*>(ptr->ifa_addr);
            auto ptr_in_addr_str = inet_ntop(AF_INET,&ptr_in_addr->sin_addr,buffer,INET_ADDRSTRLEN);
            if(ptr_in_addr_str != nullptr){
                res.push_back(ptr_in_addr_str);
            }
        }else if(ipType&IPV6 && ptr->ifa_addr->sa_family == AF_INET6){
            char buffer[INET6_ADDRSTRLEN] = {0, };
            auto ptr_in_addr = reinterpret_cast<struct sockaddr_in6*>(ptr->ifa_addr);
            auto ptr_in_addr_str = inet_ntop(AF_INET6,&ptr_in_addr->sin6_addr,buffer,INET6_ADDRSTRLEN);
            if(ptr_in_addr_str != nullptr){
                res.push_back(ptr_in_addr_str);
            }
        }
    }
    return res;
}
} // namespace zos
#endif // __ZOS_POSIX_LINUX_H__