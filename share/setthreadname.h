#ifndef __SET_THREAD_NAME_H__
#define __SET_THREAD_NAME_H__
#include <thread>
#ifdef _WIN32
#include <windows.h>

const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)


void SetThreadName(uint32_t dwThreadID, const char* threadName);
void SetThreadName( const char* threadName);

void SetThreadName( std::thread* thread, const char* threadName);
#else
#include <sys/prctl.h>
void SetThreadName(std::thread* thread, const char* threadName);
void SetThreadName( const char* threadName);
#endif
#endif // __SET_THREAD_NAME_H__
