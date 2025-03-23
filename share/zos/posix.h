#ifndef __ZOS_POSIX_H__
#define __ZOS_POSIX_H__
#include <string>
#include <vector>
namespace zos{
const int IPV4 = 0x01<<0;
const int IPV6 = 0x01<<1;
std::string exec(const char*);
std::string exec(const std::string&);
int getProcessId();
std::string getProcessName();
std::vector<std::string> getNetworkInterfacesIP();
} // namespace zos
// check if we are on linux,windows or macos
// #if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// #include "posix/posix_linux.h"
// #elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
// #include "posix/posix_windows.h"
// #elif defined(__APPLE__) || defined(__MACH__) || defined(macintosh) || defined(Macintosh)
// #include "posix/posix_macos.h"
// #endif
#endif // __ZOS_POSIX_H__
