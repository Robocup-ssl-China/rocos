#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT  __declspec(dllimport)
#else
#define DLL_EXPORT
#define DLL_IMPORT
#endif